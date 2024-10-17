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

const Mode mode = Mode::GENERATE;

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 64;
const int numImages = 4;
const int signalsBufferSize = 512;

const std::string ip = "127.0.0.1";
const int port = 8080;


#endif //MEAC___CONFIG_H
