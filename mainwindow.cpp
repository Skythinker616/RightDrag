#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qclipboard.h"
#include <windows.h>
#include "qdesktopservices.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings=new SettingsDialog();
    about=new AboutDialog();
    donate=new DonateDialog();

    grabber=new Grabber();
    baiduOCR=new BaiduOCR();
    baiduTrans=new BaiduTrans();

    isFloating=false;

    connect(settings,SIGNAL(sigSettingsAccepted()),this,SLOT(slotSettingsAccepted()));
    connect(grabber,SIGNAL(sigGrabFinished(QPixmap&)),this,SLOT(slotGrabFinished(QPixmap&)));
    connect(baiduOCR,SIGNAL(recvSuccess(const QString&)),this,SLOT(slotBaiduOCRRecv(const QString&)));
    connect(baiduOCR,SIGNAL(error(BaiduOCR::Error,const QString&)),this,SLOT(slotBaiduOCRError(BaiduOCR::Error,const QString&)));
    connect(baiduTrans,SIGNAL(recvSuccess(const QString&)),this,SLOT(slotBaiduTransRecv(const QString&)));
    connect(baiduTrans,SIGNAL(error(BaiduTrans::Error,const QString&)),this,SLOT(slotBaiduTransError(BaiduTrans::Error,const QString&)));

    if(!settings->confExist())
        writeDefaultSettings();
    else
        settings->loadSettings();
    applySettings();

    baiduOCR->sendAuth();
    switchMode(settings->values.sysMode);

    initTray();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete grabber;
    delete baiduOCR;
    delete baiduTrans;
    delete settings;
}

void MainWindow::writeDefaultSettings()
{
    settings->values.trans_srcLang="英语";
    settings->values.trans_dstLang="中文";
    settings->values.ocr_apiKey="8kEopqDXdK8VvDc3pI7AdPX8";
    settings->values.ocr_secretKey="ecN4DOkqqPq6QRu7IqM4etmRLmo0SyFT";
    settings->values.ocr_lang="中英混合";
    settings->values.cbKey_ctrl=false;
    settings->values.cbKey_shift=false;
    settings->values.cbKey_alt=false;
    settings->values.rectLineStyle=Qt::PenStyle::DotLine;
    settings->values.rectLineWidth=8;
    settings->values.rectLineColor=QColor("#0055FF");
    settings->values.autoCopy=true;
    settings->values.sysMode=SettingsDialog::OCR;
    settings->values.floatOpacity=0.8;
}

void MainWindow::applySettings()
{
    baiduOCR->setKeys(settings->values.ocr_apiKey,settings->values.ocr_secretKey);
    baiduOCR->setLanguage(settings->values.ocr_lang);
    baiduTrans->setSrcLang(settings->values.trans_srcLang);
    baiduTrans->setDstLang(settings->values.trans_dstLang);
    grabber->setRectColor(settings->values.rectLineColor);
    grabber->setRectStyle(settings->values.rectLineStyle);
    grabber->setRectWidth(settings->values.rectLineWidth);
    grabber->combineKey.ctrl=settings->values.cbKey_ctrl;
    grabber->combineKey.shift=settings->values.cbKey_shift;
    grabber->combineKey.alt=settings->values.cbKey_alt;
    if(isFloating) setWindowOpacity(settings->values.floatOpacity);
}

void MainWindow::switchMode(SettingsDialog::SysMode mode)
{
    ui->actionOCR->setChecked(mode==SettingsDialog::OCR);
    ui->actionTrans->setChecked(mode==SettingsDialog::Translate);
    ui->actionGrab->setChecked(mode==SettingsDialog::GrabOnly);

    if(mode==SettingsDialog::Translate)
    {
        if(settings->values.trans_srcLang=="中文"||
           settings->values.trans_srcLang=="文言文"||
           settings->values.trans_srcLang=="繁体中文"||
           settings->values.trans_srcLang=="英语")
        {
            baiduOCR->setLanguage("中英混合");
        }
        else
            baiduOCR->setLanguage(settings->values.trans_srcLang);
    }
    else if(mode==SettingsDialog::OCR)
    {
        baiduOCR->setLanguage(settings->values.ocr_lang);
    }

    settings->values.sysMode=mode;
    settings->saveSettings();
}

void MainWindow::initTray()
{
    trayIcon=new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("icon.png"));
    trayIcon->setToolTip("框选识别");
    QMenu *trayMenu=new QMenu(this);
    trayMenu->addAction(ui->actionGrab);
    trayMenu->addAction(ui->actionOCR);
    trayMenu->addAction(ui->actionTrans);
    trayMenu->addSeparator();
    trayMenu->addAction(ui->actionFloat);
    trayMenu->addSeparator();
    trayMenu->addAction(ui->actionExit);
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

}

void MainWindow::changeEvent(QEvent *event)
{
     if(event->type()!=QEvent::WindowStateChange) return;
     if(this->windowState()==Qt::WindowMinimized)
     {
        hide();
     }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if(!isFloating)
        QApplication::quit();
}

