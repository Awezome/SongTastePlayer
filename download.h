#ifndef DOWNLOAD_H
#define DOWNLOAD_H
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>
class Download : public QObject
{
    Q_OBJECT
public:
    explicit Download(QObject *parent = 0);
    void run(QString url,QString fileName);
signals:
    void downloaded();
    void saved();
    void progress(qint64,qint64);
private:
    QNetworkAccessManager *net;
    QByteArray result;
    void saveFile();

    QUrl url;
    QString fileName;

};

#endif // DOWNLOAD_H
