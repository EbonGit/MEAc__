#include "MEA.h"

MEA::MEA(MEA_Params params) : Vue(params.width, params.height, params.numPoints, params.numImages, params.signalsBufferSize),
                                Network(SignalType::RAW), tcp(IP, PORT) {

    generatePoints();
    readPinout();
    readZones();
}
