#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"
#include "themewin.h"

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QLabel>
#include <QMimeData>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QToolBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    applyLightTitleBar(this);

    // 工具栏按钮的悬停 / 选中效果（与实验指导书中的浅蓝底色一致）
    ui->mainToolBar->setStyleSheet(QStringLiteral(
        "QToolButton { padding: 3px; }"
        "QToolButton:hover { background: #E5F1FB; border: 1px solid #C4DDF2; border-radius: 3px; }"
        "QToolButton:checked { background: #CCE4F7; border: 1px solid #99C4E8; border-radius: 3px; }"));

    createStatusBarWidgets();
    // 去掉状态栏各分区之间的竖线，贴近实验指导书里的扁平样式
    statusBar()->setStyleSheet(QStringLiteral("QStatusBar::item { border: none; }"));
    createConnections();

    // “恢复”同时支持 Ctrl+Shift+Z 与 Ctrl+Y
    ui->actionRedo->setShortcuts({QKeySequence(QStringLiteral("Ctrl+Shift+Z")),
                                  QKeySequence(QStringLiteral("Ctrl+Y"))});

    ui->editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    ui->editor->setLineNumbersVisible(true);
    ui->editor->setEditorBackgroundColor(Qt::white);

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    updatePasteAction();

    updateWindowTitle();
    updateStatusInfo();

    ui->editor->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createStatusBarWidgets()
{
    m_labelLength = new QLabel(this);
    m_labelLines = new QLabel(this);
    m_labelPosition = new QLabel(this);
    m_labelAuthor = new QLabel(QStringLiteral("che147"), this);

    m_labelLength->setMinimumWidth(90);
    m_labelLines->setMinimumWidth(76);
    m_labelPosition->setMinimumWidth(120);

    statusBar()->addPermanentWidget(m_labelLength);
    statusBar()->addPermanentWidget(m_labelLines);
    statusBar()->addPermanentWidget(m_labelPosition);
    statusBar()->addPermanentWidget(m_labelAuthor);
    statusBar()->setSizeGripEnabled(true);
}

void MainWindow::createConnections()
{
    // ---------- 文件 ----------
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    // ---------- 编辑 ----------
    connect(ui->actionUndo, &QAction::triggered, ui->editor, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->editor, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->editor, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->editor, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->editor, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->editor, &QPlainTextEdit::selectAll);

    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    // Action 使能状态跟随编辑状态
    connect(ui->editor, &QPlainTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->editor, &QPlainTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(ui->editor, &QPlainTextEdit::copyAvailable, this, [this](bool available) {
        ui->actionCopy->setEnabled(available);
        ui->actionCut->setEnabled(available);
    });
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updatePasteAction);

    // ---------- 格式 ----------
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::chooseFontColor);
    connect(ui->actionFontBackgroundColor, &QAction::triggered,
            this, &MainWindow::chooseFontBackgroundColor);
    connect(ui->actionEditorBackgroundColor, &QAction::triggered,
            this, &MainWindow::chooseEditorBackgroundColor);
    connect(ui->actionWrap, &QAction::toggled, this, &MainWindow::setWordWrapEnabled);
    connect(ui->actionLineNumber, &QAction::toggled, this, &MainWindow::setLineNumbersVisible);

    // ---------- 查看 ----------
    connect(ui->actionToolBar, &QAction::toggled, ui->mainToolBar, &QToolBar::setVisible);
    connect(ui->actionStatusBar, &QAction::toggled, ui->statusbar, &QStatusBar::setVisible);

    // ---------- 帮助 ----------
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        AboutDialog dialog(this);
        dialog.exec();
    });

    // ---------- 状态同步 ----------
    connect(ui->editor, &QPlainTextEdit::textChanged, this, &MainWindow::updateStatusInfo);
    connect(ui->editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusInfo);
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateWindowTitle);
}

// ============================ 状态栏与标题 ============================

QString MainWindow::displayFileName() const
{
    return m_filePath.isEmpty() ? tr("新建文本文件") : QFileInfo(m_filePath).fileName();
}

QString MainWindow::lastUsedDir() const
{
    if (!m_filePath.isEmpty())
        return QFileInfo(m_filePath).absolutePath();
    return QString();
}

void MainWindow::updateWindowTitle()
{
    const QString prefix = ui->editor->document()->isModified() ? QStringLiteral("* ") : QString();
    setWindowTitle(tr("%1%2 - 编辑器").arg(prefix, displayFileName()));
}

void MainWindow::updateStatusInfo()
{
    const QTextCursor cursor = ui->editor->textCursor();
    m_labelLength->setText(tr("length: %1").arg(ui->editor->toPlainText().length()));
    m_labelLines->setText(tr("lines: %1").arg(ui->editor->blockCount()));
    m_labelPosition->setText(tr("Ln: %1  Col: %2")
                             .arg(cursor.blockNumber() + 1)
                             .arg(cursor.positionInBlock() + 1));
}

void MainWindow::updatePasteAction()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mime && mime->hasText());
}

// ============================ 文件操作 ============================

