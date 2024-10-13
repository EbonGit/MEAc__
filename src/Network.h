#ifndef MEAC___NETWORK_H
#define MEAC___NETWORK_H
#include <string>
#include "Data.h"
#include "config.h"


class Network: public virtual Data {

private:
    std::string IP;
    int port;
    SignalType signalType;

    std::vector<std::vector<float>> binaryData;

    float signalProcessing(float x);

public:
    Network(std::string IP, int port, SignalType signalType);
    void generateNextPoint();
    void setSignalType(SignalType signalType);

};


#endif //MEAC___NETWORK_H
