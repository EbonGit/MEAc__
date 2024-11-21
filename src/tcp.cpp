#include "tcp.h"

tcp::tcp(const std::string& i, int p) : ip_(i), port_(p), sockfd_(INVALID_SOCKET) {}

bool tcp::connectSocket() {
    WSADATA wsaData;
    int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock
    if (wsResult != 0) {
        std::cerr << "WSAStartup failed: " << wsResult << std::endl;
        return false;
    }

    sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd_ == INVALID_SOCKET) {
        std::cerr << "Error creating socket!" << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr);

    if (connect(sockfd_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed!" << std::endl;
        return false;
    }

    return true;
}

int tcp::receiveData(void* buffer, size_t totalBytes) {
    size_t bytesReceived = 0;
    char* buf = static_cast<char*>(buffer);

    while (bytesReceived < totalBytes) {
        int bytes = recv(sockfd_, buf + bytesReceived, totalBytes - bytesReceived, 0);
        if (bytes == 0) {
            return 0;  // Connection closed
        }
        if (bytes < 0) {
            return bytes;  // Error
        }
        bytesReceived += bytes;
    }
    return bytesReceived;  // Return total bytes received
}

void tcp::sendData(void* buffer, size_t totalBytes) {
    size_t bytesSent = 0;
    const char* buf = static_cast<const char*>(buffer);

    while (bytesSent < totalBytes) {
        int bytes = send(sockfd_, buf + bytesSent, totalBytes - bytesSent, 0);
        if (bytes == 0) {
            return;
        }
        if (bytes < 0) {
            std::cerr << "Error sending data!" << std::endl;
            return;
        }
        bytesSent += bytes;
    }
}

double convertBigEndianToDouble(const char* data) {
    uint64_t raw;
    std::memcpy(&raw, data, sizeof(raw));
    raw = _byteswap_uint64(raw);
    double result;
    std::memcpy(&result, &raw, sizeof(result));
    return result;
}

void tcp::closeSocket() {
    if (sockfd_ != INVALID_SOCKET) {
        closesocket(sockfd_);
        std::cout << "Connection closed." << std::endl;
    }
    WSACleanup();
}

