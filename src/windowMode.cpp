#include "windowMode.h"

std::vector<std::vector<float>> windowMode::process(std::vector<StackSignal> &signals, std::set<int> selectedSignalsIndex, int last_n) {
    std::vector<std::vector<float>> selectedSignals;
    last_min = INFINITY;
    last_max = -INFINITY;

    if (selectedSignalsIndex.empty()) {
        return selectedSignals;
    }

    switch (p_mode) {
        default: {
            for (int selectSignalIndex: selectedSignalsIndex) {
                std::vector<float> clipped_signal = signals[selectSignalIndex].get_last_n(last_n);
                selectedSignals.push_back(clipped_signal);

                int minLocal = std::min(last_min,
                                        (int) *std::min_element(clipped_signal.begin(), clipped_signal.end()));
                if (minLocal < last_min) {
                    last_min = minLocal;
                }
                int maxLocal = std::max(last_max,
                                        (int) *std::max_element(clipped_signal.begin(), clipped_signal.end()));
                if (maxLocal > last_max) {
                    last_max = maxLocal;
                }
            }
            break;
        }
        case processMode::OR: {
            std::vector<float> temp_signal;
            int temp_idx = 0;
            for (int selectSignalIndex: selectedSignalsIndex) {
                std::vector<float> clipped_signal = signals[selectSignalIndex].get_last_n(last_n);
                if (temp_idx == 0) {
                    for (float value: clipped_signal) {
                        if (value == 0.0) {
                            temp_signal.push_back(0.0);
                        } else {
                            temp_signal.push_back(100.0);
                        }
                    }

                } else {
                    for (int i = 0; i < temp_signal.size(); i++) {
                        if (clipped_signal[i] == 0.0 && temp_signal[i] == 0.0) {
                            temp_signal[i] = 0.0;
                        } else {
                            temp_signal[i] = 100.0;
                        }
                    }
                }
                temp_idx++;
            }
            selectedSignals.push_back(temp_signal);
            for (int i = 0; i < selectedSignalsIndex.size() - 1; i++) {
                std::vector<float> empty_signal = std::vector<float>(selectedSignals[0].size(), 0.0);
                selectedSignals.push_back(empty_signal);
            }
            last_min = 0;
            last_max = 100;
            break;
        }
        case processMode::XOR: {
                    std::vector<float> temp_signal;
                    int temp_idx = 0;
                    for (int selectSignalIndex: selectedSignalsIndex) {
                        std::vector<float> clipped_signal = signals[selectSignalIndex].get_last_n(last_n);
                        if (temp_idx == 0) {
                            for (float value: clipped_signal) {
                                if (value == 0.0) {
                                    temp_signal.push_back(0.0);
                                } else {
                                    temp_signal.push_back(100.0);
                                }
                            }

                        } else {
                            for (int i = 0; i < temp_signal.size(); i++) {
                                if (clipped_signal[i] != temp_signal[i]) {
                                    temp_signal[i] = 100.0;
                                } else {
                                    temp_signal[i] = 0.0;
                                }
                            }
                        }
                        temp_idx++;
                    }
                    selectedSignals.push_back(temp_signal);
                    for (int i = 0; i < selectedSignalsIndex.size() - 1; i++) {
                        std::vector<float> empty_signal = std::vector<float>(selectedSignals[0].size(), 0.0);
                        selectedSignals.push_back(empty_signal);
                    }
                    last_min = 0;
                    last_max = 100;
                    break;
            }
        case processMode::AND: {
            std::vector<float> temp_signal;
            int temp_idx = 0;
            for (int selectSignalIndex: selectedSignalsIndex) {
                std::vector<float> clipped_signal = signals[selectSignalIndex].get_last_n(last_n);
                if (temp_idx == 0) {
                    for (float value: clipped_signal) {
                        if (value == 0.0) {
                            temp_signal.push_back(0.0);
                        } else {
                            temp_signal.push_back(100.0);
                        }
                    }

                } else {
                    for (int i = 0; i < temp_signal.size(); i++) {
                        if (clipped_signal[i] != 0.0 && temp_signal[i] != 0.0) {
                            temp_signal[i] = 100.0;
                        } else {
                            temp_signal[i] = 0.0;
                        }
                    }
                }
                temp_idx++;
            }
            selectedSignals.push_back(temp_signal);
            for (int i = 0; i < selectedSignalsIndex.size() - 1; i++) {
                std::vector<float> empty_signal = std::vector<float>(selectedSignals[0].size(), 0.0);
                selectedSignals.push_back(empty_signal);
            }
            last_min = 0;
            last_max = 100;
            break;
        }
    }

    return selectedSignals;
}