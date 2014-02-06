#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QStringList>

class Config{
  public:
    static QString title;
    static QString homepage;
    static QString changelog;
    static QString version;
    static QString config;

    static QString stindex;
    static QString stpost;
    static QString stmusicaddress;
    static QString stimage;

    static QStringList sttype;
};

#endif
