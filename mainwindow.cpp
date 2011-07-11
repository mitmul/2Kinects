#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    util(new Utilities(this)),
    kinectTh(NULL),
    irImageNum(0),
    x(0),
    y(0),
    z(0),
    step(100)
{
    ui->setupUi(this);
    this->setWindowTitle("2Kinects");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_StartKinect_clicked()
{
    if(kinectTh == NULL)
    {
        kinectTh = new KinectThread(!ui->checkBox_IrOrImage->isChecked());
        connect(kinectTh, SIGNAL(drawGL(vector<Mat>,vector<Mat>)), this, SLOT(showGL(vector<Mat>,vector<Mat>)));
        kinectTh->start();
    }
}

void MainWindow::showGL(vector<Mat> depth, vector<Mat> color)
{
    // No GLView, Get Image, No Synthesize
    if(glView.empty() && !ui->checkBox_IrOrImage->isChecked() && !ui->checkBox_Synthesize->isChecked())
    {
        glView.resize(depth.size());

        for(int i = 0; i < glView.size(); ++i)
        {
            glView.at(i) = new GLWidget();
            glView.at(i)->setGeometry(500 * i + 10, 10, 500, 500);
            QString winTitle;
            winTitle.sprintf("Kinect %d", i);
            glView.at(i)->setWindowTitle(winTitle);
            glView.at(i)->show();

            qDebug() << i;
        }
    }

    // Num of GLView is Num of Depth, Kinect thread run
    if(glView.size() == depth.size() && kinectTh != NULL)
    {
        for(int i = 0; i < depth.size(); ++i)
        {
            glView.at(i)->setRectPoints(depth.at(i));
            glView.at(i)->setRectColors(color.at(i));
            glView.at(i)->updateGL();
        }
    }

    // No GLView, Synthesize ON
    if(glView.empty() && ui->checkBox_Synthesize->isChecked())
    {
        glView.resize(1);

        glView.at(0) = new GLWidget();
        glView.at(0)->setGeometry(0, 10, 500, 500);
        glView.at(0)->setMinimumSize(500, 500);
        QSizePolicy sp;
        sp.setHeightForWidth(true);
        glView.at(0)->setSizePolicy(sp);
        glView.at(0)->setWindowTitle("Synthesized");
        glView.at(0)->show();
    }

    // 1 GLView, Kiect Thread run
    if(ui->checkBox_Synthesize->isChecked() && glView.size() == 1 && kinectTh != NULL)
    {
        // 一つ目のデプスだけ90度回して結合
        vector<Point3d> vecsA = util->getVecsFromMat(depth.at(1));
        util->rotatePoint3dVecsAroundY(-M_PI / 2.0, vecsA);
        util->moveVecs(Point3d(x, y, z), vecsA);
        vector<Point3d> vecsB = util->getVecsFromMat(depth.at(0));
        vector<Point3d> vecsAB = util->synthVecs(vecsA, vecsB);

        vector<Point3d> colorA = util->getVecsFromMat(color.at(1));
        vector<Point3d> colorB = util->getVecsFromMat(color.at(0));
        vector<Point3d> colorAB = util->synthVecs(colorA, colorB);

        glView.at(0)->setPoints(vecsAB);
        glView.at(0)->setColors(colorAB);
        glView.at(0)->updateGL();
    }

    // IR mode
    if(ui->checkBox_IrOrImage->isChecked())
    {
        imshow("Kinect 0 IR", color.at(0));
        setWindowProperty("Kinect 0 IR", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_FREERATIO);

        imshow("Kinect 1 IR", color.at(1));
        setWindowProperty("Kinect 1 IR", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_FREERATIO);
    }

    kinectTh->active();
}

void MainWindow::on_pushButton_SaveIRImage_clicked()
{
    if(ui->checkBox_IrOrImage->isChecked() && kinectTh != NULL)
    {
        vector<Mat> ir;
        kinectTh->getIRImages(ir);

        QString fname;
        for(int i = 0; i < ir.size(); ++i)
        {
            fname.sprintf("ir%d-%d.png", i, irImageNum);

            vector<Point2f> corners;

            if(findChessboardCorners(ir.at(i), Size(7, 10), corners) == true)
            {
                Mat corner = ir.at(i).clone();
                drawChessboardCorners(corner, Size(7, 10), corners, true);

                imwrite(fname.toStdString(), ir.at(i));
                destroyWindow(fname.toStdString());
                imshow(fname.toStdString(), corner);
                cvMoveWindow(fname.toStdString().data(), 640 * i, 10);
            }
            else
            {
                destroyWindow(fname.toStdString());
            }
        }
        irImageNum++;
    }
}

void MainWindow::on_pushButton_Calibration_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open IRImages", QDir::homePath());

    if(!fileNames.empty())
    {
        int patternRow = 10;
        int patternCol = 7;
        float chessSize = 24.0;

        // チェスパターンの点座標
        vector<vector<Point3f> > objectPoints;
        vector<Point3f> OnAImage;
        for(int i = 0; i < patternRow; ++i)
        {
            for(int j = 0; j < patternCol; ++j)
            {
                Point3f point;
                point.x = chessSize * (float)j;
                point.y = chessSize * (float)i;
                point.z = 0.0;

                OnAImage.push_back(point);
            }
        }

        for(int i = 0; i < fileNames.size(); ++i)
        {
            objectPoints.push_back(OnAImage);
        }

        // 画像上の格子点
        vector<vector<Point2f> > imagePoints;
        vector<Point2f> corners;
        for(int i = 0; i < fileNames.size(); ++i)
        {
            QString fname = fileNames.at(i);
            Mat image = imread(fname.toStdString());

            corners.clear();

            if(findChessboardCorners(image, Size(7, 10), corners) == true)
            {
                drawChessboardCorners(image, Size(7, 10), corners, true);
                imshow("corner detected", image);
                QString windN; windN.sprintf("%d", i);
                displayOverlay("corner detected", windN.toStdString(), 1);
                waitKey(1);

                // 追加
                imagePoints.push_back(corners);
            }
            else
            {
                qDebug() << fname << "corner detect failed!";
            }
        }

        // 内部パラメータ
        Mat cameraMatrix;

        // 歪み係数
        Mat distCoeffs;

        // 回転ベクトル
        vector<Mat> rvecs;

        // 並進ベクトル
        vector<Mat> tvecs;

        // キャリブレーション
        double reproError = calibrateCamera(objectPoints, imagePoints, Size(640, 480), cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_RATIONAL_MODEL);

        // ウインドウを消す
        destroyWindow("corner detected");

        util->saveCSVFromMat("ir1-cameraMatrix.csv", cameraMatrix);
        util->saveCSVFromMat("ir1-distCoeffs.csv", distCoeffs);

        qDebug() << "Reprojection error:" << reproError;
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

void MainWindow::on_pushButton_Xup_clicked()
{
    x = x + step;
}

void MainWindow::on_pushButton_Xdown_clicked()
{
    x = x - step;
}

void MainWindow::on_pushButton_Yup_clicked()
{
    y = y + step;
}

void MainWindow::on_pushButton_Ydown_clicked()
{
    y = y - step;
}

void MainWindow::on_pushButton_Zup_clicked()
{
    z = z + step;
}

void MainWindow::on_pushButton_Zdown_clicked()
{
    z = z - step;
}

void MainWindow::on_lineEdit_MoveStep_returnPressed()
{
    step = ui->lineEdit_MoveStep->text().toInt();
}

void MainWindow::on_checkBox_IrOrImage_clicked(bool checked)
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
