#ifndef OCR_H
#define OCR_H

#include <QObject>
#include "qnetworkaccessmanager.h"

class BaiduOCR : public QObject
{
    Q_OBJECT
public:
    QMap<QString,QString> langMap;
    enum Error{
        Err_Network,
        Err_JsonParseFailed,
        Err_AuthFailed,
        Err_SizeWrong,
        Err_Unknown
    };
    bool addEnter;
    QString language;
    explicit BaiduOCR(QObject *parent = nullptr);
    void sendAuth();
    void sendPic(QPixmap &pixmap);
    void setLanguage(const QString &lang);
    void setKeys(const QString &apiKey,const QString &secretKey);

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
