#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QLabel;
class QCloseEvent;
class QTextCharFormat;
class FindDialog;
class ReplaceDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    // 编辑
    void showFindDialog();
    void showReplaceDialog();
    // 格式
    void chooseFont();
    void chooseFontColor();
    void chooseFontBackgroundColor();
    void chooseEditorBackgroundColor();
    void setWordWrapEnabled(bool enabled);
    void setLineNumbersVisible(bool visible);
    // 状态显示
    void updateStatusInfo();
    void updateWindowTitle();
    void updatePasteAction();

private:
    void createStatusBarWidgets();
    void createConnections();

    bool maybeSave();
    bool loadFile(const QString &path);
    bool findText(const QString &target, bool caseSensitive, bool backward,
                  bool showMessage = true);
    void replaceText(const QString &target, const QString &replacement,
                     bool caseSensitive, bool backward);
    void replaceAllText(const QString &target, const QString &replacement,
                        bool caseSensitive);
    void applyCharFormat(const QTextCharFormat &format);

    QString displayFileName() const;
    QString lastUsedDir() const;

    Ui::MainWindow *ui;
    QString m_filePath;

    QLabel *m_labelLength = nullptr;
    QLabel *m_labelLines = nullptr;
    QLabel *m_labelPosition = nullptr;
    QLabel *m_labelAuthor = nullptr;

    FindDialog *m_findDialog = nullptr;
    ReplaceDialog *m_replaceDialog = nullptr;
};

#endif // MAINWINDOW_H
