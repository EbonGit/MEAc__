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
    MEA
};

const Mode mode = Mode::MEA;

const bool saving = false;

const bool python = (mode == Mode::TCP) ? true : false;

const bool init_zero = true;

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 32;
const int numImages = 60;
const int signalsBufferSize = 1024;
const int sizeSpikesBuffer = 50;

const int minValueHeatmap = (mode == Mode::MEA) ? -100 : -2000;
const int maxValueHeatmap = (mode == Mode::MEA) ? 100 : 2000;

const int threshold_floor = 10;
const int threshold_ceil = 90;

const std::string IP = "127.0.0.1";
const int PORT = 8080;
const int P = 4;
const int N = 8;


#endif //MEAC___CONFIG_H
