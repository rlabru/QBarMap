#-------------------------------------------------
#
# Project created by QtCreator 2017-08-22T01:21:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 042_QBarMap
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        qbarmap.cpp \
        QtAwesome.cpp \
    qimgfileloader.cpp

HEADERS  += mainwindow.h \
            qbarmap.h \
            QtAwesome.h \
    qimgfileloader.h

FORMS    += mainwindow.ui

RESOURCES += \
    tools.qrc \
    QtAwesome.qrc
