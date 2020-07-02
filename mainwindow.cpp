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

    //new各个对象
    settings=new SettingsDialog();
    grabber=new Grabber();
    baiduOCR=new BaiduOCR();
    baiduTrans=new BaiduTrans();

    isFloating=false;//初始状态为非悬浮窗模式

    connect(settings,SIGNAL(sigSettingsAccepted()),this,SLOT(slotSettingsAccepted()));//设置确认信号
    connect(grabber,SIGNAL(sigGrabFinished(QPixmap&)),this,SLOT(slotGrabFinished(QPixmap&)));//截图完成信号
    connect(baiduOCR,SIGNAL(recvSuccess(const QString&)),this,SLOT(slotBaiduOCRRecv(const QString&)));//OCR识别成功信号
    connect(baiduOCR,SIGNAL(error(BaiduOCR::Error,const QString&)),this,SLOT(slotBaiduOCRError(BaiduOCR::Error,const QString&)));//OCR错误信号
    connect(baiduTrans,SIGNAL(recvSuccess(const QString&)),this,SLOT(slotBaiduTransRecv(const QString&)));//翻译成功信号
    connect(baiduTrans,SIGNAL(error(BaiduTrans::Error,const QString&)),this,SLOT(slotBaiduTransError(BaiduTrans::Error,const QString&)));//翻译错误信号

    if(!settings->confExist())//若配置文件不存在则使用默认设置
        writeDefaultSettings();
    else
    {
        settings->loadSettings();//读取设置
        if(settings->values.version!=QString(APP_VERSION))//若存在的配置文件版本不符则使用默认设置
        {
            writeDefaultSettings();
            settings->saveSettings();
        }
    }
    applySettings();//应用设置

    baiduOCR->sendAuth();//发送OCR认证请求
    switchMode(settings->values.sysMode);

    initTray();//初始化托盘
}

MainWindow::~MainWindow()
{
    delete ui;
    delete grabber;
    delete baiduOCR;
    delete baiduTrans;
    delete settings;
}

//将默认设置写入settings中
void MainWindow::writeDefaultSettings()
{
    settings->values.version=APP_VERSION;
    settings->values.trans_srcLang="英语";
    settings->values.trans_dstLang="中文";
    settings->values.ocr_apiKey="8kEopqDXdK8VvDc3pI7AdPX8";
    settings->values.ocr_secretKey="ecN4DOkqqPq6QRu7IqM4etmRLmo0SyFT";
    settings->values.ocr_lang="中英混合";
    settings->values.cbKey_ctrl=true;
    settings->values.cbKey_shift=false;
    settings->values.cbKey_alt=false;
    settings->values.rectLineStyle=Qt::PenStyle::DotLine;
    settings->values.rectLineWidth=8;
    settings->values.rectLineColor=QColor("#0055FF");
    settings->values.autoCopy=true;
    settings->values.sysMode=SettingsDialog::OCR;
    settings->values.floatOpacity=0.8;
    settings->values.blockMouse=true;
}

//应用设置，将settings->values中的值应用到各个模块
void MainWindow::applySettings()
{
    baiduOCR->setKeys(settings->values.ocr_apiKey,settings->values.ocr_secretKey);
    if(settings->values.sysMode==SettingsDialog::Translate)//翻译模式OCR识别语言也要调整为翻译源语言
        setOCRLang(settings->values.trans_srcLang);
    else
        setOCRLang(settings->values.ocr_lang);
    baiduTrans->setSrcLang(settings->values.trans_srcLang);
    baiduTrans->setDstLang(settings->values.trans_dstLang);
    grabber->setRectColor(settings->values.rectLineColor);
    grabber->setRectStyle(settings->values.rectLineStyle);
    grabber->setRectWidth(settings->values.rectLineWidth);
    grabber->combineKey.ctrl=settings->values.cbKey_ctrl;
    grabber->combineKey.shift=settings->values.cbKey_shift;
    grabber->combineKey.alt=settings->values.cbKey_alt;
    grabber->blockMouseEvent=settings->values.blockMouse;
    if(isFloating) setWindowOpacity(settings->values.floatOpacity);
}

//切换系统模式
void MainWindow::switchMode(SettingsDialog::SysMode mode)
{
    //设置action勾选情况
    ui->actionOCR->setChecked(mode==SettingsDialog::OCR);
    ui->actionTrans->setChecked(mode==SettingsDialog::Translate);
    ui->actionGrab->setChecked(mode==SettingsDialog::GrabOnly);

    if(mode==SettingsDialog::Translate)//若切换到翻译模式，则OCR识别语言也要调整为翻译源语言
        setOCRLang(settings->values.trans_srcLang);
    else if(mode==SettingsDialog::OCR)//若切换到OCR模式，则识别语言即为设置的识别语言
        setOCRLang(settings->values.ocr_lang);

    settings->values.sysMode=mode;
    settings->saveSettings();
}

//初始化托盘
void MainWindow::initTray()
{
    trayIcon=new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("icon.png"));
    trayIcon->setToolTip("框选识别");

    QMenu *trayMenu=new QMenu(this);//添加托盘菜单
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

