#ifndef STPAGE_H
#define STPAGE_H
#include "stmodel.h"
#include "http.h"
class STPage{
public:
    static QList<STModel> musicLists(int type);
    static QStringList typeLists();
    static QString songUrl(QString sid);
    static QByteArray userImage(QString image);
    static QString songWeb(QString id);
    static STModel song(QString sid);
private:
    static QString songString(QString sid);
    static QStringList songList(QString sid);
    static STModel list2song(QStringList list);
};

#endif // STPAGE_H
