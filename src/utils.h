#ifndef MEAC___UTILS_H
#define MEAC___UTILS_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdlib>
#include <string>
#include "config.h"

struct Point {
    Point(int i1, int i2);

    int x;
    int y;
};

struct MEA_Params {
    MEA_Params(int width, int height, int numPoints, int numImages, int signalsBufferSize);

    int width;
    int height;
    int numPoints;
    int numImages;
    int signalsBufferSize;
};

struct Window {
    Window(int width, int height, const std::string& name, int index = 0);

    int width;
    int height;
    std::string name;
    int index;
};

struct FPS {
    FPS();

    std::vector<float> frames;
    int limit = 20;

    void addFrame(float frame);
    int getFPS();
};

std::vector<Point> floatsToPoints(std::vector<float> floats, int width, int height, float max = NAN, float min = NAN);

std::vector<std::vector<Point>> vecFloatsToVecPoints(std::vector<std::vector<float>> floats, int width, int height);

std::vector<float> selectEnd(std::vector<float> floats, int nbPoints);

void setPixel(cv::Mat& image, int x, int y, const Color& color);

void drawLine(cv::Mat& image, Point p1, Point p2, Color color = {255, 255, 255}, int offsetX = 0, int offsetY = 0);

void drawAxes(cv::Mat& image, Color axesColor = {100, 100, 100}, int height = 200, int width = 200);

void drawLabel(cv::Mat& image, const std::string& label, Point position, Color color = {255, 255, 255}, double scale = 0.5, int thickness = 1, bool isCentered = false);

cv::Mat plotPoints(const std::vector<Point>& points, int width, int height, bool randomColor = true, Color color = {255, 255, 255}, Color bgColor = bg, bool axes = false, cv::Mat baseImage = cv::Mat());

float generatePoint(float lastPoint = -1);

float generateSinusoidalPoint(long t, float amplitude = 4096.f);

float generateSquareWavePoint(long t, float amplitude = 4096.f);

float generateSpikePoint(long t, float amplitude = 4096.f);

float addNoise(float value, float noiseLevel);

std::vector<float> generatePoints(int numPoints, int width);

cv::Mat concatenateImages(std::vector<cv::Mat> images);

cv::Vec3b valueToColor(float value, float min, float max);

cv::Mat generateHeatmap(const std::vector<float>& data, int caseSize, float minValue, float maxValue, bool showLabels = true);

int getImageIndex(int x, int y, int numImages, int width, int height);

struct ThresholdingResult {
    std::vector<float> signals;
    std::vector<float> avgFilter;
    std::vector<float> stdFilter;
};

ThresholdingResult thresholding_algo(const std::vector<float>& y, int lag, double threshold, double influence);

template<typename T>
T getIthElement(const std::set<T>& mySet, size_t index) {
    if (index >= mySet.size()) {
        throw std::out_of_range("Index is out of bounds");
    }

    auto it = mySet.begin();
    std::advance(it, index); // Move the iterator to the i-th position
    return *it;
}

#endif //MEAC___UTILS_H`
