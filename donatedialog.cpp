#include "donatedialog.h"
#include "ui_donatedialog.h"

DonateDialog::DonateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DonateDialog)
{
    ui->setupUi(this);
    wechatPixmap=new QPixmap("wechat.png");
    alipayPixmap=new QPixmap("alipay.png");
    setWindowTitle("捐赠");
}

DonateDialog::~DonateDialog()
{
    delete ui;
}

void DonateDialog::setImg()
{
    ui->labelWechat->setPixmap(wechatPixmap->scaled(ui->labelWechat->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->labelAlipay->setPixmap(alipayPixmap->scaled(ui->labelAlipay->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void DonateDialog::on_pushButton_clicked()
{
    hide();
}
