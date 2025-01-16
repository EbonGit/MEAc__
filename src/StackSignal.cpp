#include "StackSignal.h"

ThresholdingResult thresholding_algo_update(float newPoint, int lag, double threshold, double influence, ThresholdingResult& result, RollingStats& stats, std::deque<float>& window) {
    // Track the current window of points (of size `lag`)
    if (window.size() >= lag) {
        float oldPoint = window.front();
        window.pop_front();
        stats.update(newPoint, lag, oldPoint); // Update rolling stats by adding new point, removing old one

    } else {
        stats.update(newPoint, lag);  // Just add new point if window not full
    }
    window.push_back(newPoint);  // Add the new point to the rolling window

    // Compute current mean and standard deviation based on rolling stats
    double currentMean = stats.mean();
    double currentStdDev = stats.stdDev();

    // Thresholding for the new point
    float signal = 0.0;
    if (window.size() >= lag && std::abs(newPoint - currentMean) > threshold * currentStdDev) {
        signal = (newPoint > currentMean) ? 2000.0 : -2000.0;
    }

    // Update result object
    if (result.signals.size() >= sizeSpikesBuffer) {
        int idx = result.signals.size() - sizeSpikesBuffer;
        if (result.signals[idx] != 0.0) {
            result.spikes--;
        }
    }
    if (result.signals.size() >= signalsBufferSize) {
        result.signals.erase(result.signals.begin());
        result.avgFilter.erase(result.avgFilter.begin());
        result.stdFilter.erase(result.stdFilter.begin());
    }

    if (signal != 0.0) {
        result.spikes++;
    }
    result.signals.push_back(signal);
    result.avgFilter.push_back(currentMean);
    result.stdFilter.push_back(currentStdDev);

    return result;
}

void StackSignal::push_threshold(const float &value) {
    thresholding_algo_update(value, LAG, THRESHOLD, INFLUENCE, this->threshold, stats, window);
}