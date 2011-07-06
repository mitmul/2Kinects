#ifndef KINECTTHREAD_H
#define KINECTTHREAD_H

#include <QThread>
#include <QObject>
#include <QDir>
#include <QMetaType>
#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QWaitCondition>
#include "kinectcontrol.h"

class KinectThread : public QThread
{
    Q_OBJECT

private:
    QDir dir;
    QString config;
    KinectControl *kinect;

    QMutex mutex;
    QWaitCondition condition;

    // true:image
    bool imageOrIR;

    vector<Mat> irImages;

public:
    KinectThread(bool imageOrIR = true, QObject *parent = 0);
    ~KinectThread();

    void setImageOrIR(bool checked);
    void getIRImages(vector<Mat>& ir);

protected:
    void run();

signals:
    void drawGL(vector<Mat> depth, vector<Mat> color);

public slots:
    void active();

};

#endif // KINECTTHREAD_H
