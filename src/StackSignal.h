#ifndef MEAC___STACKSIGNAL_H
#define MEAC___STACKSIGNAL_H

#include "stack.h"
#include <numeric>
#include "config.h"
#include <algorithm>
#include <deque>

struct ThresholdingResult {
    std::vector<float> signals;
    std::vector<float> avgFilter;
    std::vector<float> stdFilter;

    int spikes = 0;
};

struct RollingStats {
    double sum = 0.0;
    double sumOfSquares = 0.0;
    int count = 0;

    void update(double newPoint, int LAG, double oldPoint = 0.0) {
        sum += newPoint - oldPoint;
        sumOfSquares += newPoint * newPoint - oldPoint * oldPoint;
        count = std::min(count + 1, LAG);
    }

    double mean() const {
        return count > 0 ? sum / count : 0.0;
    }

    double stdDev() const {
        if (count > 1) {
            double meanSquare = sumOfSquares / count;
            double squareMean = (sum / count) * (sum / count);
            return std::sqrt((meanSquare - squareMean) * count / (count - 1));

        }
        return 0.0;
    }
};

class StackSignal: public Stack<float> {
private:
    ThresholdingResult threshold;
    std::deque<float> window;
    RollingStats stats;

    void push_threshold(const float& value);

public:
    StackSignal(int max_size = -1) : Stack<float>(max_size) {}

    void push(const float& item) {
        Stack<float>::push(item);
        push_threshold(item);
    }

    ThresholdingResult get_threshold() {
        return threshold;
    }

    int LAG = 512;
    double THRESHOLD = 1.5;
    double INFLUENCE = 1;

    void addLag(int l) { LAG = LAG + l; }

    void addThreshold(double t) { THRESHOLD = THRESHOLD + t; }

};


#endif //MEAC___STACKSIGNAL_H
