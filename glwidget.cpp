#include "glwidget.h"
#include "QtOpenGL"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent),
      util(new Utilities())
{
    xRot = 0;
    yRot = 0;
    zRot = 0;

    xOrigin = 0.0;
    yOrigin = 0.0;
    zOrigin = 0.0;

    glScale = 1.0;

    state_Axis = true;
    state_Light = true;
    state_Mesh = false;
    state_Bone = true;

    farThresh = 0;

    bones.clear();
    points.clear();
    colors.clear();
    vertex.release();
    face.release();
    rectPoints.release();
    rectColors.release();
    bgPicture.release();

    pointSize = 1.0;
}

static void qNormalizeAngle(int &angle)
{
    while(angle < 0 )
        angle += 360 * 16;

    while(angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);

    if(angle != xRot)
    {
        xRot = angle;
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);

    if(angle != yRot)
    {
        yRot = angle;
        updateGL();
    }
}
void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if(angle != zRot)
    {
        zRot = angle;
        updateGL();
    }
}

void GLWidget::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}

void GLWidget::resizeGL(int width,int height)
{
    glViewport(0, 0, width, height);

    //現在選択されている行列に単位行列をロードする
    glLoadIdentity();
}

void GLWidget::drawAxis()
{
    if(state_Axis == true)
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        //X軸
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(-1.0, 0.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glVertex3f(0.9, 0.1, 0.0);
        glVertex3f(0.9, -0.1, 0.0);
        glEnd();

        //Y軸
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, -1.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
        glVertex3f(-0.1, 0.9, 0.0);
        glVertex3f(0.1, 0.9, 0.0);
        glEnd();

        //Z軸
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, -1.0);
        glVertex3f(0.0, 0.0, 1.0);
        glVertex3f(0.0, -0.1, 0.9);
        glVertex3f(0.0, 0.1, 0.9);
        glEnd();
    }
}

