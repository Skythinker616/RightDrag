#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowTitle("关于");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_btnReturn_clicked()
{
    hide();
}
