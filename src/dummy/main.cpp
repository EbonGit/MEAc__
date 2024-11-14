#include "tcp_dummy.h"

const std::vector<std::vector<double>> inputs = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
const std::vector<std::vector<double>> targets = {{0}, {1}, {1}, {0}};

int main() {

    // get parameters
    printf("Enter the number P: ");
    int p;
    scanf_s("%d", &p);
    printf("Enter the number N: ");
    int n;
    scanf_s("%d", &n);
    printf("Enter the mode (0: SPIKE, 1: SINUS, 2: MEA): ");
    int mode;
    scanf_s("%d", &mode);

    SimpleNeuralNetwork test = SimpleNeuralNetwork(2, 10, 1);

    // Train the network
    int epochs = 10000;
    for (int e = 0; e < epochs; ++e) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            test.train(inputs[i], targets[i]);
        }
    }

    // Test the network
    for (size_t i = 0; i < inputs.size(); ++i) {
        std::vector<double> output = test.forward(inputs[i]);
        std::cout << "Input: (" << inputs[i][0] << ", " << inputs[i][1] << ") -> Output: " << output[0] << std::endl;
    }

    try {
        TCPServer server = TCPServer(4, 60, Mode(mode));
        server.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
