
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
HEADERS += mainWindow.h
HEADERS += apps/live/liveWidget.h apps/live/videoThread.h ./apps/live/audioThread.h
HEADERS += utils/YuvFileReader.h utils/OpencvReader.h utils/ffCameraReader.h utils/ffMicroReader.h

SOURCES += abc.cpp
SOURCES += mainWindow.cpp
SOURCES += apps/live/liveWidget.cpp apps/live/videoThread.cpp apps/live/audioThread.cpp
SOURCES += utils/YuvFileReader.cpp utils/OpencvReader.cpp utils/ffCameraReader.cpp utils/ffMicroReader.cpp


QT += widgets core
QT += gui
