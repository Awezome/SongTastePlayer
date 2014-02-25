# Project created by QtCreator 2014-01-21T11:53:24

QT       += core widgets network multimedia
win32{
    QT+=winextras
}
TARGET = SongTastePlayer
TEMPLATE = app
SOURCES += main.cpp\
        widget.cpp \
    stpage.cpp \
    ui.cpp \
    util/download.cpp \
    util/http.cpp \
    util/tool.cpp
HEADERS  += widget.h \
    stpage.h \
    stmodel.h \
    ui.h \
    util/download.h \
    util/http.h \
    util/tool.h \
    config/config.h
RC_FILE = config/pro.rc
RESOURCES += image.qrc
CONFIG += c++11
