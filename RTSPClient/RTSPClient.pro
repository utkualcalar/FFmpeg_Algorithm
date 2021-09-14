
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RTSPClient
TEMPLATE = app


SOURCES += main.cpp\
    FFmpeg.cpp \
    login.cpp \
    video.cpp

HEADERS  += \
    FFmpeg.h \
    login.h \
    video.h

FORMS    += login.ui \
    video.ui

LIBS += -L /usr/local/lib -lavcodec -lavformat -lswscale -lavutil\
