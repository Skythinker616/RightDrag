/*设置*/
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "qsettings.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
    enum SysMode{//系统模式
        GrabOnly,//框选截图
        OCR,//进行OCR识别
        Translate//翻译
    };
    struct{//所有设置
        QString version;//应用版本
        QString trans_srcLang,trans_dstLang;//翻译语言
        QString ocr_apiKey,ocr_secretKey,ocr_lang;//ocr认证key、识别语言
        bool cbKey_ctrl,cbKey_shift,cbKey_alt;//组合键设置
        Qt::PenStyle rectLineStyle;//选框样式
        QColor rectLineColor;
        int rectLineWidth;
        bool autoCopy;//自动复制
        double floatOpacity;//浮窗不透明度
        SysMode sysMode;//系统模式
        bool blockMouse;//是否拦截鼠标
    }values;
    bool confExist();//检查是否已有设置
    void loadSettings();//加载设置(到values)
    void saveSettings();//保存设置(从values)
    void display();//显示设置界面

signals:
    void sigSettingsAccepted();

private slots:
    void on_btn_lineColor_clicked();
    void on_btnAccept_clicked();

    void on_btnCancel_clicked();

private:
    Ui::SettingsDialog *ui;
    QSettings *settings;
};

#endif // SETTINGSDIALOG_H
