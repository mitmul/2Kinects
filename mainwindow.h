#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "glwidget.h"
#include "kinectthread.h"

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
    void on_checkBox_ImageOrIR_clicked(bool checked);

    void on_horizontalSlider_GLPointSize_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    vector<GLWidget*> glView;
    KinectThread *kinectTh;

    int irImageNum;
};

#endif // MAINWINDOW_H
