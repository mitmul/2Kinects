#ifndef KINECTCONTROL_H
#define KINECTCONTROL_H

#include <iostream>
#include <stdexcept>
#include <map>
#include <sstream>
#include <QDebug>
#include <opencv.hpp>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#define WIDTH 640
#define HEIGHT 480

#define CHECK_RC(nRetVal, what)                                         \
    if (nRetVal != XN_STATUS_OK)                                        \
{                                                                       \
    qDebug("%s failed: %s\n", what, xnGetStatusString(nRetVal));        \
}

using namespace std;
using namespace cv;
using namespace xn;

class KinectControl
{
public:
    KinectControl(bool imageOrIR = true);
    ~KinectControl();

    void GetDepthColor(vector<Mat>& depthMat, vector<Mat>& colorMat);
    void setImageOrIR(bool checked);

private:
    vector<float> getDepthHistgram(const DepthGenerator& depth, const DepthMetaData& depthMD);

    // ジェネレータを作成する
    template<typename T>
    T CreateGenerator(const NodeInfo& node)
    {
        T g;
        XnStatus rc = node.GetInstance(g);
        CHECK_RC(rc, "GetInstance");

        g.SetMapOutputMode(OUTPUT_MODE);

        return g;
    }

    void EnumerateProductionTrees(Context& context, XnProductionNodeType type);

    XnStatus rc;
    Context context;

    XnMapOutputMode OUTPUT_MODE;
    DepthMetaData depthMD;
    ImageMetaData imageMD;
    SceneMetaData sceneMD;
    IRMetaData irMD;

    struct Kinect
    {
        DepthGenerator depth;
        ImageGenerator image;
        UserGenerator user;
        IRGenerator ir;

        Mat camera;
    };
    map<int, Kinect> kinect;

    // true:image
    bool imageOrIR;
};

#endif // KINECTCONTROL_H
