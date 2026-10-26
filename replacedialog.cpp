#include "replacedialog.h"
#include "ui_replacedialog.h"
#include "themewin.h"

#include <QMessageBox>
#include <QShowEvent>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);
    applyLightTitleBar(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedSize(sizeHint());

    connect(ui->btnFindNext, &QPushButton::clicked, this, &ReplaceDialog::onFindNextClicked);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &ReplaceDialog::onFindNextClicked);
    connect(ui->btnReplace, &QPushButton::clicked, this, &ReplaceDialog::onReplaceClicked);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAllClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

QString ReplaceDialog::findText() const
{
    return ui->editFind->text();
}

void ReplaceDialog::setFindText(const QString &text)
{
    ui->editFind->setText(text);
    ui->editFind->selectAll();
}

bool ReplaceDialog::caseSensitive() const
{
    return ui->checkCaseSensitive->isChecked();
}

bool ReplaceDialog::searchBackward() const
{
    return ui->radioUp->isChecked();
}

void ReplaceDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    ui->editFind->setFocus();
    ui->editFind->selectAll();
}

bool ReplaceDialog::checkTargetNotEmpty()
{
    if (!ui->editFind->text().isEmpty())
        return true;

    QMessageBox::information(this, tr("替换"), tr("请输入要查找的内容。"));
    ui->editFind->setFocus();
    return false;
}

void ReplaceDialog::onFindNextClicked()
{
    if (!checkTargetNotEmpty())
        return;

    emit findRequested(ui->editFind->text(), caseSensitive(), searchBackward());
}

void ReplaceDialog::onReplaceClicked()
{
    if (!checkTargetNotEmpty())
        return;

    emit replaceRequested(ui->editFind->text(), ui->editReplace->text(),
                          caseSensitive(), searchBackward());
}

void ReplaceDialog::onReplaceAllClicked()
{
    if (!checkTargetNotEmpty())
        return;

    emit replaceAllRequested(ui->editFind->text(), ui->editReplace->text(),
                             caseSensitive());
}
