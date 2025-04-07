#ifndef MEAC___NETWORK_H
#define MEAC___NETWORK_H
#include <string>
#include "tcp.h"
#include "PythonAPI.h"
#include "SerialReader.h"

class Network: public virtual tcp, public PythonAPI {

private:
    SignalType signalType;

    std::vector<std::vector<float>> binaryData;

    float signalProcessing(float x);

    int Core0;
    int Core1;

public:
    Network(SignalType signalType);
    void generateNextPoint();
    void setSignalType(SignalType signalType);

    void launchGenerate();
    void launchTCP();
    void launchSerial();

    int receive();

    int getCore0(){ return Core0; }
    int getCore1(){ return Core1;}

    int sampleRate = 0;
};

double convertBigEndianToDouble(const char* buffer);


#endif //MEAC___NETWORK_H