bool MainWindow::maybeSave()
{
    if (!ui->editor->document()->isModified())
        return true;

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(tr("文本编辑器"));
    box.setText(tr("文档“%1”已被修改，是否保存？").arg(displayFileName()));

    QPushButton *btnSave = box.addButton(tr("保存(&S)"), QMessageBox::AcceptRole);
    QPushButton *btnDiscard = box.addButton(tr("不保存(&N)"), QMessageBox::DestructiveRole);
    QPushButton *btnCancel = box.addButton(tr("取消"), QMessageBox::RejectRole);
    box.setDefaultButton(btnSave);
    box.exec();

    if (box.clickedButton() == btnSave)
        return saveFile();
    if (box.clickedButton() == btnCancel)
        return false;

    Q_UNUSED(btnDiscard);
    return true;
}

void MainWindow::newFile()
{
    if (!maybeSave())
        return;

    ui->editor->clear();
    m_filePath.clear();
    ui->editor->document()->setModified(false);
    updateWindowTitle();
    updateStatusInfo();
    statusBar()->showMessage(tr("已新建文件"), 2000);
    ui->editor->setFocus();
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;

    QFileDialog dialog(this, tr("打开文件"), lastUsedDir());
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("文本文件 (*.txt *.md *.cpp *.h *.log);;所有文件 (*.*)"));

    if (dialog.exec() != QDialog::Accepted)
        return;

    const QStringList files = dialog.selectedFiles();
    if (files.isEmpty())
        return;

    loadFile(files.first());
}

bool MainWindow::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("打开文件"),
                             tr("无法打开文件“%1”：\n%2").arg(path, file.errorString()));
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    // 简单的编码兼容：优先按 UTF-8 解码，失败（出现替换字符）时回退到本地编码
    QString text;
    if (data.startsWith("\xEF\xBB\xBF")) {
        text = QString::fromUtf8(data.mid(3));
    } else {
        text = QString::fromUtf8(data);
        if (text.contains(QChar::ReplacementCharacter))
            text = QString::fromLocal8Bit(data);
    }

    ui->editor->setPlainText(text);
    ui->editor->document()->setModified(false);
    m_filePath = path;

    updateWindowTitle();
    updateStatusInfo();
    statusBar()->showMessage(tr("已打开 %1").arg(QFileInfo(path).fileName()), 3000);
    return true;
}

bool MainWindow::saveFile()
{
    if (m_filePath.isEmpty())
        return saveFileAs();

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("保存文件"),
                             tr("无法保存文件“%1”：\n%2")
                                 .arg(m_filePath, file.errorString()));
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);   // Qt 6
#else
    out.setCodec("UTF-8");                     // Qt 5
#endif
    out << ui->editor->toPlainText();
    out.flush();
    file.close();

    ui->editor->document()->setModified(false);
    updateWindowTitle();
    statusBar()->showMessage(tr("已保存到 %1").arg(QFileInfo(m_filePath).fileName()), 3000);
    return true;
}

bool MainWindow::saveFileAs()
{
    QFileDialog dialog(this, tr("另存为"), lastUsedDir());
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("文本文件 (*.txt);;所有文件 (*.*)"));
    dialog.setDefaultSuffix(QStringLiteral("txt"));
    dialog.selectFile(m_filePath.isEmpty() ? tr("新建文本文件.txt") : m_filePath);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    const QStringList files = dialog.selectedFiles();
    if (files.isEmpty())
        return false;

    QString path = files.first();
    if (QFileInfo(path).suffix().isEmpty())
        path += QStringLiteral(".txt");

    m_filePath = path;
    return saveFile();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

// ============================ 查找 / 替换 ============================

void MainWindow::showFindDialog()
{
    if (!m_findDialog) {
        m_findDialog = new FindDialog(this);
        connect(m_findDialog, &FindDialog::findRequested, this,
                [this](const QString &text, bool caseSensitive, bool backward) {
                    findText(text, caseSensitive, backward);
                });
    }

    const QTextCursor cursor = ui->editor->textCursor();
    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        selected.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        m_findDialog->setFindText(selected);
    }

    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}

void MainWindow::showReplaceDialog()
{
    if (!m_replaceDialog) {
        m_replaceDialog = new ReplaceDialog(this);
        connect(m_replaceDialog, &ReplaceDialog::findRequested, this,
                [this](const QString &text, bool caseSensitive, bool backward) {
                    findText(text, caseSensitive, backward);
                });
        connect(m_replaceDialog, &ReplaceDialog::replaceRequested, this,
                [this](const QString &target, const QString &replacement,
                       bool caseSensitive, bool backward) {
                    replaceText(target, replacement, caseSensitive, backward);
                });
        connect(m_replaceDialog, &ReplaceDialog::replaceAllRequested, this,
                [this](const QString &target, const QString &replacement, bool caseSensitive) {
                    replaceAllText(target, replacement, caseSensitive);
                });
    }

    const QTextCursor cursor = ui->editor->textCursor();
    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        selected.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        m_replaceDialog->setFindText(selected);
    }

    m_replaceDialog->show();
    m_replaceDialog->raise();
    m_replaceDialog->activateWindow();
}

