#ifndef MEAC___VUE_H
#define MEAC___VUE_H
#include "Data.h"
#include "config.h"
#include <fstream>

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

    std::vector<std::set<int>> getSelectedSignalIndex();

    void addLag(int delta);
    void addThreshold(double delta);

    void nextWindowMode() {
        windowsMode[selectedWindow].nextMode();
    }

    int lag = 512;
    double threshold = 1.5;
};


#endif //MEAC___VUE_H
