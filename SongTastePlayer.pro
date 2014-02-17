# Project created by QtCreator 2014-01-21T11:53:24

QT       += core widgets gui network multimedia
win32{
    QT+=winextras
}
TARGET = SongTastePlayer
TEMPLATE = app
SOURCES += main.cpp\
        widget.cpp \
    stpage.cpp \
    http.cpp \
    tool.cpp \
    ui.cpp \
    download.cpp
HEADERS  += widget.h \
    stpage.h \
    stmodel.h \
    http.h \
    config.h \
    download.h \
    tool.h \
    ui.h
FORMS    += widget.ui
RC_FILE = pro.rc
RESOURCES += image.qrc
CONFIG += c++11
