#ifndef SERIALREADER_H
#define SERIALREADER_H

#include "serialib/serialib.h"
#include <string>
#include <iostream>

class SerialReader {
public:
    SerialReader(const std::string& port, int baud_rate = 115200);

    bool openPort(bool verbose = true);

    bool waitForSequence(const uint8_t* sequence, size_t size, int timeout_ms);

    bool initialize();

    int read(float* data, size_t N);

    void closePort(bool verbose = true);

private:
    serialib serial_;
    std::string port_;
    int baud_rate_;
    bool is_open_;
};

#endif // SERIALREADER_H
