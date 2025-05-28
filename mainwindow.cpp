#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <qmessageBOX>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->imageWidget->setFixedWidth(500);
    this->setWindowTitle("ImageHandler");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_imageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "選擇檔案", "", "Images (*.png *.jpg *.jpeg)");
    cv::Mat img = cv::imread(fileName.toStdString());
    if (!img.empty()) {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        originalImage = img;
        currentImage = originalImage.clone();
        showImage(currentImage);
    }
}


void MainWindow::on_grayButton_clicked() {
    if (originalImage.empty()) return;
    cv::Mat gray;
    cv::cvtColor(originalImage, gray, cv::COLOR_BGR2GRAY);
    currentImage = gray;
    showImage(currentImage);
}

void MainWindow::on_blurButton_clicked() {
    if (currentImage.empty()) return;
    cv::Mat blur;
    cv::GaussianBlur(currentImage, blur, cv::Size(15, 15), 0);
    currentImage = blur;
    showImage(currentImage);
}

void MainWindow::on_edgeButton_clicked() {
    if (originalImage.empty()) return;
    cv::Mat gray, edges;
    cv::cvtColor(originalImage, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 70, 200);
    currentImage = edges;
    showImage(currentImage);
}

void MainWindow::on_resetButton_clicked() {
    if (originalImage.empty()) return;
    currentImage = originalImage;
    showImage(currentImage);
}

void MainWindow::on_hueSlider_valueChanged(int value) {
    updateImage();
}
void MainWindow::on_saturationSlider_valueChanged(int value) {
    updateImage();
}
void MainWindow::on_brightnessSlider_valueChanged(int value) {
    updateImage();
}

void MainWindow::updateImage() {
    if (originalImage.empty()) return;

    cv::Mat hsv;
    cv::cvtColor(originalImage, hsv, cv::COLOR_BGR2HSV);

    // 2. 取得 slider 數值
    int hueShift = ui->hueSlider->value();
    int saturationShift = ui->saturationSlider->value();
    int brightnessShift = ui->brightnessSlider->value();

    // 3. 修改 HSV
    for (int y = 0; y < hsv.rows; y++) {
        for (int x = 0; x < hsv.cols; x++) {
            cv::Vec3b &pixel = hsv.at<cv::Vec3b>(y, x);
            // Hue: 0~179
            int h = pixel[0] + hueShift;
            if (h < 0) h += 180;
            if (h >= 180) h -= 180;
            pixel[0] = h;

            // Saturation: 0~255
            int s = pixel[1] + saturationShift;
            pixel[1] = cv::saturate_cast<uchar>(s);

            // Brightness (V): 0~255
            int v = pixel[2] + brightnessShift;
            pixel[2] = cv::saturate_cast<uchar>(v);
        }
    }

    // 4. 轉回 RGB
    cv::Mat result;
    cv::cvtColor(hsv, result, cv::COLOR_HSV2RGB);
    currentImage = result;

    showImage(currentImage);
}

void MainWindow::showImage(const cv::Mat &img) {
    if (img.channels() == 3) {
        // 假設傳入的三通道是RGB格式，不再轉色
        QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (img.channels() == 1) {
        QImage qimg(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
        ui->imageLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::on_saveButton_clicked()
{
    if (currentImage.empty()) {
        QMessageBox::warning(this, "警告", "目前沒有影像可以存檔！");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "儲存檔案", "", "Images (*.png *.jpg *.bmp)");
    if (!fileName.isEmpty()) {
        cv::Mat saveImg;
        if (currentImage.channels() == 3) {
            cv::cvtColor(currentImage, saveImg, cv::COLOR_RGB2BGR);
        } else {
            saveImg = currentImage.clone();
        }

        if (cv::imwrite(fileName.toStdString(), saveImg)) {
            QMessageBox::information(this, "完成", "檔案已成功儲存！");
        } else {
            QMessageBox::critical(this, "錯誤", "無法儲存檔案。");
        }
    }
}

