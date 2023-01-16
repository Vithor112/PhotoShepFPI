#include "Histogram.hpp"
#include <exception>
#include "Image.hpp"
#include <QChart>
#include <QBarSeries>
#include <math.h>
#include <QBarSet>
#include <QValueAxis> 

#define MAX_BAR_HEIGHT 100


Histogram::Histogram(Image& image){
    if (!image.IsMonochromatic()){
        std::__throw_invalid_argument("Image need to be monochromatic"); 
    }
    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        values[i] = 0; 
    }
    for (int i = 0 ; i < image.getHeight(); i++) {
        for (int j = 0; j < image.getWidth(); j++) {
            values[image.getGray(i*image.getWidth()+j)]++; 
        }
    }
    double factor = (double) (HISTOGRAM_SIZE-1)/(image.getImageDimensions()); 
    histCum[0] = values[0] * factor; 
    for (int i = 1; i < HISTOGRAM_SIZE; i++) {
        histCum[i] = histCum[i-1] + factor * values[i]; 
    }
}

int Histogram::getValue(int index){
    if (index < 0 || index >= HISTOGRAM_SIZE){
        std::__throw_out_of_range("Invalid range of Histogram: " + index);
    }
    return values[index]; 
}

int Histogram::getHistogramCumValue(int index){
    if (index < 0 || index >= HISTOGRAM_SIZE){
        std::__throw_out_of_range("Invalid range of Histogram: " + index);
    }
    return histCum[index]; 
}

std::unique_ptr<QtCharts::QChartView> Histogram::getQChart(QWidget *parent){
    QtCharts::QBarSet *set0 = new QtCharts::QBarSet("");
    double max = 0; 
    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        max = max < values[i] ? values[i] : max; 
    }

    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        *set0 << std::round(values[i] * MAX_BAR_HEIGHT / max);
    }

    QtCharts::QBarSeries *series = new QtCharts::QBarSeries();
    series->append(set0);

    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    axisX->setRange(0,255);
    series->attachAxis(axisX);

    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    axisY->setRange(0,MAX_BAR_HEIGHT);
    series->attachAxis(axisY);

    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);

    auto chartView = std::make_unique<QtCharts::QChartView>(chart); 
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView; 
}