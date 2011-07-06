#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "glwidget.h"
#include "kinectthread.h"
#include "utilities.h"
#include <opencv2/gpu/gpu.hpp>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showGL(vector<Mat> depth, vector<Mat> color);

private slots:
    void on_pushButton_StartKinect_clicked();
    void on_pushButton_SaveIRImage_clicked();
    void on_pushButton_Calibration_clicked();
    void on_horizontalSlider_GLPointSize_valueChanged(int value);
    void on_pushButton_Xup_clicked();
    void on_pushButton_Xdown_clicked();
    void on_pushButton_Yup_clicked();
    void on_pushButton_Ydown_clicked();
    void on_pushButton_Zup_clicked();
    void on_pushButton_Zdown_clicked();
    void on_lineEdit_MoveStep_returnPressed();
    void on_checkBox_IrOrImage_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    Utilities *util;
    vector<GLWidget*> glView;
    KinectThread *kinectTh;

    int irImageNum;
    int x, y, z, step;
    vector<Point3f> objectPoint;
    vector<Mat> cameraMatrixes;
    vector<Mat> distCoeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;

    // カメラごと
    vector<Mat> lastFrames;
    vector<vector<KeyPoint> > lastKeyPoints;
    vector<gpu::GpuMat> lastDescriptors;

    vector<vector<KeyPoint> > lastKeyPoints_cpu;
    vector<Mat> lastDescriptors_cpu;
};

#endif // MAINWINDOW_H
