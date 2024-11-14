#ifndef MEAC___CONFIG_H
#define MEAC___CONFIG_H

struct Color {
    int r;
    int g;
    int b;
};

enum class Mode {
    GENERATE,
    TCP
};

const Mode mode = Mode::TCP;

const bool saving = false;

const bool python = false;

const bool init_zero = true;

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 32;
const int numImages = 60;
const int signalsBufferSize = 1024;

const int minValueHeatmap = -100;
const int maxValueHeatmap = 100;

const std::string IP = "127.0.0.1";
const int PORT = 8080;
const int P = 4;


#endif //MEAC___CONFIG_H
