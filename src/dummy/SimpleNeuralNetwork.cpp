#include "SimpleNeuralNetwork.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

// Sigmoid activation function
double SimpleNeuralNetwork::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

// Derivative of sigmoid
double SimpleNeuralNetwork::sigmoidDerivative(double x) {
    return x * (1.0 - x);
}

// Constructor - initialize the network
SimpleNeuralNetwork::SimpleNeuralNetwork(int inputSize, int hiddenSize, int outputSize, double learningRate)
        : inputSize(inputSize), hiddenSize(hiddenSize), outputSize(outputSize), learningRate(learningRate) {
    weightsInputHidden.resize(inputSize, std::vector<double>(hiddenSize));
    weightsHiddenOutput.resize(hiddenSize, std::vector<double>(outputSize));

    // Randomly initialize weights
    std::srand(std::time(0));
    for (int i = 0; i < inputSize; ++i)
        for (int j = 0; j < hiddenSize; ++j)
            weightsInputHidden[i][j] = ((double) std::rand() / RAND_MAX) * 2 - 1; // Between -1 and 1

    for (int i = 0; i < hiddenSize; ++i)
        for (int j = 0; j < outputSize; ++j)
            weightsHiddenOutput[i][j] = ((double) std::rand() / RAND_MAX) * 2 - 1;
}

// Forward pass
std::vector<double> SimpleNeuralNetwork::forward(const std::vector<double>& inputs) {
    std::vector<double> hidden(hiddenSize, 0.0);
    std::vector<double> outputs(outputSize, 0.0);

    // Input to hidden layer
    for (int j = 0; j < hiddenSize; ++j)
        for (int i = 0; i < inputSize; ++i)
            hidden[j] += inputs[i] * weightsInputHidden[i][j];
    for (double &h : hidden) h = sigmoid(h);

    // Hidden to output layer
    for (int j = 0; j < outputSize; ++j)
        for (int i = 0; i < hiddenSize; ++i)
            outputs[j] += hidden[i] * weightsHiddenOutput[i][j];
    for (double &o : outputs) o = sigmoid(o);

    return outputs;
}

// Training with backpropagation
void SimpleNeuralNetwork::train(const std::vector<double>& inputs, const std::vector<double>& targets) {
    // Forward pass
    std::vector<double> hidden(hiddenSize, 0.0);
    std::vector<double> outputs(outputSize, 0.0);

    for (int j = 0; j < hiddenSize; ++j)
        for (int i = 0; i < inputSize; ++i)
            hidden[j] += inputs[i] * weightsInputHidden[i][j];
    for (double &h : hidden) h = sigmoid(h);

    for (int j = 0; j < outputSize; ++j)
        for (int i = 0; i < hiddenSize; ++i)
            outputs[j] += hidden[i] * weightsHiddenOutput[i][j];
    for (double &o : outputs) o = sigmoid(o);

    // Compute output layer error
    std::vector<double> outputErrors(outputSize);
    for (int j = 0; j < outputSize; ++j)
        outputErrors[j] = (targets[j] - outputs[j]) * sigmoidDerivative(outputs[j]);

    // Compute hidden layer error
    std::vector<double> hiddenErrors(hiddenSize, 0.0);
    for (int i = 0; i < hiddenSize; ++i) {
        for (int j = 0; j < outputSize; ++j)
            hiddenErrors[i] += outputErrors[j] * weightsHiddenOutput[i][j];
        hiddenErrors[i] *= sigmoidDerivative(hidden[i]);
    }

    // Update weights for hidden to output layer
    for (int i = 0; i < hiddenSize; ++i)
        for (int j = 0; j < outputSize; ++j)
            weightsHiddenOutput[i][j] += learningRate * outputErrors[j] * hidden[i];

    // Update weights for input to hidden layer
    for (int i = 0; i < inputSize; ++i)
        for (int j = 0; j < hiddenSize; ++j)
            weightsInputHidden[i][j] += learningRate * hiddenErrors[j] * inputs[i];
}
