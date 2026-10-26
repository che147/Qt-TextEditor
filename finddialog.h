#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    QString findText() const;
    void setFindText(const QString &text);
    bool caseSensitive() const;
    bool searchBackward() const;

signals:
    // 点击“查找下一个”时发出，由主窗口完成真正的查找
    void findRequested(const QString &text, bool caseSensitive, bool backward);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onFindNextClicked();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
