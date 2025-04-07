#include "SerialReader.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

SerialReader::SerialReader(const std::string& port, int baud_rate)
    : port_(port), baud_rate_(baud_rate) {}

bool SerialReader::openPort(bool verbose) {
    if (serial_.openDevice(port_.c_str(), baud_rate_) != 1) {
        if (verbose) {
            std::cerr << "[ERROR] Impossible d'ouvrir le port serie !" << std::endl;
        }
        return false;
    }
    if (verbose){
        std::cout << "[INFO] Port serie ouvert avec succes : " << port_ << std::endl;
    }
    return true;
}

void SerialReader::closePort(bool verbose) {
    serial_.closeDevice();
    if (verbose) {
        std::cout << "[INFO] Port serie ferme proprement." << std::endl;
    }
}

template <typename T>
int SerialReader::read(T* data, size_t N) {
    const uint8_t startSequence[3] = {0xFF, 0xAA, 0x55};
    const uint8_t endSequence[3] = {0xEE, 0xBB, 0x66};

    uint8_t receivedBytes[3] = {0};
    size_t matchedBytes = 0;
    while (matchedBytes < 3) {
        if (serial_.readChar(reinterpret_cast<char*>(&receivedBytes[matchedBytes]), 1000) == 1) {
            if (receivedBytes[matchedBytes] == startSequence[matchedBytes]) {
                matchedBytes++;
            } else {
                //std::cout << "[INFO] Wait: " << receivedBytes[matchedBytes] << std::endl;
                matchedBytes = 0;
            }
        } else {
            std::cerr << "[ERROR] Timeout en attente de la sequence de depart !" << std::endl;
            return 0;
        }
    }

    size_t bytesRead = 0;
    while (bytesRead < N * sizeof(T)) {
        int chunk = serial_.readBytes(reinterpret_cast<char*>(data) + bytesRead, (N * sizeof(T)) - bytesRead, 1000);
        if (chunk > 0) {
            bytesRead += chunk;
        } else {
            std::cerr << "[ERROR] Echec de la lecture apres " << bytesRead << " bytes." << std::endl;
            return 0;
        }
    }

    if (serial_.readBytes(reinterpret_cast<char*>(receivedBytes), 3, 1000) != 3) {
        std::cerr << "[ERROR] Timeout ou erreur lors de la lecture de la sequence de fin !" << std::endl;
        return 0;
    }

    if (memcmp(receivedBytes, endSequence, 3) != 0) {
        return 2;
    }

    uint8_t receivedQsize = 0;
    if (serial_.readBytes(reinterpret_cast<char*>(&receivedQsize), 1, 1000) != 1) {
        std::cerr << "[ERROR] Timeout ou erreur lors de la lecture de la taille de la file !" << std::endl;
        return 0;
    }

    uint8_t receivedCPU0 = 0;
    if (serial_.readBytes(reinterpret_cast<char*>(&receivedCPU0), 1, 1000) != 1) {
        std::cerr << "[ERROR] Timeout ou erreur lors de la lecture de la charge CPU !" << std::endl;
        return 0;
    }

    Core0 = 100 - static_cast<int>(receivedCPU0);

    uint8_t receivedCPU1 = 0;
    if (serial_.readBytes(reinterpret_cast<char*>(&receivedCPU1), 1, 1000) != 1) {
        std::cerr << "[ERROR] Timeout ou erreur lors de la lecture de la charge CPU !" << std::endl;
        return 0;
    }

    Core1 = 100 - static_cast<int>(receivedCPU1);

    return 1;
}

template int SerialReader::read<float>(float*, size_t);
template int SerialReader::read<uint32_t>(uint32_t*, size_t);

