#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include "Histogram.hpp"
#include <iostream>
#include <QDialog>
#include <QImage>
#include <QFormLayout>
#include <QList>
#include <QRegExpValidator> 
#include <QDialogButtonBox>
#include <QLineEdit> 
#include <QHBoxLayout>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    for (int i = 0; i < KERNEL_SIZE; i++){
        for (int j = 0; j < KERNEL_SIZE; j++) {
            kernel[i][j] = 1; 
        }
    }
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_displayImageBtn_clicked(){
    QLabel *imageLabel = new QLabel;
    auto img = _changed->GetQImage(); 
    imageLabel->setPixmap(QPixmap::fromImage(img));
    imageLabel->resize(_changed->getWidth(), _changed->getHeight()); 
    _wdg = std::make_unique<QWidget>();
    QScrollArea *scrollArea = new QScrollArea(_wdg.get());
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->resize(_changed->getWidth(), _changed->getHeight());
    _wdg->show(); 
    _wdg->resize(_changed->getWidth(), _changed->getHeight());
}

void MainWindow::closeEvent(QCloseEvent *event){
    if (_chartImageChanged) _chartImageChanged->close(); 
    if (_chartImageOriginal) _chartImageOriginal->close(); 
}

void MainWindow::on_openImgBtn_clicked()
{
    QString fileName =  QFileDialog::getOpenFileName(this, "Open A File", QString(), QString("Image Files (*.png *.jpg *.jpeg)"));
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
    ui->adjustBrightnessBtn->setEnabled(true);
    ui->adjustContrastBtn->setEnabled(true);
    ui->negativeBtn->setEnabled(true);
    ui->rotatePlusBtn->setEnabled(true);
    ui->rotateMinusBtn->setEnabled(true);
    ui->histogramBtn->setEnabled(true);
    ui->equalizeHistogramBtn->setEnabled(true);
    ui->zoomInBtn->setEnabled(true);
    ui->zoomOutBtn->setEnabled(true);
    ui->matchHistogramBtn->setEnabled(true);
    ui->convolutionBtn->setEnabled(true);
    ui->displayImageBtn->setEnabled(true); 
}


