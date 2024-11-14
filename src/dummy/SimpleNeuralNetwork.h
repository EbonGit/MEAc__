#ifndef SIMPLENEURALNETWORK_H
#define SIMPLENEURALNETWORK_H

#include <vector>

class SimpleNeuralNetwork {
private:
    int inputSize;
    int hiddenSize;
    int outputSize;
    double learningRate;
    std::vector<std::vector<double>> weightsInputHidden; // Weights from input to hidden layer
    std::vector<std::vector<double>> weightsHiddenOutput; // Weights from hidden to output layer

    double sigmoid(double x);
    double sigmoidDerivative(double x);

public:
    SimpleNeuralNetwork(int inputSize, int hiddenSize, int outputSize, double learningRate = 0.1);
    std::vector<double> forward(const std::vector<double>& inputs);
    void train(const std::vector<double>& inputs, const std::vector<double>& targets);
};

#endif
