#include "utilities.h"

Utilities::Utilities(QObject *parent) :
    QObject(parent)
{
}

void Utilities::sendConsoleText(QString text)
{
    emit appendConsoleText(text);
}

int Utilities::getVLineNumberFromObj(const QString fileName)
{
    int vNum = 0;

    QFile fp(fileName);
    if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        while(!st.atEnd())
        {
            QString line = st.readLine();
            QStringList splitLine = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(QString::compare(splitLine.at(0), "v") == 0)
            {
                ++vNum;
            }
        }
        fp.close();
    }

    return vNum;
}

int Utilities::getFLineNumberFromObj(const QString fileName)
{
    int fNum = 0;

    QFile fp(fileName);
    if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        while(!st.atEnd())
        {
            QString line = st.readLine();
            QStringList splitLine = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(QString::compare(splitLine.at(0), "f") == 0)
            {
                ++fNum;
            }
        }
        fp.close();
    }

    return fNum;
}

void Utilities::getFLinesFromObj(const QString fileName, Mat &fLines)
{
    int fNum = getFLineNumberFromObj(fileName);
    fLines.create(fNum, 3, CV_64FC1);

    QFile fp(fileName);
    if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        int nowFNum = 0;

        QTextStream st(&fp);
        while(!st.atEnd())
        {
            QString line = st.readLine();
            QStringList splitLine = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(QString::compare(splitLine.at(0), "f") == 0)
            {
                fLines.at<double>(nowFNum, 0) = splitLine.at(1).toDouble();
                fLines.at<double>(nowFNum, 1) = splitLine.at(2).toDouble();
                fLines.at<double>(nowFNum, 2) = splitLine.at(3).toDouble();
                ++nowFNum;
            }
        }
        fp.close();
    }
}

void Utilities::getVLinesFromObj(const QString fileName, Mat &vLines)
{
    int vNum = getVLineNumberFromObj(fileName);
    vLines.create(vNum, 3, CV_64FC1);

    QFile fp(fileName);
    if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        int nowVNum = 0;

        QTextStream st(&fp);
        while(!st.atEnd())
        {
            QString line = st.readLine();
            QStringList splitLine = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(QString::compare(splitLine.at(0), "v") == 0)
            {
                vLines.at<double>(nowVNum, 0) = splitLine.at(1).toDouble();
                vLines.at<double>(nowVNum, 1) = splitLine.at(2).toDouble();
                vLines.at<double>(nowVNum, 2) = splitLine.at(3).toDouble();
                ++nowVNum;
            }
        }
        fp.close();
    }
}

void Utilities::saveObjFileFromVLinesFLines(const QString fileName, const Mat vLines, const Mat fLines)
{
    QFile fp(fileName);
    if(fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        for(int i = 0; i < vLines.rows; ++i)
        {
            st << "v" << " " << vLines.at<double>(i, 0) << " " << vLines.at<double>(i, 1) << " " << vLines.at<double>(i, 2) << "\n";
        }
        for(int i = 0; i < fLines.rows; ++i)
        {
            st << "f" << " " << fLines.at<double>(i, 0) << " " << fLines.at<double>(i, 1) << " " << fLines.at<double>(i, 2) << "\n";
        }
        fp.close();
    }
}

void Utilities::moveModelToOrigin(Mat &vertex)
{

#if 1

    Point3d gravPoint(0.0, 0.0, 0.0);
    for(int i = 0; i < vertex.rows; ++i)
    {
        gravPoint.x += vertex.at<double>(i, 0);
        gravPoint.y += vertex.at<double>(i, 1);
        gravPoint.z += vertex.at<double>(i, 2);
    }
    gravPoint.x /= (double)vertex.rows;
    gravPoint.y /= (double)vertex.rows;
    gravPoint.z /= (double)vertex.rows;

    for(int i = 0; i < vertex.rows; ++i)
    {
        vertex.at<double>(i, 0) -= gravPoint.x;
        vertex.at<double>(i, 1) -= gravPoint.y;
        vertex.at<double>(i, 2) -= gravPoint.z;
    }

#else

    double initX = vertex.at<double>(0, 0);
    double initY = vertex.at<double>(0, 1);
    double initZ = vertex.at<double>(0, 2);
    double minX = initX, minY = initY, minZ = initZ;
    double maxX = initX, maxY = initY, maxZ = initZ;

    for(int i = 0; i < vertex.rows; ++i)
    {
        if(minX > vertex.at<double>(i, 0))
            minX = vertex.at<double>(i, 0);
        if(maxX < vertex.at<double>(i, 0))
            maxX = vertex.at<double>(i, 0);

        if(minY > vertex.at<double>(i, 1))
            minY = vertex.at<double>(i, 1);
        if(maxY < vertex.at<double>(i, 1))
            maxY = vertex.at<double>(i, 1);

        if(minZ > vertex.at<double>(i, 2))
            minZ = vertex.at<double>(i, 2);
        if(maxZ < vertex.at<double>(i, 2))
            maxZ = vertex.at<double>(i, 2);
    }

    Point3d meanPoint;
    meanPoint.x = minX + (maxX - minX) / 2.0;
    meanPoint.y = minY + (maxY - minY) / 2.0;
    meanPoint.z = minZ + (maxZ - minZ) / 2.0;

    for(int i = 0; i < vertex.rows; ++i)
    {
        vertex.at<double>(i, 0) -= meanPoint.x;
        vertex.at<double>(i, 1) -= meanPoint.y;
        vertex.at<double>(i, 2) -= meanPoint.z;
    }

#endif

}

