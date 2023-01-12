#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Image.hpp"
#include <memory> 

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

    void on_mirHorBtn_clicked(); 

    void on_mirVerBtn_clicked(); 

    void on_quantBtn_clicked(); 

    void on_SaveImgBtn_clicked(); 

    void on_resetImageBtn_clicked();

private:
    std::unique_ptr<Image> _original; 
    std::unique_ptr<Image> _changed; 
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
