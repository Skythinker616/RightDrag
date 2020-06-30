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
    enum SysMode{
        GrabOnly,
        OCR,
        Translate
    };
    struct{
        QString trans_srcLang,trans_dstLang;
        QString ocr_apiKey,ocr_secretKey,ocr_lang;
        bool cbKey_ctrl,cbKey_shift,cbKey_alt;
        Qt::PenStyle rectLineStyle;
        QColor rectLineColor;
        int rectLineWidth;
        bool autoCopy;
        double floatOpacity;
        SysMode sysMode;
    }values;
    bool confExist();
    void loadSettings();
    void saveSettings();
    void display();

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
