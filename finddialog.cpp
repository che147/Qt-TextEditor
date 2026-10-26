#include "finddialog.h"
#include "ui_finddialog.h"
#include "themewin.h"

#include <QMessageBox>
#include <QShowEvent>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    applyLightTitleBar(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedSize(sizeHint());

    connect(ui->btnFindNext, &QPushButton::clicked, this, &FindDialog::onFindNextClicked);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &FindDialog::onFindNextClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

FindDialog::~FindDialog()
{
    delete ui;
}

QString FindDialog::findText() const
{
    return ui->editFind->text();
}

void FindDialog::setFindText(const QString &text)
{
    ui->editFind->setText(text);
    ui->editFind->selectAll();
}

bool FindDialog::caseSensitive() const
{
    return ui->checkCaseSensitive->isChecked();
}

bool FindDialog::searchBackward() const
{
    return ui->radioUp->isChecked();
}

void FindDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    // 每次显示时自动聚焦并把已有内容全选，方便直接输入新的查找目标
    ui->editFind->setFocus();
    ui->editFind->selectAll();
}

void FindDialog::onFindNextClicked()
{
    const QString target = ui->editFind->text();
    if (target.isEmpty()) {
        QMessageBox::information(this, tr("查找"), tr("请输入要查找的内容。"));
        ui->editFind->setFocus();
        return;
    }

    emit findRequested(target, caseSensitive(), searchBackward());
}
