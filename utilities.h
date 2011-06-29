#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QPixmap>
#include <QDebug>
#include <opencv.hpp>

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
    void saveCSVFromVector(const QString fileName, const vector<T> vec)
    {
        QFile fp(fileName);
        if(fp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream st(&fp);
            for(int i = 0; i < (int)vec.size(); ++i)
            {
                st << vec.at(i) << "\n";
            }
            fp.close();
        }
    }

    void saveCSVFromMat(const QString fileName, const Mat src);

signals:
    void appendConsoleText(QString text);

public slots:
    void sendConsoleText(QString text);

};

#endif // UTILITIES_H
