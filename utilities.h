#ifndef UTILITIES_H
#define UTILITIES_H

#include <vector>
#include <QObject>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QPixmap>
#include <QDebug>
#include <opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace std;
using namespace cv;

class Utilities : public QObject
{
    Q_OBJECT

public:
    explicit Utilities(QObject *parent = 0);

    int getVLineNumberFromObj(const QString fileName);
    int getFLineNumberFromObj(const QString fileName);
    void getFLinesFromObj(const QString fileName, Mat &fLines);
    void getVLinesFromObj(const QString fileName, Mat &vLines);
    void saveObjFileFromVLinesFLines(const QString fileName, const Mat vLines, const Mat fLines);
    void moveModelToOrigin(Mat &vertex);

    template<class T>
    void saveCSVFromVector(const QString fileName, const vector<T> vecs)
    {
        QFile fp(fileName);
        if(fp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream st(&fp);
            for(int i = 0; i < (int)vecs.size(); ++i)
            {
                st << vecs.at(i) << "\n";
            }
            fp.close();
        }
    }

    void saveCSVFromPoint3fVector(const QString fileName, const vector<Point3f> vecs);

    void saveCSVFromMat(const QString fileName, const Mat src);
    vector<Point3d> getVecsFromMat(const Mat src, const Mat cameraMat);
    void rotatePoint3dVecsAroundY(const double rotate, vector<Point3d>& vecs);
    vector<Point3d> synthVecs(const vector<Point3d> vecA, const vector<Point3d> vecB);
    void moveVecs(const Point3d shift, vector<Point3d>& vecs);
    void getMatFromCSV(const QString fileName, Mat& dst);
    void getExtrinsicMatrix(const Mat initCalib, const Size pttrnSize, const vector<Point3f> objectPoint, const Mat cameraMatrix, const Mat distCoeffs, Mat &rotationVec, Mat &transVec);
    int getKeyPointsOfSURF(const Mat src, vector<KeyPoint>& keypoints, vector<float>& descriptors);
    void getKeyPointsOfSURF_gpu(const Mat src, vector<KeyPoint>& keypoints, gpu::GpuMat& descriptors);
    void drawKeyPoints(Mat& src, const vector<KeyPoint> keypoints);

signals:
    void appendConsoleText(QString text);

public slots:
    void sendConsoleText(QString text);

};

#endif // UTILITIES_H
