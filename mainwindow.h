#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "grab/grabber.h"
#include "net/ocr.h"
#include "net/trans.h"
#include "qsystemtrayicon.h"
#include "aboutdialog.h"
#include "donatedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    SettingsDialog *settings;
    AboutDialog *about;
    DonateDialog *donate;
    Grabber *grabber;
    BaiduOCR *baiduOCR;
    BaiduTrans *baiduTrans;
    struct{
        bool isPixmap;
        QPixmap pixmap;
        QString text;
    }tempRes;
    bool isFloating;
    void writeDefaultSettings();
    void applySettings();
    void switchMode(SettingsDialog::SysMode mode);
    void initTray();

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionOCR_triggered();
    void on_actionGrab_triggered();
    void on_actionTrans_triggered();
    void on_actionSettings_triggered();
    void on_actionExit_triggered();
    void on_btn_copy_clicked();
    void slotGrabFinished(QPixmap &pixmap);
    void slotBaiduOCRRecv(const QString &resStr);
    void slotBaiduTransRecv(const QString &resStr);
    void slotSettingsAccepted();
    void slotActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void slotBaiduOCRError(BaiduOCR::Error error,const QString &errInfo);
    void slotBaiduTransError(BaiduTrans::Error error,const QString &errInfo);
    void on_actionFloat_triggered(bool checked);
    void on_actionFeedback_triggered();
    void on_actionAbout_triggered();
    void on_actionDonate_triggered();
    void on_actionHelp_triggered();
};
#endif // MAINWINDOW_H
