#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <opencv.hpp>
#include "utilities.h"

using namespace cv;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    QPixmap getGLViewAsPixmap();
    int heightForWidth(int w);

    Mat getGLViewAsMat();
    void setVertexMat(const Mat &_vertex);
    void setFaceMat(const Mat &_face);
    Mat getVertexMat();
    Mat getFaceMat();
    void setBones(const vector<Point3d> &_points);
    void setPoints(const vector<Point3d> &_points);
    void setColors(const vector<Point3d> &_colors);
    void setRectPoints(const Mat &_rectPoints);
    void setRectColors(const Mat &_rectColors);
    void pushPoint(const Point3d &_point);
    void setFarThresh(int value);
    void setBGPicture(Mat &_bg);

    void setAxis(bool _state);
    void setLight(bool _state);
    void setScale(double _scale);
    void setPointSize(double size);

signals:

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

protected:
    void initializeGL();
    void resizeGL(int width, int heigth);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void drawAxis();
    void drawLight();
    void drawObject();

private:
    int calcNormal();

    Mat vertex;
    Mat face;
    vector<Point3d> bones;
    vector<Point3d> points;
    vector<Point3d> colors;
    Mat rectPoints;
    Mat rectColors;
    Mat bgPicture;

    int xRot;
    int yRot;
    int zRot;

    double xOrigin;
    double yOrigin;
    double zOrigin;

    double glScale;
    QPoint lastPos;

    double p1[3];
    double p2[3];
    double p3[3];
    double n[3];

    bool state_Axis;
    bool state_Light;
    bool state_Mesh;
    bool state_Bone;

    int farThresh;

    Utilities *util;

    double pointSize;
};

#endif // GLWIDGET_H
