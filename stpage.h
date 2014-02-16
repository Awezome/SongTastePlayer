#ifndef STPAGE_H
#define STPAGE_H
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QList>
#include "stmodel.h"
#include "http.h"
class STPage  : public QObject
{

public:
    STPage();
    QList<STModel> musicLists(int type);
    QStringList typeLists();
    QString songUrl(QString sid);
    QByteArray userImage(QString image);
private:
    QString songString(QString sid);
};

#endif // STPAGE_H
