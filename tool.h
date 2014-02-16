#ifndef TOOL_H
#define TOOL_H
#include <QTime>

class Tool
{
public:
    static QTime qint64ToTime(qint64 time);
    static QString qint64ToStringKb(qint64 i);
    static QString getQss(QString file);
    static QString byte2String(QByteArray byte);
};

#endif // TOOL_H
