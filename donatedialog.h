/*捐赠窗口*/
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
    void setImg();//根据控件大小显示图片

private slots:
    void on_pushButton_clicked();

private:
    Ui::DonateDialog *ui;
    QPixmap wechatPixmap,alipayPixmap;//存两个方式的捐赠二维码图片

protected:
    void resizeEvent(QResizeEvent *);
};

#endif // DONATEDIALOG_H
