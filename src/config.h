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

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 32;
const int numImages = 64;
const int signalsBufferSize = 512;

const std::string IP = "127.0.0.1";
const int PORT = 8080;
const int P = 4;


#endif //MEAC___CONFIG_H
