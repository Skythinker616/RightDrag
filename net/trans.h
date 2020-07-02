/*翻译，调用百度接口*/
#ifndef TRANS_H
#define TRANS_H

#include <QObject>
#include "qnetworkaccessmanager.h"

class BaiduTrans : public QObject
{
    Q_OBJECT
public:
    enum Error{//错误类型
        Err_JsonParseFailed,
        Err_Network,
        Err_Unknown
    };
    explicit BaiduTrans(QObject *parent = nullptr);
    ~BaiduTrans();
    void sendTrans(const QString &text);//发送翻译请求
    void setSrcLang(QString &lang);//设置源语言和目标语言
    void setDstLang(QString &lang);

private:
    QMap<QString,QString> langMap;//语言代码对照表
    QString fromLang,toLang;//目标语言，源语言
    QString appId,secret;
    QNetworkAccessManager *manager;

signals:
    void recvSuccess(const QString &resStr);
    void error(BaiduTrans::Error error,const QString &errInfo);

private slots:
    void replyHandler(QNetworkReply *reply);
};

#endif // TRANS_H
