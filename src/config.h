#ifndef MEAC___CONFIG_H
#define MEAC___CONFIG_H

struct Color {
    int r;
    int g;
    int b;
};

const Color bg = {0, 0, 0};

const int width = 64;
const int height = 64;
const int numPoints = 32;
const int numImages = 64;
const int signalsBufferSize = 512;


#endif //MEAC___CONFIG_H