void MainWindow::on_actionOCR_triggered()
{
    if(settings->values.sysMode!=SettingsDialog::OCR)
        switchMode(SettingsDialog::OCR);
}

void MainWindow::on_actionGrab_triggered()
{
    if(settings->values.sysMode!=SettingsDialog::GrabOnly)
        switchMode(SettingsDialog::GrabOnly);
}

void MainWindow::on_actionTrans_triggered()
{
    if(settings->values.sysMode!=SettingsDialog::Translate)
        switchMode(SettingsDialog::Translate);
}

void MainWindow::on_actionSettings_triggered()
{
    settings->display();
}

void MainWindow::slotGrabFinished(QPixmap &pixmap)
{
    switch(settings->values.sysMode)
    {
        case SettingsDialog::GrabOnly:{
            QPixmap fitPixmap=pixmap.scaled(ui->scrollArea->width()-10,ui->scrollArea->height()-10,Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->labelResult->setAlignment(Qt::AlignCenter);
            ui->labelResult->setPixmap(fitPixmap);
            tempRes.isPixmap=true;
            tempRes.pixmap=pixmap;
            if(settings->values.autoCopy)
                QApplication::clipboard()->setPixmap(pixmap);
            }
        break;
        case SettingsDialog::OCR:
            baiduOCR->sendPic(pixmap);
        break;
        case SettingsDialog::Translate:
            baiduOCR->sendPic(pixmap);
        break;
    }
}

void MainWindow::slotBaiduOCRRecv(const QString &resStr)
{
    if(settings->values.sysMode==SettingsDialog::OCR)
    {
        ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);
        ui->labelResult->setText(resStr);
        tempRes.isPixmap=false;
        tempRes.text=resStr;
        if(settings->values.autoCopy)
            QApplication::clipboard()->setText(resStr);
    }
    else if(settings->values.sysMode==SettingsDialog::Translate)
    {
        baiduTrans->sendTrans(resStr);
    }
}

void MainWindow::slotBaiduTransRecv(const QString &resStr)
{
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    ui->labelResult->setText(resStr);
    tempRes.isPixmap=false;
    tempRes.text=resStr;
    if(settings->values.autoCopy)
        QApplication::clipboard()->setText(resStr);
}

void MainWindow::slotSettingsAccepted()
{
    applySettings();
    baiduOCR->sendAuth();
}

void MainWindow::slotActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            show();
            setWindowState(Qt::WindowNoState);
        break;
        default:
        break;
    }
}

void MainWindow::slotBaiduOCRError(BaiduOCR::Error error, const QString &errInfo)
{
    QString displayStr;
    switch(error)
    {
        case BaiduOCR::Err_Network:
            displayStr+="网络故障，请检查网络。";
        break;
        case BaiduOCR::Err_JsonParseFailed:
            displayStr+="数据解析错误。";
        break;
        case BaiduOCR::Err_AuthFailed:
            displayStr+="认证失败，请检查OCR的Api_Key和Secret_Key。";
        break;
        case BaiduOCR::Err_Unknown:
            displayStr+="未知错误。";
        break;
        case BaiduOCR::Err_SizeWrong:
        break;
    }
    if(!errInfo.isEmpty())
        displayStr+="\n"+errInfo;
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    ui->labelResult->setText(displayStr);
}

void MainWindow::slotBaiduTransError(BaiduTrans::Error error, const QString &errInfo)
{
    QString displayStr;
    switch(error)
    {
        case BaiduTrans::Err_Network:
            displayStr+="网络故障，请检查网络。\n";
        break;
        case BaiduTrans::Err_JsonParseFailed:
            displayStr+="数据解析错误。\n";
        break;
        case BaiduTrans::Err_Unknown:
            displayStr+="未知错误。\n";
        break;
    }
    if(!errInfo.isEmpty())
        displayStr+="错误信息："+errInfo;
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    ui->labelResult->setText(displayStr);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_btn_copy_clicked()
{
    if(tempRes.isPixmap)
        QApplication::clipboard()->setPixmap(tempRes.pixmap);
    else
        QApplication::clipboard()->setText(tempRes.text);
}


void MainWindow::on_actionFloat_triggered(bool checked)
{
    isFloating=checked;
    if(checked)
    {
        setWindowFlags(windowFlags()|Qt::Tool|Qt::WindowStaysOnTopHint);
        setWindowOpacity(settings->values.floatOpacity);
        show();
    }
    else
    {
        setWindowFlags(windowFlags()&~(Qt::Tool|Qt::WindowStaysOnTopHint));
        setWindowOpacity(1);
        show();
    }
}

void MainWindow::on_actionFeedback_triggered()
{
    QDesktopServices::openUrl(QUrl("https://support.qq.com/product/172973"));
}

void MainWindow::on_actionAbout_triggered()
{
    about->show();
}

void MainWindow::on_actionDonate_triggered()
{
    donate->show();
    donate->setImg();
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/BowenLan/RightDrag/blob/master/help.md"));
}
