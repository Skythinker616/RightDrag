#ifndef TRANS_H
#define TRANS_H

#include <QObject>
#include "qnetworkaccessmanager.h"

class BaiduTrans : public QObject
{
    Q_OBJECT
public:
    enum Error{
        Err_JsonParseFailed,
        Err_Network,
        Err_Unknown
    };
    explicit BaiduTrans(QObject *parent = nullptr);
    void sendTrans(const QString &text);
    void setSrcLang(QString &lang);
    void setDstLang(QString &lang);

private:
    QMap<QString,QString> langMap;
    QString fromLang,toLang;
    QString appId,secret;
    QNetworkAccessManager *manager;

signals:
    void recvSuccess(const QString &resStr);
    void error(BaiduTrans::Error error,const QString &errInfo);

private slots:
    void replyHandler(QNetworkReply *reply);
};

#endif // TRANS_H
