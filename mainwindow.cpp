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
        }
    }

    // Num of GLView is Num of Depth, Kinect thread run
    if(glView.size() == depth.size() && kinectTh != NULL)
    {
        for(int i = 0; i < depth.size(); ++i)
        {
            if(lastFrames.size() != depth.size())
            {
                lastFrames.push_back(color.at(i));
            }

#if 0
            // forGPU
            if(lastKeyPoints.size() != depth.size())
            {
                vector<KeyPoint> lastKp;
                gpu::GpuMat lastDesc;
                util->getKeyPointsOfSURF_gpu(color.at(i), lastKp, lastDesc);

                lastKeyPoints.push_back(lastKp);
                lastDescriptors.push_back(lastDesc);
            }

            //--------GPU処理--------//
            clock_t old_gpu = clock();

            // SURF検出（GPU）
            vector<KeyPoint> keypoints_gpu;
            gpu::GpuMat descriptors_gpu;
            util->getKeyPointsOfSURF_gpu(color.at(i), keypoints_gpu, descriptors_gpu);

            // マッチング（GPU）
            gpu::BruteForceMatcher_GPU<L2<float> > matcher_gpu;
            gpu::GpuMat trainIdx_gpu, distance_gpu;
            matcher_gpu.matchSingle(descriptors_gpu, lastDescriptors.at(i), trainIdx_gpu, distance_gpu);

            // 描画(GPU)
            vector<DMatch> matches_gpu;
            gpu::BruteForceMatcher_GPU<L2<float> >::matchDownload(trainIdx_gpu, distance_gpu, matches_gpu);

            Mat imageMatch_gpu;
            drawMatches(color.at(i), keypoints_gpu, lastFrames.at(i), lastKeyPoints.at(i), matches_gpu, imageMatch_gpu);

            imshow("matching", imageMatch_gpu);

            clock_t new_gpu = clock();

            float procTime_gpu = (float)(new_gpu - old_gpu) / (float)CLOCKS_PER_SEC;
            qDebug() << "GPU:" << procTime_gpu << "(sec)";
            //----------------------//


            // 現フレームを保存(GPU)
            if(lastKeyPoints.size() == depth.size())
            {
                lastKeyPoints.at(i) = keypoints_gpu;
                lastDescriptors.at(i) = descriptors_gpu;
            }

#else
            // forCPU
            if(lastKeyPoints_cpu.size() != depth.size())
            {
                vector<KeyPoint> lastKp;
                vector<float> lastDesc;
                int descSize = util->getKeyPointsOfSURF(color.at(i), lastKp, lastDesc);

                // Matへ
                Mat desc_cpuMat(lastKp.size(), descSize, CV_32FC1);
                for(int i = 0; i < desc_cpuMat.rows; ++i)
                {
                    for(int j = 0; j < desc_cpuMat.cols; ++j)
                    {
                        desc_cpuMat.at<float>(i, j) = lastDesc.at(i * desc_cpuMat.cols + j);
                    }
                }

                lastKeyPoints_cpu.push_back(lastKp);
                lastDescriptors_cpu.push_back(desc_cpuMat);
            }

            //---------CPU処理------//
            clock_t old_cpu = clock();

            // SURF検出（CPU）
            vector<KeyPoint> keypoints_cpu;
            vector<float> descriptors_cpu;
            int descriptorSize = util->getKeyPointsOfSURF(color.at(i), keypoints_cpu, descriptors_cpu);

            // Matへ
            Mat descriptors_cpuMat(keypoints_cpu.size(), descriptorSize, CV_32FC1);
            for(int i = 0; i < descriptors_cpuMat.rows; ++i)
            {
                for(int j = 0; j < descriptors_cpuMat.cols; ++j)
                {
                    descriptors_cpuMat.at<float>(i, j) = descriptors_cpu.at(i * descriptors_cpuMat.cols + j);
                }
            }

            // マッチング（CPU）
            BruteForceMatcher<L2<float> > matcher_cpu;
            vector<DMatch> matches_cpu;
            matcher_cpu.match(descriptors_cpuMat, lastDescriptors_cpu.at(i), matches_cpu);

            // 描画(CPU)
            Mat imageMatch_cpu;
            drawMatches(color.at(i), keypoints_cpu, lastFrames.at(i), lastKeyPoints_cpu.at(i), matches_cpu, imageMatch_cpu);

            imshow("matching_cpu", imageMatch_cpu);

            clock_t new_cpu = clock();

            float procTime_cpu = (float)(new_cpu - old_cpu) / (float)CLOCKS_PER_SEC;
            qDebug() << "CPU:" << procTime_cpu << "(sec)";
            qDebug() << "descriptors:" << descriptorSize << "keypoints:" << keypoints_cpu.size() << keypoints_cpu.size() << "\n";
            //-----------------------//

            // 現フレームを保存(CPU)
            if(lastKeyPoints_cpu.size() == depth.size())
            {
                lastKeyPoints_cpu.at(i) = keypoints_cpu;
                lastDescriptors_cpu.at(i) = descriptors_cpuMat;
            }
#endif
            if(lastFrames.size() == depth.size())
            {
                lastFrames.at(i) = color.at(i).clone();
            }
        }
    }

    // No GLView, Synthesize ON
    if(glView.empty() && ui->checkBox_Synthesize->isChecked())
    {
        glView.resize(1);
        glView.at(0) = new GLWidget();
        glView.at(0)->setGeometry(0, 10, 500, 500);
        glView.at(0)->setMinimumSize(500, 500);
        glView.at(0)->sizePolicy().setHeightForWidth(true);
        glView.at(0)->setWindowTitle("Synthesized");
        glView.at(0)->show();

        // カメラパラメータ読み込み
        cameraMatrixes.clear();
        for(int i = 0; i < depth.size(); ++i)
        {
            Mat camera;
            QString fname;
            fname.sprintf("ir%d-cameraMatrix.csv", i);
            util->getMatFromCSV(fname, camera);
            cameraMatrixes.push_back(camera);
        }

        // 歪み係数
        distCoeffs.clear();
        for(int i = 0; i < depth.size(); ++i)
        {
            Mat dist;
            QString fname;
            fname.sprintf("ir%d-distCoeffs.csv", i);
            util->getMatFromCSV(fname, dist);
            distCoeffs.push_back(dist);
        }

        // 外部パラメータ
        rvecs.clear();
        tvecs.clear();
        for(int i = 0; i < depth.size(); ++i)
        {
            Mat rvec;
            Mat tvec;
            QString fname = QFileDialog::getOpenFileName(this, "Open Init Calib Image", QDir::homePath());

            Mat initCalib = imread(fname.toStdString());
            util->getExtrinsicMatrix(initCalib, Size(7, 10), objectPoint, cameraMatrixes.at(i), distCoeffs.at(i), rvec, tvec);

            rvecs.push_back(rvec);
            tvecs.push_back(tvec);

            qDebug() << "rvecs:";
            for(int j = 0; j < rvec.rows; ++j)
            {
                for(int k = 0; k < rvec.cols; ++k)
                {
                    qDebug() << rvec.at<double>(j, k);
                }
            }
            qDebug() << "tvecs:";
            for(int j = 0; j < tvec.rows; ++j)
            {
                for(int k = 0; k < tvec.cols; ++k)
                {
                    qDebug() << tvec.at<double>(j, k);
                }
            }
        }
    }

    // 1 GLView, Kiect Thread run
    if(ui->checkBox_Synthesize->isChecked() && kinectTh != NULL)
    {
        // 一つ目のデプスだけ90度回して結合
        vector<Point3d> vecsA = util->getVecsFromMat(depth.at(1), cameraMatrixes.at(1));
        util->rotatePoint3dVecsAroundY(-M_PI / 2.0, vecsA);
        util->moveVecs(Point3d(x, y, z), vecsA);
        vector<Point3d> vecsB = util->getVecsFromMat(depth.at(0), cameraMatrixes.at(0));
        vector<Point3d> vecsAB = util->synthVecs(vecsA, vecsB);

        vector<Point3d> colorA = util->getVecsFromMat(color.at(1), Mat());
        vector<Point3d> colorB = util->getVecsFromMat(color.at(0), Mat());
        vector<Point3d> colorAB = util->synthVecs(colorA, colorB);

        glView.at(0)->setPoints(vecsAB);
        glView.at(0)->setColors(colorAB);
        glView.at(0)->updateGL();
    }

    // IR mode
    if(ui->checkBox_IrOrImage->isChecked())
    {
        /*
        imshow("Kinect 0 IR", color.at(0));
        setWindowProperty("Kinect 0 IR", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_FREERATIO);

        imshow("Kinect 1 IR", color.at(1));
        setWindowProperty("Kinect 1 IR", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_FREERATIO);
        */
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
        objectPoint.clear();
        for(int i = 0; i < patternRow; ++i)
        {
            for(int j = 0; j < patternCol; ++j)
            {
                Point3f point;
                point.x = chessSize * (float)j;
                point.y = chessSize * (float)i;
                point.z = 0.0;

                objectPoint.push_back(point);
            }
        }

        for(int i = 0; i < fileNames.size(); ++i)
        {
            objectPoints.push_back(objectPoint);
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

        util->saveCSVFromMat("ir0-cameraMatrix.csv", cameraMatrix);
        util->saveCSVFromMat("ir0-distCoeffs.csv", distCoeffs);

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
