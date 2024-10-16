#ifndef MEAC___DATA_H
#define MEAC___DATA_H

#include "utils.h"
#include "stack.h"

enum class SignalType {
    RAW,
    ABS,
};

class Data {
protected :

    std::vector<Stack<float>> signals;
    std::mutex signalsMutex;
    std::vector<float> lastSignal;
    std::mutex lastSignalMutex;

    std::vector<std::set<int>> selectedSignalsIndexes = {};
    int selectedWindow = 0;
    std::set<int> thresholdedSignalsIndexes = {};

    long t = 0;

public :
    Stack<float> getSignals(int index);
};

std::vector<float> read_binary_file(const std::string& file_path);

std::vector<std::vector<float>> reshape_data(const std::vector<float>& data, size_t array_size);


#endif //MEAC___DATA_H
