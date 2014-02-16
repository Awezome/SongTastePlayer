#include "stpage.h"
#include <QRegExp>
#include "config.h"

//music lists
QList<STModel> STPage::musicLists(int type){
    QString u=type>0?"040"+QString::number(type):"";
    QList<STModel> musiclist;    
    QString url=Http::getString(STIndex+u);
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
    QString str=songString(sid);
    return Http::postString(STPost,"str="+str+"&sid="+sid);
}

QString STPage::songString(QString sid){
    QString songstring="";
    QString url=Http::getString(STMusicaddress+sid);
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
    return Http::get(STImage+image);
}

QStringList STPage::typeLists(){
    return STType;
}
