/*关于信息窗口*/
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private slots:
    void on_btnReturn_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
