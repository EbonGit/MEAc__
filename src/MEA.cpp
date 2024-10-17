#include "MEA.h"

MEA::MEA(MEA_Params params) : Vue(params.width, params.height, params.numPoints, params.numImages, params.signalsBufferSize),
                                PythonAPI(), Network(SignalType::RAW), tcp(IP, PORT) {
    generatePoints();
}
