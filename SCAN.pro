#-------------------------------------------------
#
# Project created by QtCreator 2016-04-06T21:34:22
#
#-------------------------------------------------

QT       += core gui
QT       += webkitwidgets
QT       += network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SCAN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    payloadmanager.cpp \
    dialogviewfile.cpp \
    dialogrenamefile.cpp \
    dialogdownloadfromserver.cpp \
    dialogdownloadfromdevice.cpp

HEADERS  += mainwindow.h \
    payloadmanager.h \
    dialogviewfile.h \
    dialogrenamefile.h \
    dialogdownloadfromserver.h \
    dialogdownloadfromdevice.h

FORMS    += mainwindow.ui \
    dialogviewfile.ui \
    dialogrenamefile.ui \
    dialogdownloadfromserver.ui \
    dialogdownloadfromdevice.ui

RESOURCES +=
