#ifndef MEAC___TCP_H
#define MEAC___TCP_H
#include "Data.h"
#include "config.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const int N = numImages;
const int P = 10;

class tcp: public virtual Data {
public:
    tcp(const std::string& ip, int port) : ip_(ip), port_(port), sockfd_(INVALID_SOCKET) {}

    bool connectSocket();

    int receive();

    void closeSocket();


private:
    std::string ip_;
    int port_;
    int sockfd_;

};


#endif //MEAC___TCP_H