void MainWindow::on_adjustBrightnessBtn_clicked(){
    bool ok; 
    int bias = QInputDialog::getInt(this, "Insert Bias", "Insert a integer value between -255 and 255", 0, -255, 255, 1, &ok);
    if (!ok)  return; 
    _changed->AdjustBrightness(bias); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_adjustContrastBtn_clicked(){
    bool ok; 
    double gain = QInputDialog::getDouble(this, "Insert Gain", "Insert a decimal value between 0.0001 and 255", 1, 0.0001, 255, 4, &ok, Qt::WindowFlags()); 
    if (!ok)  return; 
    _changed->AdjustContrast(gain); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_matchHistogramBtn_clicked(){
    QString fileName =  QFileDialog::getOpenFileName(this, "Open a Image to be Matched", QString(), QString("Image Files (*.png *.jpg *.jpeg)"));
    if (fileName.isNull()){
        return; 
    }
    *_changed =  _original->GetGrayImage(1); 
    QPixmap pixmap(fileName);
    ui->label->setPixmap(pixmap);
    _original = std::make_unique<Image>(fileName.toStdString());
    *_original = _original->GetGrayImage(1);  
    _changed->MatchHistogram(*_original); 
    QImage img = _changed->GetQImage(); 
    pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
    if (_changed->IsMonochromatic()){
        _chartImageOriginal = create_histogram(*_original); 
        _chartImageChanged = create_histogram(*_changed); 
    }
}

void MainWindow::on_equalizeHistogramBtn_clicked(){
    _changed->EqualizeHistogram(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
    if (_changed->IsMonochromatic()){
        _chartImageOriginal = create_histogram(*_original); 
        _chartImageChanged = create_histogram(*_changed); 
    }
}

void MainWindow::on_rotatePlusBtn_clicked(){
    _changed->RotatePlus(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_negativeBtn_clicked(){
    _changed->Negative(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_rotateMinusBtn_clicked(){
    _changed->RotateMinus(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}



void MainWindow::on_mirHorBtn_clicked()
{
    _changed->GetMirrorImageHorizontal(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_zoomOutBtn_clicked(){
    QDialog dialog(this);
    QFormLayout form(&dialog);

    form.addRow(new QLabel("Insert Sx and Sy ( Need to be positive integers )"));

    QList<QLineEdit *> fields;
    for(int i = 0; i < 2; ++i) {
        QLineEdit *lineEdit = new QLineEdit("1", &dialog);
        QRegExp re("^[1-9][0-9]*$");
        QRegExpValidator *validator = new QRegExpValidator(re, this);
        lineEdit->setValidator(validator);
        QString label;
        if (i) label = QString("Value Sx");
        else label = QString("Value Sy");
        form.addRow(label, lineEdit);
        fields << lineEdit;
    }

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                            Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        bool empty = false;
        int s[2];
        int count = 0;
        foreach(QLineEdit * lineEdit, fields) {
            if (lineEdit->text().isEmpty()) {
                empty = true;
                break;
            }
            s[count] = std::stoi(lineEdit->text().toStdString()); 
            count++;
        }
        if (!empty) {
            _changed->ZoomOut(s[0], s[1]); 
            QImage img = _changed->GetQImage(); 
            QPixmap pixmap = QPixmap::fromImage(img); 
            ui->label_2->setPixmap(pixmap);
        }

    }
}

void MainWindow::on_convolutionBtn_clicked(){
    QDialog dialog(this);
    QFormLayout form(&dialog);

    form.addRow(new QLabel("Insert Kernel"));
    QList<QLineEdit *> fields;
    for(int i = 0; i < KERNEL_SIZE; ++i) {
        QHBoxLayout *layout = new QHBoxLayout();
        for (int j = 0; j < KERNEL_SIZE; j++) {
            QLineEdit *lineEdit = new QLineEdit(std::to_string(kernel[i][j]).c_str(), &dialog);
            QRegExp re("^-?[0-9]*(\\.[0-9]+)?$");
            QRegExpValidator *validator = new QRegExpValidator(re, this);
            lineEdit->setValidator(validator);
            layout->addWidget(lineEdit); 
            fields << lineEdit;
        }
        form.addRow(layout); 
    }

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                            Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        bool empty = false;
        int row = 0;
        int column = 0; 
        foreach(QLineEdit * lineEdit, fields) {
            if (lineEdit->text().isEmpty()) {
                empty = true;
                break;
            }
            kernel[row][column] = std::stod(lineEdit->text().toStdString()); 
            column++;
            if (column >= KERNEL_SIZE){
                column = 0; 
                row++; 
            }
        }
        if (!empty) {
            Kernel k(kernel); 
            if (!k.isGaussian()) _changed->GetGrayImage(1); 
            _changed->Convolution(k); 
            QImage img = _changed->GetQImage(); 
            QPixmap pixmap = QPixmap::fromImage(img); 
            ui->label_2->setPixmap(pixmap);
        }

    }
}

void MainWindow::on_zoomInBtn_clicked(){
    _changed->ZoomIn(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}



void MainWindow::on_grayBtn_clicked()
{
    _changed->GetGrayImage(1); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);

}

void MainWindow::on_quantBtn_clicked()
{
    bool ok; 
    int tones = QInputDialog::getInt(this, "Insert Tones", "Insert a integer value between 1 and 256", 256, 1, 256, 1, &ok); 
    if (!ok)  return; 
    _changed->GetGrayImage(1);
    _changed->quantizeImage(tones); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

void MainWindow::on_mirVerBtn_clicked()
{
    
    _changed->GetMirrorImageVertical(); 
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::unique_ptr<QtCharts::QChartView> MainWindow::create_histogram(Image& img){
    Image copy = img;
    copy.GetGrayImage(1);
    Histogram histogram(copy);
    auto chartView = histogram.getQChart(this); 
    chartView->resize(1200,700); 
    chartView->raise();
    chartView->show(); 
    return chartView; 
}

void MainWindow::on_histogramBtn_clicked(){
    _chartImageChanged = create_histogram(*_changed); 
}

void MainWindow::on_SaveImgBtn_clicked()
{
    QString fileName =  QFileDialog::getSaveFileName(this, "Choose a Directory",  QString(), QString("Image Files (*.png *.jpg *.jpeg)")); 
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
    QImage img = _changed->GetQImage(); 
    QPixmap pixmap = QPixmap::fromImage(img); 
    ui->label_2->setPixmap(pixmap);
}