void Utilities::saveCSVFromMat(const QString fileName, const Mat src)
{
    Mat dst;
    dst.create(src.size(), CV_64FC(src.channels()));
    src.convertTo(dst, CV_64F);

    QFile fp(fileName);
    if(fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        for(int i = 0; i < dst.rows; ++i)
        {
            for(int j = 0; j < dst.cols; ++j)
            {
                if(dst.channels() == 1)
                {
                    st << dst.at<double>(i, j) << ",";
                }
                else
                {
                    for(int k = 0; k < dst.channels(); ++k)
                    {
                        st << dst.at<Vec3d>(i, j)(k) << ",";
                    }
                }
            }
            st << "\n";
        }
        fp.close();
    }
}

vector<Point3d> Utilities::getVecsFromMat(const Mat src, const Mat cameraMat)
{
    vector<Point3d> vecs;

    if(src.type() == CV_64FC1)
    {
        double f = (cameraMat.at<double>(0, 0) + cameraMat.at<double>(1, 1)) / 2.0;
        double cx = cameraMat.at<double>(0, 2);
        double cy = cameraMat.at<double>(1, 2);

        for(int i = 0; i < src.rows; ++i)
        {
            for(int j = 0; j < src.cols; ++j)
            {
                double val = src.at<double>(i, j);

                Point3d vec;
                vec.x = (j - cx) * val / f;
                vec.y = -((i - cy) * val / f);
                vec.z = val;

                vecs.push_back(vec);
            }
        }
    }

    if(src.type() == CV_8UC3)
    {
        double x = -1.0;
        double y = -1.0;
        double xStep = 2.0 / (double)src.cols;
        double yStep = 2.0 / (double)src.rows;

        for(int i = 0; i < src.rows; ++i)
        {
            for(int j = 0; j < src.cols; ++j)
            {
                Point3d vec;
                vec.x = (double)src.at<Vec3b>(i, j)(2) / 255.0;
                vec.y = (double)src.at<Vec3b>(i, j)(1) / 255.0;
                vec.z = (double)src.at<Vec3b>(i, j)(0) / 255.0;

                vecs.push_back(vec);
            }
        }
    }

    return vecs;
}

void Utilities::rotatePoint3dVecsAroundY(const double rotate, vector<Point3d>& vecs)
{
    for(int i = 0; i < vecs.size(); ++i)
    {
        double x = vecs.at(i).x;
        double z = vecs.at(i).z;

        vecs.at(i).x = z * sin(rotate) + x * cos(rotate);
        vecs.at(i).z = z * cos(rotate) - x * sin(rotate);
    }
}

vector<Point3d> Utilities::synthVecs(const vector<Point3d> vecA, const vector<Point3d> vecB)
{
    vector<Point3d> vecAB;

    for(int i = 0; i < vecA.size(); ++i)
    {
        vecAB.push_back(vecA.at(i));
    }
    for(int i = 0; i < vecB.size(); ++i)
    {
        vecAB.push_back(vecB.at(i));
    }

    return vecAB;
}

void Utilities::moveVecs(const Point3d shift, vector<Point3d> &vecs)
{
    for(int i = 0; i < vecs.size(); ++i)
    {
        vecs.at(i).x += shift.x;
        vecs.at(i).y += shift.y;
        vecs.at(i).z += shift.z;
    }
}

void Utilities::saveCSVFromPoint3fVector(const QString fileName, const vector<Point3f> vecs)
{
    QFile fp(fileName);
    if(fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        for(int i = 0; i < (int)vecs.size(); ++i)
        {
            st << vecs.at(i).x << "," << vecs.at(i).y << "," << vecs.at(i).z << "\n";
        }
        fp.close();
    }
}

