#include "Data.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdexcept>

Stack<float> Data::getSignals(int index) {
    std::lock_guard<std::mutex> lock(signalsMutex);
    return signals[index];
}

std::vector<float> read_binary_file(const std::string& file_path) {
    std::vector<float> data;
    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file!");
    }

    while (true) {
        float unpacked_data;
        char packed_data[sizeof(float)];

        // Read 4 bytes (size of float)
        file.read(packed_data, sizeof(float));

        if (!file) {
            break;
        }

        // Unpack the 4 bytes to a float value
        std::memcpy(&unpacked_data, packed_data, sizeof(float));
        data.push_back(unpacked_data);
    }

    return data;
}

std::vector<std::vector<float>> reshape_data(const std::vector<float>& data, size_t array_size) {
    std::vector<std::vector<float>> reshaped_data;

    for (int i = 0; i < array_size; i++) {
        reshaped_data.push_back({});
    }

    for (int i = 0; i < data.size(); i += array_size) {
        int idx = i % array_size;
        reshaped_data[idx].push_back(data[i]);
    }

    return reshaped_data;
}

void write_single_point(const std::string& file_path, const std::vector<float>& new_points) {
    std::ofstream file(file_path, std::ios::binary | std::ios::app);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file!");
    }

    // Write each new point to the binary file
    for (float point : new_points) {
        file.write(reinterpret_cast<const char*>(&point), sizeof(float));
    }
}

void Data::readPinout() {
    std::ifstream file("pinout.csv");
    std::vector<int> temp;
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
    }

    while (std::getline(file, line)) {
        try {
            temp.push_back(std::stoi(line));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data: " << line << " is not an integer." << std::endl;
        }
    }

    file.close();

    pinout.clear();

    float nearSqrt = std::ceil(std::sqrt(numImages));

    for (int i = 0; i < nearSqrt*nearSqrt; i++) {
        if(i < temp.size()) {
            pinout.push_back(temp[i]);
        } else {
            pinout.push_back(-1);
        }
    }
}

void Data::readZones() {
    std::ifstream file("zones.txt");
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        Zone entry;
        stream >> entry.name;

        int index;
        while (stream >> index) {
            entry.indexes.push_back(pinout[index]);
        }

        zones.push_back(entry);

        std::cout << entry.name << ": ";
        for (int i : entry.indexes) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}
