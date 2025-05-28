#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_imageButton_clicked();

    void on_grayButton_clicked();

    void on_blurButton_clicked();

    void on_edgeButton_clicked();

    void on_resetButton_clicked();

    void on_brightnessSlider_valueChanged(int value);

    void on_hueSlider_valueChanged(int value);

    void on_saturationSlider_valueChanged(int value);

    void updateImage();

    void showImage(const cv::Mat &img);

    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    cv::Mat originalImage;
    cv::Mat currentImage;

};
#endif // MAINWINDOW_H
