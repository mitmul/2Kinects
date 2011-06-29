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
                    st << dst.at<double>(i, j) << " ";
                }
                else
                {
                    for(int k = 0; k < dst.channels(); ++k)
                    {
                        st << dst.at<Vec3d>(i, j)(k) << " ";
                    }
                }
            }
            st << "\n";
        }
        fp.close();
    }
}
