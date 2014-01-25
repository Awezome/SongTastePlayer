#include "stpage.h"
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDebug>
#include <QRegExp>
#include <QTextCodec>
#include <QStringList>
#include "config.h"

STPage::STPage(){
    this->http=new Http();
}

//music lists
QList<STModel> STPage::musicLists(int type){
    QString u=type>0?"040"+QString::number(type):"";

    QList<STModel> musiclist;
    http->get(Config::stindex+u,"");
    QString url=http->getResult();
    QRegExp reg("MSL\\((.*)\\);");
    int pos =0;
    reg.setMinimal(true);
    STModel song;
    QString str;
    while((pos = reg.indexIn(url,pos)) != -1){
        str = reg.cap(1);
        pos+=reg.matchedLength();

        str.replace("\"","");
        QStringList list=str.split(",");

        song.id=list.at(1).trimmed();
        song.name=list.at(0).trimmed();
        song.author=list.at(2).trimmed();
        song.image=list.at(4).trimmed();
        musiclist.append(song);
    }
    return musiclist;
}

QString STPage::songUrl(QString sid){
    QString str= this->songString(sid);
    http->post(Config::stpost,"str="+str+"&sid="+sid);
    qDebug()<<http->getResult();
    return http->getResult();
}

QString STPage::songString(QString sid){
    QString songstring="";
    http->get(Config::stmusicaddress+sid,"");
    QString url=http->getResult();
    QRegExp reg("WrtSongLine\\((.*)\\);");
    int pos =0;
    reg.setMinimal(true);
    QString str;
    int flag=0;
    while((pos = reg.indexIn(url,pos)) != -1){
        pos+=reg.matchedLength();
        if(flag==1){
            str = reg.cap(1);
            str.replace("\"","");
            QStringList list=str.split(",");
            songstring=list.at(6);
            break;
        }
        flag++;
    }
    return songstring.trimmed();
}

QByteArray STPage::userImage(QString image){
    http->get(Config::stimage+image,"");
    return http->getResultByte();
}

QStringList STPage::typeLists(){
    return Config::sttype.split(",");
}
