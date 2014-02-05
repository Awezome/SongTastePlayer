#include "download.h"
#include <QEventLoop>


Download::Download(QObject *parent):QObject(parent){
    this->net = new QNetworkAccessManager(this);
    connect(this, &Download::downloaded, this,&Download::saveFile);
}

void Download::setUrl(QString url){
    this->url=QUrl(url);
}

void Download::setFilename(QString fileName){
    this->fileName=fileName;
}

void Download::run(){
    QNetworkRequest req;
    req.setUrl(this->url);
    QNetworkReply *reply=this->net->get(req);

    QEventLoop eventLoop;
    connect(net,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);
    connect(reply,&QNetworkReply::downloadProgress, this,&Download::downloadProgress);
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
}

void Download::downloadProgress(qint64 recieved, qint64 total){
    emit progress(recieved,total);
}
