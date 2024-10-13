#ifndef MEAC___VUE_H
#define MEAC___VUE_H
#include "Data.h"
#include "config.h"

class Vue: public virtual Data {
protected:
    int width;
    int height;
    int numPoints;
    int numImages;
    int signalsBufferSize;

public :
    Vue(int width, int height, int numPoints, int numImages, int signalsBufferSize);

    void generatePoints();

    cv::Mat plotGrid();

    std::vector<cv::Mat> plotSelectedSignal(int scale = 1);
    void setSelectedWindow(int index);
    void addWindow();

    cv::Mat plotHeatmap(int caseSize = 16);

    void selectSignal(int index);

    void selectThresholdedSignal();
    int lag = 25;
    void setLag(int l) { this->lag = l; };
    int threshold = 4;
    void setThreshold(int t) { this->threshold = t; };

    std::vector<std::set<int>> getSelectedSignalIndex();
};


#endif //MEAC___VUE_H
