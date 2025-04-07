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

    template <typename T>
    int read(T* data, size_t N);

    void closePort(bool verbose = true);

    int getCore0(){ return Core0; }
    int getCore1(){ return Core1;}

private:
    serialib serial_;
    std::string port_;
    int baud_rate_;
    bool is_open_;
    int Core0;
    int Core1;
};

#endif // SERIALREADER_H
