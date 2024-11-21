#ifndef MEAC___WINDOWMODE_H
#define MEAC___WINDOWMODE_H

#include <vector>
#include "string"
#include <set>
#include "StackSignal.h"

enum class processMode {
    NORMAL,
    XOR,
    OR,
    AND,
};

class windowMode {
private:
    processMode p_mode;
    int last_min;
    int last_max;

    std::vector<std::string> processNames = {
            "NORMAL",
            "XOR",
            "OR",
            "AND",
    };

public:

    windowMode() : p_mode(processMode::NORMAL), last_min(0), last_max(0) {};

    void nextMode() { p_mode = static_cast<processMode>((static_cast<int>(p_mode) + 1) % 4); };
    std::vector<std::vector<float>> process(std::vector<StackSignal>& signals, std::set<int> selectedSignalsIndex, int last_n);
    int get_last_min() { return last_min; };
    int get_last_max() { return last_max; };
    std::string getModeName() { return processNames[static_cast<int>(p_mode)]; };

};


#endif //MEAC___WINDOWMODE_H
