#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "themewin.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    applyLightTitleBar(this);

    // 去掉标题栏上的“?”帮助按钮，保持和文档截图一致的简洁外观
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setFixedSize(sizeHint());

    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
