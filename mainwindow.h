/*主窗口*/
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
    QSystemTrayIcon *trayIcon;//托盘应用管理
    SettingsDialog *settings;//设置
    AboutDialog *about;//关于窗口
    DonateDialog *donate;//捐赠窗口
    Grabber *grabber;//屏幕抓取
    BaiduOCR *baiduOCR;//OCR识别
    BaiduTrans *baiduTrans;//翻译
    struct{
        bool isPixmap;//标识当前结果为图像还是文字 true:图像 false:文字
        QPixmap pixmap;
        QString text;
    }tempRes;//暂存识别结果
    bool isFloating;//是否在悬浮窗模式
    void writeDefaultSettings();//写入默认设置
    void applySettings();//读取设置并应用
    void switchMode(SettingsDialog::SysMode mode);//切换系统模式(截图/OCR/翻译)
    void initTray();//初始化托盘
    void setOCRLang(const QString &lang);//设置OCR识别语言

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void slotGrabFinished(QPixmap &pixmap);
    void slotBaiduOCRRecv(const QString &resStr);
    void slotBaiduTransRecv(const QString &resStr);
    void slotSettingsAccepted();
    void slotActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason);
    void slotBaiduOCRError(BaiduOCR::Error error,const QString &errInfo);
    void slotBaiduTransError(BaiduTrans::Error error,const QString &errInfo);
    void on_actionOCR_triggered();
    void on_actionGrab_triggered();
    void on_actionTrans_triggered();
    void on_actionSettings_triggered();
    void on_actionExit_triggered();
    void on_btn_copy_clicked();
    void on_actionFloat_triggered(bool checked);
    void on_actionFeedback_triggered();
    void on_actionAbout_triggered();
    void on_actionDonate_triggered();
    void on_actionHelp_triggered();
};
#endif // MAINWINDOW_H
