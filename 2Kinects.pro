#-------------------------------------------------
#
# Project created by QtCreator 2011-06-24T22:09:48
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = 2Kinects
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    kinectcontrol.cpp \
    kinectthread.cpp \
    glwidget.cpp \
    utilities.cpp

HEADERS  += mainwindow.h \
    kinectcontrol.h \
    kinectthread.h \
    glwidget.h \
    utilities.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/ni \
    /usr/local/include \
    /usr/local/include/opencv \
    /usr/local/include/opencv2

LIBS += -L/usr/lib \
    -lnimCodecs \
    -lnimMockNodes \
    -lnimRecorder \
    -lOpenNI \
    -L/usr/local/lib \
    -lopencv_gpu.2.3.1 \
    -lopencv_video.2.3.1 \
    -lopencv_objdetect.2.3.1 \
    -lopencv_ml.2.3.1 \
    -lopencv_legacy.2.3.1 \
    -lopencv_imgproc.2.3.1 \
    -lopencv_highgui.2.3.1 \
    -lopencv_flann.2.3.1 \
    -lopencv_features2d.2.3.1 \
    -lopencv_core.2.3.1 \
    -lopencv_contrib.2.3.1 \
    -lopencv_calib3d.2.3.1

CONFIG -= app_bundle
