#ifndef MEAC___VUE_H
#define MEAC___VUE_H
#include "Data.h"

class Vue: public virtual Data {
protected:
    int width;
    int height;
    int numPoints;
    int numImages;
    int signalsBufferSize;

    std::vector<std::vector<float>> binaryData;

public :
    Vue(int width, int height, int numPoints, int numImages, int signalsBufferSize);

    void generatePoints();
    void generateNextPoint();

    cv::Mat plotGrid();

    std::vector<cv::Mat> plotSelectedSignal();
    void setSelectedWindow(int index);
    void addWindow();

    cv::Mat plotHeatmap(int caseSize = 16);

    void selectSignal(int index);
    std::vector<std::set<int>> getSelectedSignalIndex();

    int getNumImages();
};


#endif //MEAC___VUE_H
