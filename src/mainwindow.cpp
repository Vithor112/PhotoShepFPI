#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QRegExp rx("^[1-9][0-9]*$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit->setValidator(validator);
    
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openImgBtn_clicked()
{
    QString fileName =  QFileDialog::getOpenFileName(this, "Open A File", QString(), QString("Image Files (*.png *.jpg, *.jpeg)"));
    if (fileName.isNull()){
        return; 
    }
    QPixmap pixmap(fileName);
    ui->label->setPixmap(pixmap);
    _original = std::make_unique<Image>(fileName.toStdString()); 
    _changed = std::make_unique<Image>(*_original); 
    ui->label_2->setPixmap(pixmap);
    ui->quantBtn->setEnabled(true);
    ui->grayBtn->setEnabled(true);
    ui->mirHorBtn->setEnabled(true);
    ui->mirVerBtn->setEnabled(true);
    ui->SaveImgBtn->setEnabled(true);
    ui->resetImageBtn->setEnabled(true);
}



void MainWindow::on_mirHorBtn_clicked()
{
    _changed->GetMirrorImageHorizontal(); 
    _changed->SavePNGImg("tmp.png");
    QPixmap pixmap("tmp.png");
    ui->label_2->setPixmap(pixmap);
}



void MainWindow::on_grayBtn_clicked()
{
    _changed->GetGrayImage(3); 
    _changed->SavePNGImg("tmp.png");
    QPixmap pixmap("tmp.png");
    ui->label_2->setPixmap(pixmap);

}

void MainWindow::on_quantBtn_clicked()
{
    int tones = stoi(ui->lineEdit->text().toStdString()); 
    _changed->GetGrayImage(1);
    _changed->quantizeImage(tones); 
    _changed->SavePNGImg("tmp.png");
    QPixmap pixmap("tmp.png");
    ui->label_2->setPixmap(pixmap);
}



void MainWindow::on_mirVerBtn_clicked()
{
    _changed->GetMirrorImageVertical(); 
    _changed->SavePNGImg("tmp.png");
    QPixmap pixmap("tmp.png");
    ui->label_2->setPixmap(pixmap);
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}


void MainWindow::on_SaveImgBtn_clicked()
{
    QString fileName =  QFileDialog::getSaveFileName(this, "Choose a Directory",  QString(), QString("Image Files (*.png *.jpg, *.jpeg)")); 
    if (fileName.isNull()){
        return; 
    }

    if (hasEnding(fileName.toStdString(), ".jpeg") || hasEnding(fileName.toStdString(), ".jpg") )
        _changed->SaveJPEGImg(fileName.toStdString(), 90);
    if (hasEnding(fileName.toStdString(), ".png"))
        _changed->SavePNGImg(fileName.toStdString());
}



void MainWindow::on_resetImageBtn_clicked()
{
    _changed =  std::make_unique<Image>(*_original); 
    _changed->SavePNGImg("tmp.png");
    QPixmap pixmap("tmp.png");
    ui->label_2->setPixmap(pixmap);
}
