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

STPage::STPage(){
    this->http=new Http();
}

//music lists
QList<STModel> STPage::musicLists(){
    QList<STModel> musiclist;
    http->get("http://www.songtaste.com/music/","");
    QString url=http->getResult();

    //QString url=get("http://127.0.0.1/songteste/music.htm");
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
    http->post("http://www.songtaste.com/time.php","str="+str+"&sid="+sid);
    qDebug()<<http->getResult();
    return http->getResult();
}

QString STPage::songString(QString sid){
    QString songstring="";
    http->get("http://www.songtaste.com/playmusic.php?song_id="+sid,"");
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
    http->get("http://image.songtaste.com/images/usericon/s/"+image,"");
    return http->getResultByte();
}
