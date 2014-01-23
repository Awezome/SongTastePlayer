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
    stmodel.cpp \
    http.cpp

HEADERS  += widget.h \
    stpage.h \
    stmodel.h \
    http.h

FORMS    += widget.ui
