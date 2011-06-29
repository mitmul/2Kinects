#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    kinectTh(NULL),
    irImageNum(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_StartKinect_clicked()
{
    if(kinectTh == NULL)
    {
        kinectTh = new KinectThread(!ui->checkBox_ImageOrIR->isChecked());
        connect(kinectTh, SIGNAL(drawGL(vector<Mat>,vector<Mat>)), this, SLOT(showGL(vector<Mat>,vector<Mat>)));
        kinectTh->start();
    }
}

void MainWindow::showGL(vector<Mat> depth, vector<Mat> color)
{
    if(glView.empty() && !ui->checkBox_ImageOrIR->isChecked())
    {
        glView.resize(depth.size());

        for(int i = 0; i < glView.size(); ++i)
        {
            glView.at(i) = new GLWidget();
            GLWidget a;
            glView.at(i)->setGeometry(640 * i + 10, 10, 640, 480);
            glView.at(i)->show();
        }
    }
    if(!glView.empty() && kinectTh != NULL)
    {
        for(int i = 0; i < depth.size(); ++i)
        {
            glView.at(i)->setRectPoints(depth.at(i));
            glView.at(i)->setRectColors(color.at(i));
            glView.at(i)->updateGL();
        }
    }

    kinectTh->active();
}

void MainWindow::on_pushButton_SaveIRImage_clicked()
{
    if(ui->checkBox_ImageOrIR->isChecked() && kinectTh != NULL)
    {
        vector<Mat> ir;
        kinectTh->getIRImages(ir);

        QString fname;
        for(int i = 0; i < ir.size(); ++i)
        {
            fname.sprintf("../../../ir%d-%d.png", i, irImageNum);
            imwrite(fname.toStdString(), ir.at(i));
        }
        irImageNum++;
    }
}

void MainWindow::on_pushButton_Calibration_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open IRImages", QDir::homePath());

    if(!fileNames.empty())
    {
        for(int i = 0; i < fileNames.size(); ++i)
        {
            QString fname = fileNames.at(i);
            Mat image = imread(fname.toStdString());

            vector<Point2f> corners;

            if(findChessboardCorners(image, Size(6, 8), corners) == true)
            {
                drawChessboardCorners(image, Size(6, 8), corners, true);

                fname.sprintf("../../../ok%d.png", i);
                imwrite(fname.toStdString(), image);

                qDebug() << "corner detected!";
            }
            else
            {
                qDebug() << "corner detect failed!";
            }
        }
    }
}

void MainWindow::on_checkBox_ImageOrIR_clicked(bool checked)
{
    if(kinectTh == NULL)
    {
        // チェックされたらIR画像に
        if(checked)
        {
            // IR画像保存ボタンを有効にする
            ui->pushButton_SaveIRImage->setEnabled(checked);
        }

        // チェックはずされたら可視画像に
        if(!checked)
        {
            // IR画像保存ボタンを無効にする
            ui->pushButton_SaveIRImage->setEnabled(checked);
        }
    }
}

void MainWindow::on_horizontalSlider_GLPointSize_valueChanged(int value)
{
    if(glView.size() != 0)
    {
        for(int i = 0; i < glView.size(); ++i)
        {
            glView.at(i)->setPointSize((double)value / 10);
        }
    }
}
