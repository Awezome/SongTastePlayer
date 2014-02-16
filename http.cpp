#include "http.h"
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

QByteArray Http::post(QString url, QString content){
    QNetworkAccessManager *net=new QNetworkAccessManager();
    QByteArray _content = content.toLatin1();
    int contentLength = _content.length();
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::ContentLengthHeader,contentLength);

    QNetworkReply *reply=net->post(req,_content);

    QEventLoop eventLoop;
    QTimer::singleShot(3000, &eventLoop, SLOT(quit()));//超时退出
    connect(net,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    eventLoop.exec();

    QByteArray result=reply->readAll();
    reply->deleteLater();
    return result;
}


QByteArray Http::get(QString url){
    QNetworkAccessManager *net=new QNetworkAccessManager();
    QNetworkRequest req;
    req.setUrl(QUrl(url));
    QNetworkReply *reply=net->get(req);

    QEventLoop eventLoop;
    QTimer::singleShot(5000, &eventLoop, SLOT(quit()));//超时退出
    connect(net,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    eventLoop.exec();

    QByteArray result=reply->readAll();
    reply->deleteLater();
    return result;
}