void GLWidget::drawLight()
{
    if(state_Light == true)
    {
        //光源の位置
        GLfloat light0_pos[] = { 1.0, 0.0, 1.0, 0.0 };
        GLfloat light1_pos[] = { 0.0, -1.0, -1.0, 0.0 };

        //光源の色
        GLfloat light_color[] = { 1.0, 1.0, 1.0, 1.0 };

        //0番目の光源の設定
        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
        glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }
}
void GLWidget::drawObject()
{
    if(!state_Mesh && !vertex.empty() && !face.empty())
    {
        Mat normVertex;
        normalize(vertex, normVertex, -1.0, 1.0, NORM_MINMAX);

        for(int i = 0; i < face.rows; ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                p1[j] = normVertex.at<double>((int)(face.at<double>(i, 0) - 1.0), j);
                p2[j] = normVertex.at<double>((int)(face.at<double>(i, 1) - 1.0), j);
                p3[j] = normVertex.at<double>((int)(face.at<double>(i, 2) - 1.0), j);
            }

            if(calcNormal() == 1)
            {
                glNormal3f(n[0], n[1], n[2]);
            }

            if(state_Light == false)
            {
                glColor3f(1.0, 1.0, 1.0);
            }

            glBegin(GL_POLYGON);
            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
            glVertex3f(p3[0], p3[1], p3[2]);
            glEnd();
        }
    }

    if(state_Mesh && !vertex.empty() && !face.empty())
    {
        Mat normVertex;
        normalize(vertex, normVertex, -1.0, 1.0, NORM_MINMAX);

        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // デフォルト緑
        glColor3f(0.0, 1.0, 0.0);

        for(int i = 0; i < face.rows; ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                p1[j] = normVertex.at<double>((int)(face.at<double>(i, 0) - 1.0), j);
                p2[j] = normVertex.at<double>((int)(face.at<double>(i, 1) - 1.0), j);
                p3[j] = normVertex.at<double>((int)(face.at<double>(i, 2) - 1.0), j);
            }

            glBegin(GL_LINE_LOOP);
            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
            glVertex3f(p3[0], p3[1], p3[2]);
            glEnd();
        }
    }

    if(state_Bone && !bones.empty())
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // 線で繋ぐ
        glLineWidth(8.0);

        // デフォルトで青
        glColor3d(0.0, 0.0, 1.0);

        glBegin(GL_LINE_STRIP);
        for(int i = 0; i < (int)bones.size(); ++i)
        {
            glVertex3d(bones.at(i).x, bones.at(i).y, bones.at(i).z);
        }
        glEnd();
        glLineWidth(1.0);

        // 点を描画
        glPointSize(10.0);

        // デフォルトで赤
        glColor3d(1.0, 0.0, 0.0);

        glBegin(GL_POINTS);
        for(int i = 0; i < (int)bones.size(); ++i)
        {
            glVertex3d(bones.at(i).x, bones.at(i).y, bones.at(i).z);
        }
        glEnd();

        glPointSize(1.0);
    }

    if(!points.empty())
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // 点を描画
        glPointSize(5.0);

        glBegin(GL_POINTS);
        for(int i = 0; i < (int)points.size(); ++i)
        {
            // デフォルトで黄
            if(!colors.empty())
                glColor3d(colors.at(i).x, colors.at(i).y, colors.at(i).z);
            else
                glColor3f(1.0, 1.0, 0.0);

            glVertex3d(points.at(i).x, points.at(i).y, points.at(i).z);
        }
        glEnd();

        glPointSize(1.0);
    }

    if(!rectPoints.empty())
    {
        Mat points64;
        rectPoints.convertTo(points64, CV_64F);

        Mat normPoints;
        normalize(points64, normPoints, -1.0, 1.0, NORM_MINMAX);

        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // 点を描画
        glPointSize(pointSize);

        glBegin(GL_POINTS);
        for(int i = 0; i < normPoints.rows; ++i)
        {
            for(int j = 0; j < normPoints.cols; ++j)
            {
                // デフォルトで黄
                if(!rectColors.empty())
                {
                    if(rectColors.channels() == 3)
                    {
                        glColor3f((float)rectColors.at<Vec3b>(i, j)(2) / 255.0,
                                  (float)rectColors.at<Vec3b>(i, j)(1) / 255.0,
                                  (float)rectColors.at<Vec3b>(i, j)(0) / 255.0);
                    }
                    else if(rectColors.channels() == 1)
                    {
                        glColor3f((float)rectColors.at<uchar>(i, j) / 255.0,
                                  (float)rectColors.at<uchar>(i, j) / 255.0,
                                  (float)rectColors.at<uchar>(i, j) / 255.0);
                    }
                    else
                    {
                        glColor3f(1.0, 1.0, 1.0);
                    }
                }
                else
                {
                    glColor3f(1.0, 1.0, 1.0);
                }

                // zは値が増えるほど近い(kinect入力)
                if(normPoints.at<double>(i, j) != 1.0
                        && normPoints.at<double>(i, j) > ((double)farThresh / 100.0) * 2.0 - 1.0)
                {
                    glVertex3d(((double)j / (double)normPoints.cols) * 2.0 - 1.0,
                               (-(double)i / (double)normPoints.rows) * 2.0 + 1.0,
                               normPoints.at<double>(i, j));
                }
            }
        }
        glEnd();

        glPointSize(1.0);
    }

    if(!bgPicture.empty())
    {
        //ライティングを一時的にオフに
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);

        // 点を描画
        glPointSize((double)this->width() / (double)bgPicture.cols);

        glBegin(GL_POINTS);
        for(int i = 0; i < bgPicture.rows; ++i)
        {
            for(int j = 0; j < bgPicture.cols; ++j)
            {
                // デフォルトで黄
                glColor3f((float)bgPicture.at<Vec3b>(i, j)(2) / 255.0,
                          (float)bgPicture.at<Vec3b>(i, j)(1) / 255.0,
                          (float)bgPicture.at<Vec3b>(i, j)(0) / 255.0);

                glVertex3d(((double)j / (double)bgPicture.cols) * 2.0 - 1.0,
                           (-(double)i / (double)bgPicture.rows) * 2.0 + 1.0,
                           -1.0);
            }
        }
        glEnd();

        glPointSize(1.0);
    }
}

void GLWidget::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    //拡大縮小・平行移動・回転を反映
    glScalef(glScale, glScale, glScale);
    glTranslatef(xOrigin, yOrigin, zOrigin);

    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

    // 正射影
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    // 透視投影
    //gluPerspective(30.0, (double)this->width() / (double)this->height(), 1.0, 100.0);

    //座標軸を描画
    drawAxis();

    //光源を設定
    drawLight();

    //objectを描画
    drawObject();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if(event->buttons() & Qt::LeftButton)
    {
        setXRotation(xRot + 8  *dy);
        setYRotation(yRot + 8  *dx);
    }
    else if(event->buttons() & Qt::RightButton)

    {
        setXRotation(xRot + 8  *dy);
        setZRotation(zRot + 8  *dx);
    }
    lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    // 移動量
    double shiftAmount = 0.01;

    if(e->key() == Qt::Key_X)
    {
        setXRotation(0);
        setYRotation(-90  *16);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Y)
    {
        setXRotation(-90  *16);
        setYRotation(0);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Z)
    {
        setXRotation(0);
        setYRotation(0);
        setZRotation(0);
    }
    if(e->key() == Qt::Key_Left)
        xOrigin = xOrigin - shiftAmount;
    if(e->key() == Qt::Key_Up)
        yOrigin = yOrigin + shiftAmount;
    if(e->key() == Qt::Key_Right)
        xOrigin = xOrigin + shiftAmount;
    if(e->key() == Qt::Key_Down)
        yOrigin = yOrigin - shiftAmount;
    if(e->key() == Qt::Key_B)
        glScale = glScale + shiftAmount;
    if(e->key() == Qt::Key_S)
        glScale = glScale - shiftAmount;
    if(e->key() == Qt::Key_A)
    {
        if(state_Axis)
            state_Axis = false;
        else
            state_Axis = true;
    }
    if(e->key() == Qt::Key_B)
    {
        if(state_Bone)
            state_Bone = false;
        else
            state_Bone = true;
    }
    if(e->key() == Qt::Key_L)
    {
        if(state_Light)
            state_Light = false;
        else
            state_Light = true;
    }
    if(e->key() == Qt::Key_M)
    {
        if(state_Mesh)
            state_Mesh = false;
        else
            state_Mesh = true;
    }

    updateGL();
}

