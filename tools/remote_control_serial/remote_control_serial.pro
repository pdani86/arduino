#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T02:40:12
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = remote_control_serial
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += hidapi
LIBS += -L"$$_PRO_FILE_PWD_/hidapi/" -lhidapi
