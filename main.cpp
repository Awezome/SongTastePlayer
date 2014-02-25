#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include "util/tool.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    a.setStyleSheet(Tool::getQss(":/qss/style.css"));
    Widget w;
    w.show();
    
    return a.exec();
}
