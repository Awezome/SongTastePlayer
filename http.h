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
    Http();
    ~Http();
    QString getResult();
    void post(QString url,QString content);
    void get(QString url,QString content);
    QByteArray getResultByte();
private slots:
    void replyFinished(QNetworkReply* reply);
private:
    QNetworkAccessManager *net;
    QString result;
    QByteArray resultByte;
};

#endif // HTTP_H
