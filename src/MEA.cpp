#include "MEA.h"

MEA::MEA(MEA_Params params) : Vue(params.width, params.height, params.numPoints, params.numImages, params.signalsBufferSize), PythonAPI(), Network("192.168.1.1", 1234, SignalType::RAW) {
    generatePoints();
}
