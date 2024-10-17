#ifndef MEAC___NETWORK_H
#define MEAC___NETWORK_H
#include <string>
#include "tcp.h"

class Network: public virtual tcp {

private:
    SignalType signalType;

    std::vector<std::vector<float>> binaryData;

    float signalProcessing(float x);

public:
    Network(SignalType signalType);
    void generateNextPoint();
    void setSignalType(SignalType signalType);

    void launchGenerate();
    void launchTCP();

};


#endif //MEAC___NETWORK_H
