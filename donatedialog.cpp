#include "donatedialog.h"
#include "ui_donatedialog.h"

DonateDialog::DonateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DonateDialog)
{
    ui->setupUi(this);
    wechatPixmap=QPixmap("wechat.png");
    alipayPixmap=QPixmap("alipay.png");
    setWindowTitle("捐赠");
    setAttribute(Qt::WA_DeleteOnClose,true);
}

DonateDialog::~DonateDialog()
{
    delete ui;
}

//显示图片
void DonateDialog::setImg()
{
    ui->labelWechat->setPixmap(wechatPixmap.scaled(ui->scroll_wechat->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->labelAlipay->setPixmap(alipayPixmap.scaled(ui->scroll_aliplay->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void DonateDialog::on_pushButton_clicked()
{
    close();
}

//窗口大小改变事件，重绘图片
void DonateDialog::resizeEvent(QResizeEvent *)
{
    setImg();
}