void Utilities::getMatFromCSV(const QString fileName, Mat &dst)
{
    int rows = 0, cols = 0;

    QFile fp(fileName);
    if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream st(&fp);
        while(!st.atEnd())
        {
            QString line = st.readLine();
            QStringList sLine = line.split(",", QString::SkipEmptyParts);
            cols = sLine.size();
            ++rows;
        }
        fp.close();

        dst.create(rows, cols, CV_64FC1);

        if(fp.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            st.setDevice(&fp);
            int row = 0;
            while(!st.atEnd())
            {
                QString line = st.readLine();
                QStringList sLine = line.split(",", QString::SkipEmptyParts);

                for(int i = 0; i < sLine.size(); ++i)
                {
                    dst.at<double>(row, i) = sLine.at(i).toDouble();
                }
                ++row;
            }
            fp.close();
        }
    }
}

void Utilities::getExtrinsicMatrix(const Mat initCalib, const Size pttrnSize, const vector<Point3f> objectPoint, const Mat cameraMatrix, const Mat distCoeffs, Mat &rotationVec, Mat &transVec)
{
    int allPoints = pttrnSize.width * pttrnSize.height;

    vector<Point2f> corners;
    if(findChessboardCorners(initCalib, pttrnSize, corners) == true)
    {
        Mat showCorner = initCalib.clone();
        drawChessboardCorners(showCorner, pttrnSize, corners, true);
        imshow("init calib pattern", showCorner);
        waitKey(0);

        // キャリブレーションボード上の点
        CvPoint3D32f objects[allPoints];
        CvMat object_points;
        for(int i = 0; i < pttrnSize.height; ++i)
        {
            for(int j = 0; j < pttrnSize.width; ++j)
            {
                objects[i * pttrnSize.width + j].x = objectPoint.at(i).x;
                objects[i * pttrnSize.width + j].y = objectPoint.at(i).y;
                objects[i * pttrnSize.width + j].z = objectPoint.at(i).z;
            }
        }
        cvInitMatHeader(&object_points, allPoints, 3, CV_32FC1, objects);

        qDebug() << "object_points ready.";

        // 画像上の点
        CvMat* image_points = cvCreateMat(corners.size(), 1, CV_32FC2);
        for(int i = 0; i < corners.size(); ++i)
        {
            cvSet2D(image_points, i, 0, cvScalar(corners.at(i).x, corners.at(i).y));
        }

        qDebug() << "image_points ready.";

        // 内部パラメータ
        CvMat* intrinsic_matrix = cvCreateMat(cameraMatrix.rows, cameraMatrix.cols, CV_32FC1);
        for(int i = 0; i < cameraMatrix.rows; ++i)
        {
            for(int j = 0; j < cameraMatrix.cols; ++j)
            {
                cvmSet(intrinsic_matrix, i, j, cameraMatrix.at<double>(i, j));
            }
        }

        qDebug() << "intrinsic_matrix ready.";

        // 歪み係数
        CvMat* distortion_coeffs = cvCreateMat(distCoeffs.rows, 4, CV_32FC1);
        for(int i = 0; i < distCoeffs.rows; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                cvmSet(distortion_coeffs, i, j, distCoeffs.at<double>(i, j));
            }
        }

        qDebug() << "distortion_coeffs ready.";

        CvMat* rotation_vector = cvCreateMat(1, 3, CV_32FC1);
        CvMat* translation_vector = cvCreateMat(1, 3, CV_32FC1);

        cvFindExtrinsicCameraParams2(&object_points, image_points, intrinsic_matrix, distortion_coeffs, rotation_vector, translation_vector);

        Mat tmpRot(rotation_vector, true);
        rotationVec = tmpRot.clone();

        Mat tmpTrans(translation_vector, true);
        transVec = tmpTrans.clone();
    }
}

int Utilities::getKeyPointsOfSURF(const Mat src, vector<KeyPoint> &keypoints, vector<float> &descriptors)
{
    Mat gray;
    cvtColor(src, gray, CV_BGR2GRAY);

    SURF s(500, 4, 2, true);
    s(gray, Mat(), keypoints, descriptors);

    return s.descriptorSize();
}

void Utilities::getKeyPointsOfSURF_gpu(const Mat src, vector<KeyPoint>& keypoints, gpu::GpuMat& descriptors)
{
    // GPU状にGRAY画像をコピー
    Mat gray;
    cvtColor(src, gray, CV_BGR2GRAY);
    gpu::GpuMat gpuImage(gray);

    // SURF検出
    gpu::SURF_GPU surfGPU(500, 4, 2, true);

    // 画像から特徴点と特徴量を検出
    surfGPU(gpuImage, gpu::GpuMat(), keypoints, descriptors);
}

void Utilities::drawKeyPoints(Mat &src, const vector<KeyPoint> keypoints)
{
    vector<KeyPoint> kp = keypoints;
    vector<KeyPoint>::iterator it;
    for(it = kp.begin(); it != kp.end(); ++it)
    {
        circle(src, Point(it->pt.x, it->pt.y), saturate_cast<int>(it->size * 0.25), Scalar(255, 0, 0));
    }
}
