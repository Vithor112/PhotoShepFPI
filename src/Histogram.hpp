#pragma once

#define HISTOGRAM_SIZE 256 
#include "Image.hpp"
#include <memory>
#include <QChartView>
#include <QWidget>

class Histogram {
    public: 
        Histogram(Image&);
        std::unique_ptr<QtCharts::QChartView> getQChart(QWidget *); 
        int getValue(int index);
        int getHistogramCumValue(int index);
    private:
        int values[HISTOGRAM_SIZE];
        int histCum[HISTOGRAM_SIZE]; 
};