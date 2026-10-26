#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    QString findText() const;
    void setFindText(const QString &text);
    bool caseSensitive() const;
    bool searchBackward() const;

signals:
    void findRequested(const QString &text, bool caseSensitive, bool backward);
    void replaceRequested(const QString &target, const QString &replacement,
                          bool caseSensitive, bool backward);
    void replaceAllRequested(const QString &target, const QString &replacement,
                             bool caseSensitive);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onFindNextClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    bool checkTargetNotEmpty();

    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
