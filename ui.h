#ifndef UI_H
#define UI_H
#include <QString>
#include <QFont>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
class UI{
public:
    UI();
    QString fontFamily;

    QString font();
    QString color(QString color);
    QString fontWidget();
    QFont fontSmall();
    QFont fontBig();
    QString comboBox();
    QString pushBotton(QString normal);
    QString pushBotton(QString normal,QString hover);
    QString pushBotton(QString normal,QString hover,QString pressed);
    QString slider();

    void tableWidgetRowColor(QTableWidget *table,int row, QColor textcolor,QColor backcolor);
    void tableWidgetView(QTableWidget *table);
};

#endif // UI_H