int GLWidget::calcNormal()
{
    double v1[3];
    double v2[3];
    double cross[3];
    double length;

    //v1 = p2 - p1を求める
    for(int i = 0; i < 3; i++)
    {
        v1[i] = p2[i] - p1[i];
    }

    //v2 = p3 - p1を求める
    for(int i = 0; i < 3; i++)
    {
        v2[i] = p3[i] - p1[i];
    }

    //外積v2×v1（= cross）を求める
    for(int i = 0; i < 3; i++)
    {
        cross[i] = v2[(i + 1) % 3] * v1[(i + 2) % 3] - v2[(i + 2) % 3] * v1[(i + 1) % 3];
    }

    //外積v2×v1の長さ|v2×v1|（= length）を求める
    length = sqrt(cross[0]  *cross[0] + cross[1]  *cross[1] + cross[2]  *cross[2]);

    //長さ|v2×v1|が0のときは法線ベクトルは求められない
    if(length == 0.0)
    {
        return 0;
    }

    else
    {
        //外積v2×v1を長さ|v2×v1|で割って法線ベクトルnを求める
        for(int i = 0; i < 3; i++)
        {
            n[i] = cross[i] / length;
        }

        return 1;
    }
}

QPixmap GLWidget::getGLViewAsPixmap()
{
    QPixmap view = QPixmap(width(), height());
    view = renderPixmap();
    return view;
}

Mat GLWidget::getGLViewAsMat()
{
    QPixmap view = QPixmap(width(), height());
    view = renderPixmap();

    Mat viewMat(height(), width(), CV_8UC3);
    for(int i = 0; i < viewMat.rows; ++i)
    {
        uchar *p = view.toImage().scanLine(i);
        for(int j = 0; j < viewMat.cols; ++j)
        {
            viewMat.at<Vec3b>(i, j)[0] = *(p + 0);
            viewMat.at<Vec3b>(i, j)[1] = *(p + 1);
            viewMat.at<Vec3b>(i, j)[2] = *(p + 2);

            // アルファチャンネルは無視
            p += 4;
        }
    }

    return viewMat;
}

void GLWidget::setVertexMat(const Mat &_vertex)
{
    this->vertex = _vertex.clone();
}

void GLWidget::setFaceMat(const Mat &_face)
{
    this->face = _face.clone();
}

Mat GLWidget::getVertexMat()
{
    return this->vertex;
}

Mat GLWidget::getFaceMat()
{
    return this->face;
}

void GLWidget::setAxis(bool _state)
{
    state_Axis = _state;
}

void GLWidget::setLight(bool _state)
{
    state_Light = _state;
}

void GLWidget::setBones(const vector<Point3d> &_points)
{
    bones = _points;
}

void GLWidget::setPoints(const vector<Point3d> &_points)
{
    this->points.clear();
    this->points = _points;
}

void GLWidget::setRectPoints(const Mat &_rectPoints)
{
    this->rectPoints = _rectPoints;
}

void GLWidget::pushPoint(const Point3d &_point)
{
    points.push_back(_point);
}

void GLWidget::setColors(const vector<Point3d> &_colors)
{
    this->colors.clear();
    this->colors = _colors;
}

void GLWidget::setRectColors(const Mat &_rectColors)
{
    this->rectColors = _rectColors;
}

void GLWidget::setScale(double _scale)
{
    this->glScale = _scale;
}

void GLWidget::setFarThresh(int value)
{
    this->farThresh = value;
}

void GLWidget::setBGPicture(Mat &_bg)
{
    this->bgPicture = _bg;
}

void GLWidget::setPointSize(double size)
{
    this->pointSize = size;
}
