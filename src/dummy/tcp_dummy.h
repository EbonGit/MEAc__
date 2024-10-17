#ifndef MEAC___TCP_DUMMY_H
#define MEAC___TCP_DUMMY_H
#include <iostream>
#include <vector>
#include <winsock2.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib") // Link the Winsock library

constexpr int PORT = 8080;  // Port to use
constexpr int P = 10;       // Number of points per signal
constexpr int N = 4;        // Number of signals to generate

extern __int64 t;

class TCPServer {
public:
    TCPServer();   // Constructor
    ~TCPServer();  // Destructor

    void start();  // Start the server and handle connections

private:
    WSADATA wsaData;
    SOCKET sockfd;
    SOCKET client_sock;

    void send_data(SOCKET client_sock);
};


#endif //MEAC___TCP_DUMMY_H