//设置OCR识别语言
void MainWindow::setOCRLang(const QString &lang)
{
    if(lang=="中文"||lang=="文言文"||lang=="繁体中文"||lang=="英语")//将翻译特有语言种类转为OCR支持的语言
        baiduOCR->setLanguage("中英混合");
    else
        baiduOCR->setLanguage(lang);
}

//最小化自动隐藏
void MainWindow::changeEvent(QEvent *event)
{
     if(event->type()!=QEvent::WindowStateChange) return;
     if(this->windowState()==Qt::WindowMinimized)
     {
        hide();
     }
}

//非悬浮窗模式下窗口关闭则程序退出
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if(!isFloating)
        QApplication::quit();
}

//窗口大小改变，若显示的是图片则重绘
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(tempRes.isPixmap)
        ui->labelResult->setPixmap(tempRes.pixmap.scaled(ui->scrollArea->width()-10,ui->scrollArea->height()-10,Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

//三个action槽函数，切换系统模式
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

//显示设置界面
void MainWindow::on_actionSettings_triggered()
{
    settings->display();
}

//截图完成，进行下一步操作
void MainWindow::slotGrabFinished(QPixmap &pixmap)
{
    switch(settings->values.sysMode)
    {
        case SettingsDialog::GrabOnly:{//截图模式
            ui->labelResult->setAlignment(Qt::AlignCenter);//在label中显示缩放后的图片
            ui->labelResult->setPixmap(pixmap.scaled(ui->scrollArea->width()-10,ui->scrollArea->height()-10,Qt::KeepAspectRatio,Qt::SmoothTransformation));
            tempRes.isPixmap=true;//原图存入tempRes
            tempRes.pixmap=pixmap;
            if(settings->values.autoCopy)
                QApplication::clipboard()->setPixmap(pixmap);//复制到剪贴板
            }
        break;
        case SettingsDialog::OCR://OCR模式和翻译模式都需要接着进行OCR识别
            baiduOCR->sendPic(pixmap);
        break;
        case SettingsDialog::Translate:
            baiduOCR->sendPic(pixmap);
        break;
    }
}

//OCR识别结果返回成功
void MainWindow::slotBaiduOCRRecv(const QString &resStr)
{
    if(settings->values.sysMode==SettingsDialog::OCR)//OCR模式
    {
        ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);//在label中显示结果
        ui->labelResult->setText(resStr);
        tempRes.isPixmap=false;//存入tempRes
        tempRes.text=resStr;
        if(settings->values.autoCopy)
            QApplication::clipboard()->setText(resStr);//存入剪贴板
    }
    else if(settings->values.sysMode==SettingsDialog::Translate)//翻译模式，将字符串再进行翻译
    {
        baiduTrans->sendTrans(resStr);
    }
}

//翻译结果返回成功
void MainWindow::slotBaiduTransRecv(const QString &resStr)
{
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);//在label中显示结果
    ui->labelResult->setText(resStr);
    tempRes.isPixmap=false;//存入tempRes
    tempRes.text=resStr;
    if(settings->values.autoCopy)
        QApplication::clipboard()->setText(resStr);//存入剪贴板
}

//用户确认设置，应用设置
void MainWindow::slotSettingsAccepted()
{
    applySettings();//应用
    baiduOCR->sendAuth();//用户可能修改了OCR认证，再次发送认证请求
}

//托盘事件
void MainWindow::slotActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger://单击托盘图标
            show();//显示主窗口
            setWindowState(Qt::WindowNoState);//取消窗口状态(如最小化状态)
        break;
        default:
        break;
    }
}

//OCR识别错误处理
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
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);//错误信息显示在label中
    ui->labelResult->setText(displayStr);
}

//翻译错误处理
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
    ui->labelResult->setAlignment(Qt::AlignLeft|Qt::AlignTop);//错误信息显示在label中
    ui->labelResult->setText(displayStr);
}

//退出软件action
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

//"复制到剪贴板"按钮点击
void MainWindow::on_btn_copy_clicked()
{
    if(tempRes.isPixmap)
        QApplication::clipboard()->setPixmap(tempRes.pixmap);
    else
        QApplication::clipboard()->setText(tempRes.text);
}

//切换悬浮窗模式
void MainWindow::on_actionFloat_triggered(bool checked)
{
    isFloating=checked;
    if(checked)//进入悬浮窗模式
    {
        setWindowFlags(windowFlags()|Qt::Tool|Qt::WindowStaysOnTopHint);//无任务栏图标&保持在顶层
        setWindowOpacity(settings->values.floatOpacity);//设置不透明度
        show();
    }
    else//退出悬浮窗模式
    {
        setWindowFlags(windowFlags()&~(Qt::Tool|Qt::WindowStaysOnTopHint));
        setWindowOpacity(1);
        show();
    }
}

//打开反馈
void MainWindow::on_actionFeedback_triggered()
{
    QDesktopServices::openUrl(QUrl("https://support.qq.com/product/172973"));//使用腾讯兔小巢平台
}
//显示关于界面
void MainWindow::on_actionAbout_triggered()
{
    about=new AboutDialog();
    about->show();
}
//显示捐赠界面
void MainWindow::on_actionDonate_triggered()
{
    donate=new DonateDialog();
    donate->show();
    donate->setImg();
}
//显示帮助
void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/BowenLan/RightDrag/blob/master/help.md"));//显示github上的help.md
}
