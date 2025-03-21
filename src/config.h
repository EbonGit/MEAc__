#ifndef MEAC___CONFIG_H
#define MEAC___CONFIG_H

struct Color {
    int r;
    int g;
    int b;
};

enum class Mode {
    GENERATE,
    TCP,
    MEA,
    SERIAL
};

const Mode mode = Mode::SERIAL;

const bool saving = false;

const bool python = (mode == Mode::MEA) ? true : false;

const bool init_zero = true;

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 32;
const int numImages = 60;
const int signalsBufferSize = 4096;
const int sizeSpikesBuffer = 50;

const int minValueHeatmap = (mode == Mode::MEA) ? -100 : -2000;
const int maxValueHeatmap = (mode == Mode::MEA) ? 100 : 2000;

//const std::string IP = "169.254.119.97";
const std::string IP = "127.0.0.1";
//const int PORT = 6340;
const int PORT = 8080;
const int P = 10;
const int N = 4;


#endif //MEAC___CONFIG_H
