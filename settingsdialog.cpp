#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "qfileinfo.h"
#include "qcolordialog.h"

const QString confFilePath="conf.ini";

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("设置");
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::confExist()
{
    QFileInfo file(confFilePath);
    return file.exists();
}

void SettingsDialog::loadSettings()
{
    QSettings settings(confFilePath,QSettings::IniFormat);
    settings.beginGroup("settings");
    values.trans_srcLang=settings.value("trans_srcLang").toString();
    values.trans_dstLang=settings.value("trans_dstLang").toString();
    values.ocr_apiKey=settings.value("ocr_apiKey").toString();
    values.ocr_secretKey=settings.value("ocr_secretKey").toString();
    values.ocr_lang=settings.value("ocr_lang").toString();
    values.cbKey_ctrl=settings.value("cbKey_ctrl").toBool();
    values.cbKey_shift=settings.value("cbKey_shift").toBool();
    values.cbKey_alt=settings.value("cbKey_alt").toBool();
    values.rectLineStyle=(Qt::PenStyle)settings.value("rectLineStyle").toInt();
    values.rectLineWidth=settings.value("rectLineWidth").toInt();
    values.rectLineColor=QColor(settings.value("rectLineColor").toString());
    values.autoCopy=settings.value("autoCopy").toBool();
    values.sysMode=(SysMode)settings.value("sysMode").toInt();
    values.floatOpacity=settings.value("floatOpacity").toDouble();
}

void SettingsDialog::saveSettings()
{
    QSettings settings(confFilePath,QSettings::IniFormat);
    settings.clear();
    settings.beginGroup("settings");
    settings.setValue("trans_srcLang",values.trans_srcLang);
    settings.setValue("trans_dstLang",values.trans_dstLang);
    settings.setValue("ocr_apiKey",values.ocr_apiKey);
    settings.setValue("ocr_secretKey",values.ocr_secretKey);
    settings.setValue("ocr_lang",values.ocr_lang);
    settings.setValue("cbKey_ctrl",values.cbKey_ctrl);
    settings.setValue("cbKey_shift",values.cbKey_shift);
    settings.setValue("cbKey_alt",values.cbKey_alt);
    settings.setValue("rectLineStyle",(int)values.rectLineStyle);
    settings.setValue("rectLineColor",values.rectLineColor.name());
    settings.setValue("rectLineWidth",values.rectLineWidth);
    settings.setValue("autoCopy",values.autoCopy);
    settings.setValue("sysMode",values.sysMode);
    settings.setValue("floatOpacity",values.floatOpacity);
    settings.sync();
}

void SettingsDialog::display()
{
    show();
    ui->txt_apiKey->setText(values.ocr_apiKey);
    ui->txt_secretKey->setText(values.ocr_secretKey);
    ui->combo_ocrLang->setCurrentText(values.ocr_lang);
    ui->combo_srcLang->setCurrentText(values.trans_srcLang);
    ui->combo_dstLang->setCurrentText(values.trans_dstLang);
    ui->check_ctrl->setChecked(values.cbKey_ctrl);
    ui->check_shift->setChecked(values.cbKey_shift);
    ui->check_alt->setChecked(values.cbKey_alt);
    ui->combo_lineStyle->setCurrentIndex(values.rectLineStyle-1);
    ui->spin_lineWidth->setValue(values.rectLineWidth);
    ui->btn_lineColor->setStyleSheet("font-weight:bold;color:"+values.rectLineColor.name());
    ui->btn_lineColor->setText(values.rectLineColor.name());
    ui->check_autoCopy->setChecked(values.autoCopy);
    ui->spin_opacity->setValue(values.floatOpacity);
}

void SettingsDialog::on_btn_lineColor_clicked()
{
    QColorDialog colorDialog(this);
    colorDialog.setWindowTitle("请选择颜色");
    colorDialog.setCurrentColor(QColor(ui->btn_lineColor->text()));
    if(colorDialog.exec()==QColorDialog::Accepted)
    {
        QColor color=colorDialog.selectedColor();
        ui->btn_lineColor->setStyleSheet("font-weight:bold;color:"+color.name());
        ui->btn_lineColor->setText(color.name());
    }
}

void SettingsDialog::on_btnAccept_clicked()
{
    values.ocr_apiKey=ui->txt_apiKey->text();
    values.ocr_secretKey=ui->txt_secretKey->text();
    values.ocr_lang=ui->combo_ocrLang->currentText();
    values.trans_srcLang=ui->combo_srcLang->currentText();
    values.trans_dstLang=ui->combo_dstLang->currentText();
    values.cbKey_ctrl=ui->check_ctrl->checkState();
    values.cbKey_shift=ui->check_shift->checkState();
    values.cbKey_alt=ui->check_alt->checkState();
    values.rectLineStyle=(Qt::PenStyle)(ui->combo_lineStyle->currentIndex()+1);
    values.rectLineWidth=ui->spin_lineWidth->value();
    values.rectLineColor=QColor(ui->btn_lineColor->text());
    values.autoCopy=ui->check_autoCopy->checkState();
    values.floatOpacity=ui->spin_opacity->value();
    saveSettings();
    emit sigSettingsAccepted();
    hide();
}

void SettingsDialog::on_btnCancel_clicked()
{
    hide();
}
