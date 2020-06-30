#include "ocr.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qpixmap.h"
#include "qbuffer.h"

static const QString defApiKey="8kEopqDXdK8VvDc3pI7AdPX8",defSecretKey="ecN4DOkqqPq6QRu7IqM4etmRLmo0SyFT";
static QString langList[][2]={
    {"中英混合","CHN_ENG"},
    {"英文","ENG"},
    {"日语","JAP"},
    {"韩语","KOR"},
    {"法语","FRE"},
    {"西班牙语","SPA"},
    {"葡萄牙语","POR"},
    {"德语","GER"},
    {"意大利语","ITA"},
    {"俄语","RUS"}
};

BaiduOCR::BaiduOCR(QObject *parent) : QObject(parent)
{
    manager=new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyHandler(QNetworkReply*)));
    addEnter=false;
    for(unsigned int i=0;i<sizeof(langList)/sizeof(langList[0]);i++)
        langMap.insert(langList[i][0],langList[i][1]);
}

void BaiduOCR::sendAuth()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id="+api_key+"&client_secret="+secret_key));
    manager->get(request);
}

void BaiduOCR::sendPic(QPixmap &pixmap)
{
    if(pixmap.width()<15||pixmap.height()<15)
    {
        emit error(Err_SizeWrong,"截取范围过小无法进行识别");
        return;
    }
    if(pixmap.width()>4096||pixmap.height()>4096)
    {
        emit error(Err_SizeWrong,"截取范围过大无法进行识别");
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl("https://aip.baidubce.com/rest/2.0/ocr/v1/general?access_token="+access_token));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QByteArray postData,imgArray;
    QBuffer buf(&imgArray);
    pixmap.save(&buf,"jpg");
    postData.append("image="+imgArray.toBase64().toPercentEncoding());
    postData.append("&language_type="+language);
    buf.close();
    manager->post(request,postData);
}

void BaiduOCR::setLanguage(const QString &lang)
{
    if(langMap.contains(lang))
        language=langMap.value(lang);
}

void BaiduOCR::setKeys(const QString &apiKey, const QString &secretKey)
{
    api_key=apiKey;
    secret_key=secretKey;
}

void BaiduOCR::replyHandler(QNetworkReply *reply)
{
    QByteArray array=reply->readAll();

    if(reply->error()==QNetworkReply::NoError&&array.isEmpty())
    {
        qDebug( "network error, code: %d\n", (int)reply->error());
        emit error(Err_Network,QString("Error code=%1").arg((int)reply->error()));
        return;
    }

    qDebug()<<QString(array);
    QString result = QString::fromStdString(array.toStdString()).toUtf8();
    QJsonParseError parseError;
    QJsonDocument document=QJsonDocument::fromJson(result.toUtf8(),&parseError);
    if(document.isNull()||parseError.error!=QJsonParseError::NoError||!document.isObject()){
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }
    QJsonObject object = document.object();
    if(object.isEmpty()){
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }
    if(object.contains("access_token"))
    {
        access_token=object.value("access_token").toString();
    }
    else if(object.contains("words_result"))
    {
        QJsonArray words=object.value("words_result").toArray();
        QString resStr="";
        for(int i=0;i<words.size();i++)
        {
            resStr+=words[i].toObject().value("words").toString();
            if(addEnter) resStr+='\n';
            else resStr+=' ';
        }
        emit recvSuccess(resStr);
    }
    else if(object.contains("error_code"))
    {
        emit error(Err_JsonParseFailed,object.value("error_msg").toString());
    }
    else if(object.contains("error"))
    {
        emit error(Err_AuthFailed,object.value("error_description").toString());
    }
    else
    {
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }


}
