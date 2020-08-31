
TEMPLATE = app
TARGET = tutorial
INCLUDEPATH += .
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include/QtCore

LIBS += -L/home/fang/Qt5.12.0/5.12.0/gcc_64/lib -lQt5Core

#这个不加,c++11语法貌似也能用
CONFIG += c++11
#下面这个加不加似乎都行
#QMAKE_CXXFLAGS += -std=c++11

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += myWidget.h readerThread.h
SOURCES += abc.cpp myWidget.cpp readerThread.cpp

QT += widgets core
QT += gui