bool MainWindow::findText(const QString &target, bool caseSensitive, bool backward,
                          bool showMessage)
{
    if (target.isEmpty())
        return false;

    const QString content = ui->editor->toPlainText();
    const Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    const QTextCursor cursor = ui->editor->textCursor();

    int from = cursor.hasSelection()
                   ? (backward ? cursor.selectionStart() : cursor.selectionEnd())
                   : cursor.position();
    if (from < 0)
        from = 0;
    if (from > content.length())
        from = content.length();

    int index = -1;
    if (backward) {
        index = content.lastIndexOf(target, from - 1, cs);
        if (index < 0)                       // 到达文件头，从末尾继续找
            index = content.lastIndexOf(target, content.length() - 1, cs);
    } else {
        index = content.indexOf(target, from, cs);
        if (index < 0)                       // 到达文件尾，从头继续找
            index = content.indexOf(target, 0, cs);
    }

    if (index < 0) {
        if (showMessage)
            QMessageBox::information(this, tr("查找"), tr("找不到“%1”。").arg(target));
        return false;
    }

    QTextCursor found(ui->editor->document());
    found.setPosition(index);
    found.setPosition(index + target.length(), QTextCursor::KeepAnchor);
    ui->editor->setTextCursor(found);
    ui->editor->ensureCursorVisible();
    return true;
}

void MainWindow::replaceText(const QString &target, const QString &replacement,
                             bool caseSensitive, bool backward)
{
    if (target.isEmpty())
        return;

    const Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextCursor cursor = ui->editor->textCursor();

    if (cursor.hasSelection()) {
        QString selected = cursor.selectedText();
        selected.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        if (QString::compare(selected, target, cs) == 0) {
            cursor.insertText(replacement);
            ui->editor->setTextCursor(cursor);
        }
    }

    // 替换成功后自动定位到下一处
    findText(target, caseSensitive, backward);
}

void MainWindow::replaceAllText(const QString &target, const QString &replacement,
                                bool caseSensitive)
{
    if (target.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor editCursor(ui->editor->document());
    editCursor.beginEditBlock();

    int count = 0;
    QTextCursor found = ui->editor->document()->find(target, editCursor, flags);
    while (!found.isNull()) {
        found.insertText(replacement);
        ++count;
        found = ui->editor->document()->find(target, found, flags);
    }

    editCursor.endEditBlock();

    if (count == 0)
        QMessageBox::information(this, tr("替换"), tr("找不到“%1”。").arg(target));
    else
        QMessageBox::information(this, tr("替换"), tr("已替换 %1 处。").arg(count));
}

// ============================ 字体与颜色 ============================

void MainWindow::applyCharFormat(const QTextCharFormat &format)
{
    QTextCursor original = ui->editor->textCursor();
    QTextCursor cursor = original;
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::Document);

    cursor.mergeCharFormat(format);
    ui->editor->mergeCurrentCharFormat(format);
    ui->editor->setTextCursor(original);
    ui->editor->setFocus();
}

void MainWindow::chooseFont()
{
    QFont current = ui->editor->currentCharFormat().font();
    if (current.family().isEmpty() || current.pointSize() <= 0)
        current = ui->editor->font();

    QFontDialog dialog(current, this);
    dialog.setOption(QFontDialog::DontUseNativeDialog, true);
    dialog.setWindowTitle(tr("选择字体"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QTextCharFormat format;
    format.setFont(dialog.selectedFont());
    applyCharFormat(format);
}

void MainWindow::chooseFontColor()
{
    QColor current = ui->editor->currentCharFormat().foreground().color();
    if (!current.isValid())
        current = palette().color(QPalette::Text);

    QColorDialog dialog(current, this);
    dialog.setOption(QColorDialog::DontUseNativeDialog, true);
    dialog.setWindowTitle(tr("选择字体颜色"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QTextCharFormat format;
    format.setForeground(dialog.selectedColor());
    applyCharFormat(format);
}

void MainWindow::chooseFontBackgroundColor()
{
    QColorDialog dialog(Qt::yellow, this);
    dialog.setOption(QColorDialog::DontUseNativeDialog, true);
    dialog.setWindowTitle(tr("选择字体背景色"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    QTextCharFormat format;
    format.setBackground(dialog.selectedColor());
    applyCharFormat(format);
}

void MainWindow::chooseEditorBackgroundColor()
{
    QColorDialog dialog(Qt::white, this);
    dialog.setOption(QColorDialog::DontUseNativeDialog, true);
    dialog.setWindowTitle(tr("选择编辑器背景色"));
    if (dialog.exec() != QDialog::Accepted)
        return;

    ui->editor->setEditorBackgroundColor(dialog.selectedColor());
}

void MainWindow::setWordWrapEnabled(bool enabled)
{
    ui->editor->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth
                                        : QPlainTextEdit::NoWrap);
}

void MainWindow::setLineNumbersVisible(bool visible)
{
    ui->editor->setLineNumbersVisible(visible);
}
