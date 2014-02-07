#include "download.h"
#include <QEventLoop>


Download::Download(QObject *parent):QObject(parent){
    this->net = new QNetworkAccessManager(this);
    connect(this, &Download::downloaded, this,&Download::saveFile);
}

void Download::run(QString url,QString fileName){
    this->url=QUrl(url);
    this->fileName=fileName;
    QNetworkRequest req;
    req.setUrl(this->url);
    QNetworkReply *reply=this->net->get(req);

    QEventLoop eventLoop;
    connect(net,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);
    connect(reply,&QNetworkReply::downloadProgress,[this](qint64 bytesReceived,qint64 bytesTotal){
         emit progress(bytesReceived,bytesTotal);
    });
    eventLoop.exec();

    this->result=reply->readAll();
    reply->deleteLater();
    emit downloaded();
}

void Download::saveFile(){
    QFile *f=new QFile();
    f->setFileName(this->fileName);
    f->open(QIODevice::WriteOnly);
    f->write(this->result);
    f->flush();
    f->close();
    emit saved();
}
