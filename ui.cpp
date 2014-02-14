#include "ui.h"

QString UI::font(){
#ifdef Q_OS_WIN
    return "font-family:微软雅黑";
#else
    return "font-family:sans;font-weight:bold;";//字体怎么弄？
#endif
}

QString UI::pushBotton(QString normal){
    return "QPushButton{border-image: url(:/image/"+normal+".png);}";
}

QString UI::pushBotton(QString normal,QString hover){
    return "QPushButton{border-image: url(:/image/"+normal+".png);}"
        "QPushButton:hover{border-image: url(:/image/"+hover+".png);}";
}

QString UI::pushBotton(QString normal,QString hover,QString pressed){
    return "QPushButton{border-image: url(:/image/"+normal+".png);}"
        "QPushButton:hover{border-image: url(:/image/"+hover+".png);}"
        "QPushButton:pressed{border-image: url(:/image/"+pressed+".png);}";
}

void UI::tableWidgetRowColor(QTableWidget *table,int row, QColor textcolor,QColor backcolor){
    int size=table->columnCount();
    QTableWidgetItem *item;
    for (int col=0; col<size; col++){
        item =table->item(row, col);
        item->setBackgroundColor(backcolor);
        item->setTextColor(textcolor);
    }
}

void UI::tableWidgetView(QTableWidget *table){
    table->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    table->setSelectionMode(QAbstractItemView::SingleSelection); //设置为可以选中多个目标
    table->verticalHeader()->setVisible(false); //隐藏行号
    table->horizontalHeader()->setVisible(false); //隐藏行表头
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->setStyleSheet("selection-background-color:#9ED3FE;selection-color:#000");  //设置选中行颜色
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
