#include "ui.h"

QString UI::font(){
#ifdef Q_OS_WIN
    return "*{font-family:微软雅黑;font-size:12px;}";
#else
    return "*{font-family:sans;font-weight:bold;font-size:12px;}";//字体怎么弄？
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
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setFrameShape(QFrame::NoFrame);
}

QIcon UI::icon(QString name){
    return QIcon(":/icon/"+name+".png");
}

QString UI::style(){
    return
    "QComboBox{font-weight:bold;color: #fff;background:transparent;border:none;padding:0 5px;}"
    "QComboBox:on{selection-background-color: #0579C7;}"
    "QComboBox:!on{selection-background-color:#0579C7; color:#fff;}"
    "QComboBox::drop-down{border:none;width:0;}"
    "QComboBox::down-arrow{border-style:none;}"
    "QListView{background: #000;color:#fff;border:none}"
    "QScrollBar{background:#fff;width: 10px;}"
    "QScrollBar::handle{background:#ccc;border:1px solid #bbb;border-radius: 5px;}"
    "QScrollBar::handle:hover{background:#0579C7;border:none}"
    "QScrollBar::sub-page{background:#fff;}"
    "QScrollBar::add-page{background:#fff;}"
    "QSlider::groove:horizontal{height: 2px;background: #eee;}"
    "QSlider::handle:horizontal {background: #ddd;width: 8px;margin: -3px 0;border-radius: 4px;}"
    "QSlider::add-page:horizontal{background: #000;}"
    "QSlider::sub-page:horizontal{background: #0579C7;}"
    "QTableWidget{selection-background-color:#9ED3FE;selection-color:#000}";
}
