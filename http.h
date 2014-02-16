#ifndef HTTP_H
#define HTTP_H
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QObject>
class Http  : public QObject
{
    Q_OBJECT
public:
    static QByteArray post(QString url,QString content);
    static QByteArray get(QString url);
};

#endif // HTTP_H
