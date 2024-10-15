#include "Vue.h"
#include "VecFloat.h"

Vue::Vue(int width, int height, int numPoints, int numImages, int signalsBufferSize) {
    this->width = width;
    this->height = height;
    this->numPoints = numPoints;
    this->numImages = numImages;
    this->signalsBufferSize = signalsBufferSize;

}


void Vue::generatePoints() {
    std::lock_guard<std::mutex> lock(signalsMutex);
    std::lock_guard<std::mutex> lock2(lastSignalMutex);

    for (int i = 0; i < numImages; i++) {
        Stack<float> signal = ::generatePoints(numPoints, width);
        signals.push_back(signal);
        lastSignal.push_back(signal.peek());
    }
}

cv::Mat Vue::plotGrid() {
    std::lock_guard<std::mutex> lock(signalsMutex);
    std::vector<cv::Mat> images;

    uint8_t blue = 255;
    uint8_t green = 0;

    for (uint8_t i = 0; i < numImages; i++) {
        std::vector<float> clipped_signal = signals[i].get_last_n(numPoints);
        std::vector<Point> points = ::floatsToPoints(clipped_signal, width, height);
        // check in all set if i in it (not optimal)
        if (!selectedSignalsIndexes.empty() and selectedSignalsIndexes[selectedWindow].contains(i)) {
            images.push_back(::plotPoints(points, width, height, false, {255 - blue, green, blue}, {25, 25, 25}));
        } else {
            images.push_back(::plotPoints(points, width, height, false, {255 - blue, green, blue}));
        }
        blue -= std::floor(255.0f / numImages);
    }

    return ::concatenateImages(images);
}

