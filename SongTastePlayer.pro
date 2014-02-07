#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T11:53:24
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SongTastePlayer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    stpage.cpp \
    http.cpp \
    config.cpp \
    download.cpp \
    tool.cpp

HEADERS  += widget.h \
    stpage.h \
    stmodel.h \
    http.h \
    config.h \
    download.h \
    tool.h

FORMS    += widget.ui

RC_FILE = icon.rc

RESOURCES += \
    image.qrc

CONFIG += c++11
