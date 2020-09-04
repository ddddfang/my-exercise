
TEMPLATE = app
TARGET = tutorial
INCLUDEPATH += .
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include
INCLUDEPATH += /home/fang/Qt5.12.0/5.12.0/gcc_64/include/QtCore
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/opencv/include

LIBS += -L/home/fang/Qt5.12.0/5.12.0/gcc_64/lib -lQt5Core
LIBS += -L/usr/local/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lz -lm
LIBS += -L/opt/opencv/lib -lopencv_world

CONFIG += c++11
#QMAKE_CXXFLAGS += -std=c++11

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += myWidget.h readerThread.h
HEADERS += utils/YuvFileReader.h utils/OpencvReader.h

SOURCES += abc.cpp myWidget.cpp readerThread.cpp
SOURCES += utils/YuvFileReader.cpp utils/OpencvReader.cpp


QT += widgets core
QT += gui