QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = UDPSenderReciever
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += C:/opencv2413/opencv/build/install/include
LIBS += "C:/opencv2413/opencv/build/bin/libopencv_core2413d.dll"
LIBS += "C:/opencv2413/opencv/build/bin/libopencv_highgui2413d.dll"
LIBS += "C:/opencv2413/opencv/build/bin/libopencv_imgproc2413d.dll"
LIBS += "C:/opencv2413/opencv/build/bin/libopencv_photo2413d.dll"
LIBS += "C:/opencv2413/opencv/build/bin/libopencv_contrib2413d.dll"
LIBS += "C:/Windows/System32/ws2_32.dll"
#LIBS += -lws2_32

TEMPLATE = app

SOURCES += main.cpp \
    practicalsocket.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    config.h \
    practicalsocket.h
