#include "tool.h"
#include <QFile>

QTime Tool::qint64ToTime(qint64 time){
    //3个参数分别代表时，分，秒；60000毫秒为1分钟，所以分钟第二个参数是先除6000,第3个参数是直接除1s
    QTime t(0,(time/60000)%60,(time/1000)%60);
    return t;
}

QString Tool::qint64ToStringKb(qint64 i){
    return QString::number(i/1024);
}

QString Tool::getQss(QString file){
    QString qss="";
    QFile qssFile(file);
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen()){
       qss = QLatin1String(qssFile.readAll());
       qssFile.close();
    }
    return qss;
}