std::vector<cv::Mat> Vue::plotSelectedSignal(int scale) {
    std::lock_guard<std::mutex> lock(signalsMutex);

    std::vector<cv::Mat> images;

    int nearSqrt = std::ceil(std::sqrt(numImages));

    int selectedWidth = width * nearSqrt * scale;
    int selectedHeight = height * 2 * scale;

    for (int k = 0; k < selectedSignalsIndexes.size(); k++) {

        cv::Mat selectedImage(selectedHeight, selectedWidth, CV_8UC3, cv::Scalar(bg.r, bg.g, bg.b));

        int minValue = INFINITY;//(int)std::min_element(clipped_signal.begin(), clipped_signal.end())[0];
        int maxValue = -INFINITY;//(int)std::max_element(clipped_signal.begin(), clipped_signal.end())[0];

        // Float to points
        std::vector<std::vector<float>> selectedSignals;
        for (int selectSignalIndex: selectedSignalsIndexes[k]) {
            std::vector<float> clipped_signal = signals[selectSignalIndex].get_last_n(numPoints * nearSqrt * scale);
            selectedSignals.push_back(clipped_signal);

            int minLocal = std::min(minValue, (int) *std::min_element(clipped_signal.begin(), clipped_signal.end()));
            if (minLocal < minValue) {
                minValue = minLocal;
            }
            int maxLocal = std::max(maxValue, (int) *std::max_element(clipped_signal.begin(), clipped_signal.end()));
            if (maxLocal > maxValue) {
                maxValue = maxLocal;
            }

        }
        std::vector<std::vector<Point>> vecPoints = ::vecFloatsToVecPoints(selectedSignals, selectedWidth, selectedHeight);

        int idx = 0;
        // Draw multiple signals
        for (int selectSignalIndex: selectedSignalsIndexes[k]) {

            int blue = 255 - (int) (std::floor(255.0f / (float) numImages) * (float) selectSignalIndex);

            selectedImage = ::plotPoints(vecPoints[idx], selectedWidth, selectedHeight, false,
                                         {255 - blue, 0, blue}, bg,true, selectedImage);

            // Draw the thresholding line
            if (thresholdedSignalsIndexes.contains(k)){
                ThresholdingResult result = ::thresholding_algo(selectedSignals[idx], this->lag, this->threshold, 0.0);

                std::vector<Point> avgPoints = ::floatsToPoints(result.avgFilter, selectedWidth, selectedHeight, maxValue, minValue);

                VecFloat avgMinusStd = VecFloat(result.avgFilter) - VecFloat(result.stdFilter);

                VecFloat avgPlusStd = VecFloat(result.avgFilter) + VecFloat(result.stdFilter);

                // AVG
                selectedImage = ::plotPoints(avgPoints, selectedWidth, selectedHeight,false,
                                             {255, 255, 255}, bg, true, selectedImage);

                // AVG +/- STD
                selectedImage = ::plotPoints(::floatsToPoints(avgMinusStd.data, selectedWidth, selectedHeight, maxValue, minValue),
                                             selectedWidth, selectedHeight, false,
                                             {0, 255, 0}, bg, true, selectedImage);

                selectedImage = ::plotPoints(::floatsToPoints(avgPlusStd.data, selectedWidth, selectedHeight, maxValue, minValue),
                                             selectedWidth, selectedHeight, false,
                                             {0, 255, 0}, bg, true, selectedImage);

                std::vector<Point> thresholdedPoints = ::floatsToPoints(result.signals, selectedWidth, selectedHeight, maxValue, minValue);

                selectedImage = ::plotPoints(thresholdedPoints, selectedWidth, selectedHeight, false,
                                             {255, 0, 0}, bg, true, selectedImage);

                ::drawLabel(selectedImage, "Lag = " + std::to_string(this->lag), Point(5, 30), {0, 255, 0}, 0.3);
                ::drawLabel(selectedImage, "Threshold = " + std::to_string(this->threshold), Point(5, 40), {0, 255, 0}, 0.3);
            }

            idx++;

        }

        Color labelColor = {255, 0, 0};
        double fontScale = 0.3;
        // Add extremum values on the Y-axis
        std::string minLabel = std::to_string(minValue);
        std::string maxLabel = std::to_string(maxValue);

        // Draw the labels
        drawLabel(selectedImage, minLabel, Point(int(selectedWidth / 2 + 5), selectedHeight - 5), labelColor,
                  fontScale);
        drawLabel(selectedImage, maxLabel, Point(int(selectedWidth / 2 + 5), 10), labelColor, fontScale);

        std::string signalsIndexesString;
        for (int selectedSignalIndex: selectedSignalsIndexes[k]) {
            signalsIndexesString += std::to_string(selectedSignalIndex) + " ";
        }

        drawLabel(selectedImage, "Signal " + signalsIndexesString, Point(5, 10), labelColor, fontScale);

        cv::copyMakeBorder(selectedImage, selectedImage, 5, 5, 0, 0, cv::BORDER_CONSTANT, cv::Scalar(bg.r, bg.g, bg.b));

        images.push_back(selectedImage);
    }
    return images;
}

void Vue::setSelectedWindow(int index) {
    selectedWindow = index;
}

void Vue::selectThresholdedSignal() {
    if (thresholdedSignalsIndexes.contains(selectedWindow)) {
        thresholdedSignalsIndexes.erase(selectedWindow);
    } else {
        thresholdedSignalsIndexes.insert(selectedWindow);
    }
}

void Vue::addWindow(){
    selectedSignalsIndexes.push_back({0});
    // print all selectedSignalsIndexes
    for (int i = 0; i < selectedSignalsIndexes.size(); i++) {
        std::cout << "Window " << i << ": ";
        for (int j: selectedSignalsIndexes[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
}

cv::Mat Vue::plotHeatmap(int caseSize) {
    std::lock_guard<std::mutex> lock(lastSignalMutex);
    return ::generateHeatmap(lastSignal, caseSize, -4096, 4096);
}

void Vue::selectSignal(int index) {
    if (selectedSignalsIndexes.empty()) {
        return;
    } else
    if (selectedSignalsIndexes[selectedWindow].contains(index)) {
        selectedSignalsIndexes[selectedWindow].erase(index);
    } else {
        selectedSignalsIndexes[selectedWindow].insert(index);
    }
}

std::vector<std::set<int>> Vue::getSelectedSignalIndex() {
    return selectedSignalsIndexes;
}