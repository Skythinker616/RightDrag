#ifndef DONATEDIALOG_H
#define DONATEDIALOG_H

#include <QDialog>

namespace Ui {
class DonateDialog;
}

class DonateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DonateDialog(QWidget *parent = nullptr);
    ~DonateDialog();
    void setImg();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DonateDialog *ui;
    QPixmap *wechatPixmap,*alipayPixmap;
};

#endif // DONATEDIALOG_H
