#include "kinectthread.h"

KinectThread::KinectThread(bool imageOrIR, QObject *parent) :
    imageOrIR(imageOrIR),
    QThread(parent)
{
    // シグナルの引数にMat型を使う
    qRegisterMetaType<vector<Mat> >("vector<Mat>");
}

KinectThread::~KinectThread()
{

}

void KinectThread::run()
{
    // Configのパスを取得してKinect初期化
    kinect = new KinectControl(imageOrIR);

    while(1)
    {
        vector<Mat> depth, color;
        kinect->setImageOrIR(imageOrIR);
        kinect->GetDepthColor(depth, color);

        for(int i = 0; i < depth.size(); ++i)
        {
            // IR画像だったら
            if(color.at(0).channels() == 1)
            {
                irImages = color;
            }
        }

        emit drawGL(depth, color);

        mutex.lock();
        condition.wait(&mutex);
        mutex.unlock();
    }
}

void KinectThread::active()
{
    condition.wakeOne();
}

void KinectThread::setImageOrIR(bool checked)
{
    imageOrIR = checked;
}

void KinectThread::getIRImages(vector<Mat> &ir)
{
    ir = this->irImages;
}
