#include "ui.h"
UI::UI(){
#ifdef Q_OS_WIN
    fontFamily="微软雅黑";
#else
    fontFamily="宋体";//字体怎么弄？
#endif
}

QString UI::font(){
    return fontFamily;
}

QString UI::fontWidget(){
    return this->fontFamily;
}

QFont UI::fontSmall(){
    return QFont(fontFamily,8,QFont::Bold);
}

QFont UI::fontBig(){
    return QFont(fontFamily,9,QFont::Bold);
}

QString UI::color(QString color){
    return "color:"+color+";";
}

QString UI::comboBox(){
    return "QComboBox{selection-color:#fff;color: #fff;background-color:#000;border: 1px black;padding:0 5px;}"
        "QComboBox:on{selection-background-color: rgb(108, 149, 59);color:#436254;}"
        "QComboBox:!on{selection-background-color:#000; color:#fff;}"
        "QComboBox::drop-down{border:1px solid #000;width:0;}"
        "QComboBox::down-arrow{border-style:none;}";
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

QString UI::slider(){
    return "QSlider::groove:horizontal{height: 2px;background: #eee;}"
        "QSlider::handle:horizontal {background: #ddd;width: 8px;margin: -3px 0;border-radius: 4px;}"
        "QSlider::add-page:horizontal{background: #333;}"
        "QSlider::sub-page:horizontal{background: #0579C7;}";
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
    table->setStyleSheet("selection-background-color:#9ED3FE");  //设置选中行颜色
    table->setContextMenuPolicy(Qt::CustomContextMenu);

    table->verticalScrollBar()->setStyleSheet(
      "QScrollBar{background:#999;width: 10px;}"
      "QScrollBar::handle{background:#999;border:0;}"
      "QScrollBar::handle:hover{background:#0579C7;}"
      "QScrollBar::sub-page{background:#fff;}"
      "QScrollBar::add-page{background:#fff;}");
}
