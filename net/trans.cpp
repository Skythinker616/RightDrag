#include "trans.h"
#include "qnetworkreply.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qpixmap.h"
#include "qbuffer.h"

//用于认证
static const QString defAppID="20200625000505739",defSecret="GzR__hiDPx_TxCf1NntD";
//语言代码转换表
static QString langList[][2]={
    {"中文","zh"},
    {"英语","en"},
    {"粤语","yue"},
    {"文言文","wyw"},
    {"日语","jp"},
    {"韩语","kor"},
    {"法语","fra"},
    {"西班牙语","spa"},
    {"泰语","th"},
    {"阿拉伯语","ara"},
    {"俄语","ru"},
    {"葡萄牙语","pt"},
    {"德语","de"},
    {"意大利语","it"},
    {"希腊语","el"},
    {"荷兰语","nl"},
    {"波兰语","pl"},
    {"保加利亚语","bul"},
    {"爱沙尼亚语","est"},
    {"丹麦语","dan"},
    {"芬兰语","fin"},
    {"捷克语","cs"},
    {"罗马尼亚语","rom"},
    {"斯洛文尼亚语","slo"},
    {"瑞典语","swe"},
    {"匈牙利语","hu"},
    {"繁体中文","cht"},
    {"越南语","vie"}
};

BaiduTrans::BaiduTrans(QObject *parent) : QObject(parent)
{
    manager=new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyHandler(QNetworkReply*)));
    for(unsigned int i=0;i<sizeof(langList)/sizeof(langList[0]);i++)//写入语言代码表用于查询
        langMap.insert(langList[i][0],langList[i][1]);
    appId=defAppID;
    secret=defSecret;
}

BaiduTrans::~BaiduTrans()
{
    delete manager;
}

//发送翻译请求
void BaiduTrans::sendTrans(const QString &text)
{
    //生成请求体数据
    QString salt="";
    for(int i=0;i<10;i++) salt+='0'+rand()%10;
    QByteArray sign=QCryptographicHash::hash(QString(appId+text+salt+secret).toUtf8(),QCryptographicHash::Md5).toHex();//经过md5加密后生成签名
    QString bodyStr="q="+text.toUtf8().toPercentEncoding()+"&from="+fromLang+"&to="+toLang+"&appid="+appId+"&salt="+salt+"&sign="+sign;//拼接url
    //qDebug()<<bodyStr;
    //发送post请求
    QNetworkRequest request;
    request.setUrl(QUrl("http://api.fanyi.baidu.com/api/trans/vip/translate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
    QByteArray postData;
    postData.append(bodyStr);
    manager->post(request,postData);
}

//设置源语言
void BaiduTrans::setSrcLang(QString &lang)
{
    if(langMap.contains(lang))
        fromLang=langMap.value(lang);
}
//设置目标语言
void BaiduTrans::setDstLang(QString &lang)
{
    if(langMap.contains(lang))
        toLang=langMap.value(lang);
}

//网络接收信号处理
void BaiduTrans::replyHandler(QNetworkReply *reply)
{
    QByteArray array=reply->readAll();//读取回复

    if (reply->error()!=QNetworkReply::NoError&&array.isEmpty())//网络错误
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
        emit error(Err_JsonParseFailed,QString("Line:%1").arg(__LINE__));
        return;
    }

    QJsonObject object = document.object();
    if(object.isEmpty()){
        emit error(Err_JsonParseFailed,QString("Line:%1").arg(__LINE__));
        return;
    }
    //收到翻译结果
    if(object.contains("trans_result"))
    {
        QJsonArray res=object.value("trans_result").toArray();
        emit recvSuccess(res[0].toObject().value("dst").toString());
    }
    //收到错误信息
    else if(object.contains("error_code"))
    {
        emit error(Err_JsonParseFailed,QString("errorCode=%1,errorMsg=%2").arg(object.value("error_code").toString()).arg(object.value("error_msg").toString()));
        return;
    }
    //其他错误
    else
    {
        emit error(Err_JsonParseFailed,QString("Line:%1").arg(__LINE__));
        return;
    }

}
