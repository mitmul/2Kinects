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

    void saveCSVFromMat(const QString fileName, const Mat src);
    vector<Point3d> getVecsFromMat(const Mat src);
    void rotatePoint3dVecsAroundY(const double rotate, vector<Point3d>& vecs);
    vector<Point3d> synthVecs(const vector<Point3d> vecA, const vector<Point3d> vecB);
    void moveVecs(const Point3d shift, vector<Point3d>& vecs);

signals:
    void appendConsoleText(QString text);

public slots:
    void sendConsoleText(QString text);

};

#endif // UTILITIES_H
