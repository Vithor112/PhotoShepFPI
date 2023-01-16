#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Image.hpp"
#include <memory> 
#include <QChartView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openImgBtn_clicked();

    void on_grayBtn_clicked();

    void closeEvent(QCloseEvent *event);

    void on_mirHorBtn_clicked(); 

    void on_mirVerBtn_clicked(); 

    void on_quantBtn_clicked(); 

    void on_SaveImgBtn_clicked(); 

    void on_resetImageBtn_clicked();

    void on_adjustBrightnessBtn_clicked();

    void on_adjustContrastBtn_clicked();

    void on_negativeBtn_clicked();

    void on_rotateMinusBtn_clicked();

    void on_rotatePlusBtn_clicked();

    void on_histogramBtn_clicked();

    void on_equalizeHistogramBtn_clicked();

    void on_zoomInBtn_clicked();

    void on_zoomOutBtn_clicked();

    void on_matchHistogramBtn_clicked();

    void on_convolutionBtn_clicked();

    void on_displayImageBtn_clicked(); 

private:
    double kernel[KERNEL_SIZE][KERNEL_SIZE]; 
    std::unique_ptr<Image> _original; 
    std::unique_ptr<Image> _changed; 
    Ui::MainWindow *ui;
    std::unique_ptr<QtCharts::QChartView> _chartImageChanged; 
    std::unique_ptr<QtCharts::QChartView> _chartImageOriginal; 
    std::unique_ptr<QtCharts::QChartView> create_histogram(Image& img); 
    std::unique_ptr<QWidget> _wdg; 
};

#endif // MAINWINDOW_H
