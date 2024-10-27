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

