#ifndef MEAC___TCP_DUMMY_H
#define MEAC___TCP_DUMMY_H
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <cstring>
#include "SimpleNeuralNetwork.h"

#pragma comment(lib, "ws2_32.lib") // Link the Winsock library

constexpr int PORT = 8080;  // Port to use

const std::vector<int> nn_index = {15, 16, 17, 18, 19, 20};
const std::vector<int> A_index = {39, 40, 47, 48};
const std::vector<int> B_index = {43, 44, 51, 52};
const int size_in = 2;

const std::vector<std::vector<double>> inputs_CST = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
};

const std::vector<std::vector<double>> outputs_CST = {
        {0},
        {1},
        {1},
        {0}
};

enum class Mode {
    SPIKE,
    SINUS,
    MEA
};

extern __int64 t;

class TCPServer {
public:
    TCPServer(int P = 4, int N = 4, Mode mode = Mode::SINUS);   // Constructor
    ~TCPServer();  // Destructor

    void start();  // Start the server and handle connections

private:
    WSADATA wsaData;
    SOCKET sockfd;
    SOCKET client_sock;

    int P;
    int N;
    Mode mode;

    std::vector<SimpleNeuralNetwork> nn;
    std::vector<int> io_in = {1, 1};
    std::vector<int> io_in_previous = {1, 1};
    int noise = 0;

    void send_data(SOCKET client_sock);
};


#endif //MEAC___TCP_DUMMY_H
