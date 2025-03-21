#include "SerialReader.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
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

bool SerialReader::initialize() {
    const uint8_t ackSignal[3] = {0xAA, 0xBB, 0xCC};

    // Envoi du premier ACK
    serial_.writeBytes(ackSignal, 3);
    serial_.flushReceiver();
    std::cout << "[INFO] Premier ACK envoye, ESP32 peut commencer." << std::endl;

    return true;
}


int SerialReader::read(float* data, size_t N) {
    const uint8_t startSequence[3] = {0xFF, 0xAA, 0x55};
    const uint8_t endSequence[3] = {0xEE, 0xBB, 0x66};
    const uint8_t resetSignal[3] = {0xCC, 0xDD, 0xEE};
    const uint8_t ackSignal[3] = {0xAA, 0xBB, 0xCC};

    uint8_t receivedBytes[3] = {0};

    // Attente de la séquence de départ ou du signal de reset
    size_t matchedBytes = 0;
    while (matchedBytes < 3) {
        if (serial_.readChar(reinterpret_cast<char*>(&receivedBytes[matchedBytes]), 1000) == 1) {
            if (receivedBytes[matchedBytes] == startSequence[matchedBytes]) {
                matchedBytes++;
            } else if (receivedBytes[matchedBytes] == resetSignal[matchedBytes]) {
                matchedBytes++;
                if (matchedBytes == 3) {
                    std::cout << "[INFO] Recu signal de reset. Fermeture et reouverture de la connexion." << std::endl;

                    closePort(false);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    if (!openPort(false)) {
                        std::cerr << "[ERROR] Impossible de rouvrir le port serie !" << std::endl;
                        return 0;
                    }

                    serial_.flushReceiver();
                    serial_.writeBytes(ackSignal, 3);
                    return 2;    // Réinitialisation réussie
                }
            } else {
                //std::cout << "[ERROR] " << receivedBytes[matchedBytes] << " != " << startSequence[matchedBytes] << std::endl;
                matchedBytes = 0; // Reset si erreur
            }
        } else {
            std::cerr << "[ERROR] Timeout en attente de la sequence de depart !" << std::endl;
            return 0;
        }
    }

        // Lecture des données
    size_t bytesRead = 0;
    while (bytesRead < N * sizeof(float)) {
        int chunk = serial_.readBytes(reinterpret_cast<char*>(data) + bytesRead, (N * sizeof(float)) - bytesRead, 1000);
        if (chunk > 0) {
            bytesRead += chunk;
        } else {
            std::cerr << "[ERROR] Echec de la lecture apres " << bytesRead << " bytes." << std::endl;
            return 0;
        }
    }

    // Vérification de la séquence de fin
    if (serial_.readBytes(reinterpret_cast<char*>(receivedBytes), 3, 1000) != 3) {
        std::cerr << "[ERROR] Timeout ou erreur lors de la lecture de la sequence de fin !" << std::endl;
        return 0;
    }

    if (memcmp(receivedBytes, endSequence, 3) != 0) {
        std::cerr << "[ERROR] Sequence de fin invalide !" << std::endl;
        return 0;
    }

    return 1;
}