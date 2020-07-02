#include "ocr.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qpixmap.h"
#include "qbuffer.h"

//语言列表，用于语言名与语言代码的转换
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
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyHandler(QNetworkReply*)));//连接网络接收槽
    addEnter=false;
    for(unsigned int i=0;i<sizeof(langList)/sizeof(langList[0]);i++)//写入语言代码表，用于查找
        langMap.insert(langList[i][0],langList[i][1]);
}

BaiduOCR::~BaiduOCR()
{
    delete manager;
}

//发送认证请求
void BaiduOCR::sendAuth()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id="+api_key+"&client_secret="+secret_key));
    manager->get(request);
}

//发送图片
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
    //设置请求头
    request.setUrl(QUrl("https://aip.baidubce.com/rest/2.0/ocr/v1/general?access_token="+access_token));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    //设置请求体
    QByteArray postData,imgArray;
    QBuffer buf(&imgArray);
    pixmap.save(&buf,"jpg");//图片转存至QByteArray
    postData.append("image="+imgArray.toBase64().toPercentEncoding());//图片经过base64编码和urlencode后放入请求
    postData.append("&language_type="+language);
    buf.close();
    manager->post(request,postData);
}

//设置识别语言(传入语言名)
void BaiduOCR::setLanguage(const QString &lang)
{
    if(langMap.contains(lang))//在表中查找语言代码
        language=langMap.value(lang);
}

//设置keys
void BaiduOCR::setKeys(const QString &apiKey, const QString &secretKey)
{
    api_key=apiKey;
    secret_key=secretKey;
}

//网络返回数据处理
void BaiduOCR::replyHandler(QNetworkReply *reply)
{
    QByteArray array=reply->readAll();//读取回复

    if(reply->error()==QNetworkReply::NoError&&array.isEmpty())//网络错误判定
    {
        //qDebug( "network error, code: %d\n", (int)reply->error());
        emit error(Err_Network,QString("Error code=%1").arg((int)reply->error()));
        return;
    }

    //qDebug()<<QString(array);
    //解析JSON数据
    QString result = QString::fromStdString(array.toStdString()).toUtf8();
    QJsonParseError parseError;
    QJsonDocument document=QJsonDocument::fromJson(result.toUtf8(),&parseError);

    if(document.isNull()||parseError.error!=QJsonParseError::NoError||!document.isObject()){//解析错误，一般是返回数据为空
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }

    QJsonObject object = document.object();
    if(object.isEmpty()){
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }
    //接收到认证结果
    if(object.contains("access_token"))
    {
        access_token=object.value("access_token").toString();
    }
    //接收到识别结果
    else if(object.contains("words_result"))
    {
        //逐项找出结果字符串并拼接
        QJsonArray words=object.value("words_result").toArray();
        QString resStr="";
        for(int i=0;i<words.size();i++)
        {
            resStr+=words[i].toObject().value("words").toString();
            if(addEnter) resStr+='\n';
            else resStr+=' ';
        }
        emit recvSuccess(resStr);//发出识别成功信号
    }
    //返回错误代码
    else if(object.contains("error_code"))
    {
        emit error(Err_JsonParseFailed,object.value("error_msg").toString());
    }
    //认证失败
    else if(object.contains("error"))
    {
        emit error(Err_AuthFailed,object.value("error_description").toString());
    }
    //其他错误
    else
    {
        emit error(Err_JsonParseFailed,QString("Line:%1,recv data:%2").arg(__LINE__).arg(QString(array)));
        return;
    }


}
