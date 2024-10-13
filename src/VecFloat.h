#ifndef MEAC___VECFLOAT_H
#define MEAC___VECFLOAT_H

#include <vector>
#include <iostream>
#include <stdexcept>

class VecFloat {
public:
    std::vector<float> data;

    VecFloat(const std::vector<float>& vec) : data(vec) {}

    VecFloat operator+(const VecFloat& other) const {
        if (this->data.size() != other.data.size()) {
            throw std::invalid_argument("Vectors must have the same size for addition.");
        }

        std::vector<float> result(this->data.size());
        for (size_t i = 0; i < this->data.size(); ++i) {
            result[i] = this->data[i] + other.data[i];
        }
        return VecFloat(result);
    }

    VecFloat operator-(const VecFloat& other) const {
        if (this->data.size() != other.data.size()) {
            throw std::invalid_argument("Vectors must have the same size for subtraction.");
        }

        std::vector<float> result(this->data.size());
        for (size_t i = 0; i < this->data.size(); ++i) {
            result[i] = this->data[i] - other.data[i];
        }
        return VecFloat(result);
    }

    void print() const {
        for (const auto& val : data) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
};

#endif //MEAC___VECFLOAT_H
