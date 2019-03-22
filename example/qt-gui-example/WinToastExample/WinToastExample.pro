#-------------------------------------------------
#
# Project created by QtCreator 2016-10-01T17:29:36
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WinToastExample
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../src/wintoastlib.cpp

HEADERS  += mainwindow.h \
    ../../../src/wintoastlib.h

FORMS    += mainwindow.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../lib/Release/ -lWinToastLib
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../lib/Debug/ -lWinToastLib

