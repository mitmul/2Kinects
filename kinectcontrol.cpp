#include "kinectcontrol.h"

// デプスのヒストグラムを作成
vector<float> KinectControl::getDepthHistgram(const DepthGenerator& depth, const DepthMetaData& depthMD)
{
    // デプスの傾向を計算する(アルゴリズムはNiSimpleViewer.cppを利用)
    const int MAX_DEPTH = depth.GetDeviceMaxDepth();
    vector<float> depthHist(MAX_DEPTH);

    unsigned int points = 0;
    const XnDepthPixel* pDepth = depthMD.Data();
    for(XnUInt y = 0; y < depthMD.YRes(); ++y)
    {
        for(XnUInt x = 0; x < depthMD.XRes(); ++x, ++pDepth)
        {
            if(*pDepth != 0)
            {
                depthHist[*pDepth]++;
                points++;
            }
        }
    }

    for(int i = 1; i < MAX_DEPTH; ++i)
    {
        depthHist[i] += depthHist[i-1];
    }

    if(points != 0)
    {
        for(int i = 1; i < MAX_DEPTH; ++i)
        {
            depthHist[i] = (unsigned int)(256 * (1.0f - (depthHist[i] / points)));
        }
    }

    return depthHist;
}

// 検出されたデバイスを列挙する
void KinectControl::EnumerateProductionTrees(Context& context, XnProductionNodeType type)
{
    NodeInfoList nodes;
    XnStatus rc = context.EnumerateProductionTrees(type, NULL, nodes);

    if(rc != XN_STATUS_OK)
    {
        throw runtime_error(xnGetStatusString(rc));
    }
    else if(nodes.Begin () == nodes.End ())
    {
        throw runtime_error("No devices found.");
    }

    for(NodeInfoList::Iterator it = nodes.Begin(); it != nodes.End(); ++it)
    {
        qDebug() << xnProductionNodeTypeToString((*it).GetDescription().Type) <<
                    ", " <<
                    (*it).GetCreationInfo() << ", " <<
                    (*it).GetInstanceName() << ", " <<
                    (*it).GetDescription().strName << ", " <<
                    (*it).GetDescription().strVendor << ", ";

        NodeInfo info = *it;
        context.CreateProductionTree(info);
    }
}

KinectControl::KinectControl(bool imageOrIR)
{
    this->imageOrIR = imageOrIR;

    OUTPUT_MODE.nXRes = 640;
    OUTPUT_MODE.nYRes = 480;
    OUTPUT_MODE.nFPS = 30;

    //エラーチェック用
    rc = XN_STATUS_OK;

    //初期設定を読み込む
    rc = context.Init();
    CHECK_RC(rc, "Initialize context");

    // 検出されたデバイスを利用可能として登録する
    EnumerateProductionTrees(context, XN_NODE_TYPE_DEVICE);
    EnumerateProductionTrees(context, XN_NODE_TYPE_DEPTH);
    if(imageOrIR)
        EnumerateProductionTrees(context, XN_NODE_TYPE_IMAGE);
    else
        EnumerateProductionTrees(context, XN_NODE_TYPE_IR);

    // 登録されたデバイスを取得する
    qDebug() << "xn::Context::EnumerateExistingNodes ... ";
    NodeInfoList nodeList;
    rc = context.EnumerateExistingNodes( nodeList );
    CHECK_RC(rc, "EnumerateExistingNodes");
    qDebug() << "Success";

    // 登録されたデバイスからジェネレータを生成する
    for (NodeInfoList::Iterator it = nodeList.Begin(); it != nodeList.End(); ++it )
    {
        // インスタンス名の最後が番号になっている
        string name = (*it).GetInstanceName();
        int no = *name.rbegin() - '1';

        qDebug() << xnProductionNodeTypeToString((*it).GetDescription().Type) <<
                    ", " <<
                    (*it).GetCreationInfo() << ", " <<
                    (*it).GetInstanceName() << ", " <<
                    (*it).GetDescription().strName << ", " <<
                    (*it).GetDescription().strVendor << ", " <<
                    no << ", ";

        if((*it).GetDescription().Type == XN_NODE_TYPE_IMAGE && imageOrIR)
        {
            kinect[no].image = CreateGenerator<ImageGenerator>(*it);
        }
        else if((*it).GetDescription().Type == XN_NODE_TYPE_IR && !imageOrIR)
        {
            kinect[no].ir = CreateGenerator<IRGenerator>(*it);
        }
        else if((*it).GetDescription().Type == XN_NODE_TYPE_DEPTH)
        {
            kinect[no].depth = CreateGenerator<DepthGenerator>(*it);
        }
    }

    // ジェネレートを開始する
    context.StartGeneratingAll();

    // ビューポイントの設定や、カメラ領域を作成する
    for(map<int, Kinect>::iterator it = kinect.begin(); it != kinect.end(); ++it)
    {
        int no = it->first;
        Kinect& k = it->second;

        if(imageOrIR)
            k.depth.GetAlternativeViewPointCap().SetViewPoint(k.image);

        kinect[no].camera.create(Size(OUTPUT_MODE.nXRes, OUTPUT_MODE.nYRes), CV_8UC3);
    }
}

KinectControl::~KinectControl()
{
    context.Shutdown();
}

void KinectControl::GetDepthColor(vector<Mat>& depthMat, vector<Mat>& colorMat)
{
    context.WaitAndUpdateAll();

    depthMat.clear();
    colorMat.clear();

    // 検出したすべてのKinectの画像を表示する
    for(map<int, Kinect>::iterator it = kinect.begin(); it != kinect.end(); ++it)
    {
        Kinect& k = it->second;

        if(imageOrIR)
        {
            ImageMetaData imageMD;
            k.image.GetMetaData(imageMD);

            // 可視画像
            Mat colorImage(imageMD.YRes(), imageMD.XRes(), CV_8UC3, (uchar*)imageMD.RGB24Data());
            cvtColor(colorImage, colorImage, CV_RGB2BGR);
            colorMat.push_back(colorImage);
        }
        else
        {
            IRMetaData irMD;
            k.ir.GetMetaData(irMD);

            // IR画像
            Mat irImage(irMD.YRes(), irMD.XRes(), CV_16SC1, irMD.WritableData());
            Mat irImage8;
            irImage.convertTo(irImage8, CV_8U);
            imshow(k.ir.GetName(), irImage8);
            colorMat.push_back(irImage8);
        }

        DepthMetaData depthMD;
        k.depth.GetMetaData(depthMD);

        // デプスマップの作成
        vector<float> depthHist = getDepthHistgram(k.depth, depthMD);

        // デプス画像
        Mat depthImage(OUTPUT_MODE.nYRes, OUTPUT_MODE.nXRes, CV_64FC1);
        for(XnUInt y = 0; y < OUTPUT_MODE.nYRes; ++y)
        {
            for(XnUInt x = 0; x < OUTPUT_MODE.nXRes; ++x)
            {
                const XnDepthPixel& depth = depthMD(x, y);
                if(depth != 0)
                {
                    depthImage.at<double>(y, x) = depthHist[depthMD(x, y)];
                }
                else
                {
                    depthImage.at<double>(y, x) = 0.0;
                }
            }
        }
        depthMat.push_back(depthImage);
    }
}

void KinectControl::setImageOrIR(bool checked)
{
    imageOrIR = checked;
}
