/*OCR识别，调用百度OCR接口*/
#ifndef OCR_H
#define OCR_H

#include <QObject>
#include "qnetworkaccessmanager.h"

class BaiduOCR : public QObject
{
    Q_OBJECT
public:
    QMap<QString,QString> langMap;//语言代码对照表
    enum Error{
        Err_Network,
        Err_JsonParseFailed,
        Err_AuthFailed,
        Err_SizeWrong,
        Err_Unknown
    };
    bool addEnter;//在段落结尾插入\n，若为false则插入空格
    QString language;//需要识别的语言名
    explicit BaiduOCR(QObject *parent = nullptr);
    ~BaiduOCR();
    void sendAuth();//进行认证
    void sendPic(QPixmap &pixmap);//发送图片进行识别
    void setLanguage(const QString &lang);//设置语言
    void setKeys(const QString &apiKey,const QString &secretKey);//设置api_key和secret_key

private:
    QString access_token,api_key,secret_key;
    QNetworkAccessManager *manager;

signals:
    void recvSuccess(const QString &resStr);
    void error(BaiduOCR::Error error,const QString &errInfo);

private slots:
    void replyHandler(QNetworkReply *reply);
};

#endif // OCR_H
