
TEMPLATE = app
TARGET = tutorial
INCLUDEPATH += .
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include/QtCore

LIBS += -L/home/fang/Qt5.12.0/5.12.0/gcc_64/lib -lQt5Core

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
SOURCES += abc.cpp

QT += widgets core
QT -= gui
