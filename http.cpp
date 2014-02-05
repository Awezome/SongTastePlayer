#include "http.h"
#include <QTextCodec>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

Http::Http(){
    this->net = new QNetworkAccessManager(this);
    //connect(net,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
}

Http::~Http(){
}

void Http::post(QString url, QString content){
    QByteArray _content = content.toLatin1();
    int contentLength = _content.length();
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    QNetworkReply *reply=this->net->post(req,_content);

    QEventLoop eventLoop;
    connect(net,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    eventLoop.exec();

    this->resultByte = reply->readAll();
}

void Http::get(QString url, QString content){
    QNetworkRequest req;
    req.setUrl(QUrl(url+ content));
    QNetworkReply *reply=this->net->get(req);

    QEventLoop eventLoop;
    QTimer::singleShot(3000, &eventLoop, SLOT(quit()));//超时退出
    connect(net,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));

    eventLoop.exec();

    this->resultByte=reply->readAll();
    reply->deleteLater();
}

void Http::replyFinished(QNetworkReply *reply){
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    this->result = codec->toUnicode((reply->readAll()));
    qDebug()<<result;
}

QString Http::getResult(){
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    return  codec->toUnicode(this->resultByte).toUtf8();
}

QByteArray Http::getResultByte(){
    return this->resultByte;
}
