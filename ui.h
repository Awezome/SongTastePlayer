#ifndef UI_H
#define UI_H
#include <QString>
#include <QFont>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QIcon>
class UI{
public:
    static QString font();
    static QString pushBotton(QString normal);
    static QString pushBotton(QString normal,QString hover);
    static QString pushBotton(QString normal,QString hover,QString pressed);

    static void tableWidgetRowColor(QTableWidget *table,int row, QColor textcolor,QColor backcolor);
    static void tableWidgetView(QTableWidget *table);

    static QIcon icon(QString name);
    static QString style();
};

#endif // UI_H
