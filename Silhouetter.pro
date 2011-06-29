#-------------------------------------------------
#
# Project created by QtCreator 2011-06-24T22:09:48
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = Silhouetter
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
    /Applications/OpenCV/include \
    /Applications/OpenCV/include/opencv \
    /Applications/OpenCV/include/opencv2

LIBS += -L/usr/lib \
    -lnimCodecs \
    -lnimMockNodes \
    -lnimRecorder \
    -lOpenNI \
    -L/Applications/OpenCV/lib \
    -lopencv_calib3d \
    -lopencv_contrib \
    -lopencv_core \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_gpu \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_legacy \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_video
