#include "stpage.h"
#include <QRegExp>
#include "config/config.h"

//music lists
QList<STModel> STPage::musicLists(int type){
    QString u=type>0?"040"+QString::number(type):"";
    QList<STModel> musiclist;    
    QString url=Http::getString(STIndex+u);
    if(url==""){
        return musiclist;
    }
    QRegExp reg("MSL\\((.*)\\);");
    int pos =0;
    reg.setMinimal(true);
    QString str;
    while((pos = reg.indexIn(url,pos)) != -1){
        str = reg.cap(1);
        pos+=reg.matchedLength();
        str.replace("\"","");
        musiclist.append(list2song(str.split(", ")));
    }
    return musiclist;
}

QString STPage::songUrl(QString sid){
    QString str=songString(sid);
    return Http::postString(STPost,"str="+str+"&sid="+sid);
}

QString STPage::songString(QString sid){
    QStringList list=songList(sid);
    return list.empty()?"error":list.at(6);
}

STModel STPage::song(QString sid){
    return list2song(songList(sid));
}

STModel STPage::list2song(QStringList list){
    STModel song;
    if(!list.empty()){
        song.id=list.at(1);
        song.name=list.at(0);
        song.author=list.at(2);
        song.image=list.at(4);
    }
    return song;
}

//得到一首歌的具体信息，包括song str,末过行空格处理
QStringList STPage::songList(QString sid){
    QStringList list;
    QString url=Http::getString(STMusicaddress+sid);
    if(url==""){
        return list;
    }
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
            list=str.split(", ");
            break;
        }
        flag++;
    }
    return list;
}

QByteArray STPage::userImage(QString image){
    return Http::get(STImage+image);
}

QStringList STPage::typeLists(){
    return STType;
}

QString STPage::songWeb(QString id){
    return STWeb+id+"/";
}
