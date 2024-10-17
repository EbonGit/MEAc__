#include "tcp.h"

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

    std::cout << "Connected to the server." << std::endl;
    return true;
}

int receiveData(SOCKET sockfd, void* buffer, size_t totalBytes) {
    size_t bytesReceived = 0;
    char* buf = static_cast<char*>(buffer);

    while (bytesReceived < totalBytes) {
        int bytes = recv(sockfd, buf + bytesReceived, totalBytes - bytesReceived, 0);
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

double convertBigEndianToDouble(const char* data) {
    uint64_t raw;
    std::memcpy(&raw, data, sizeof(raw));  // Copy the raw bytes into a uint64_t
    raw = _byteswap_uint64(raw);  // Swap bytes for big-endian to host byte order
    double result;
    std::memcpy(&result, &raw, sizeof(result));  // Convert the uint64_t to a double
    return result;
}

int tcp::receive() {
    int totalBytes = 8 * N * P + 8;

    while (true) {
        // Buffer to store the received data
        std::vector<char> data(totalBytes);
        int bytes_received = ::receiveData(sockfd_, data.data(), totalBytes);
        if (bytes_received <= 0) {
            std::cerr << "Error receiving data!" << std::endl;
            closesocket(sockfd_);
            WSACleanup();
            return -1;
        }

        // Unpacking the first 4 bytes (big-endian integer)
        int decoded_int_x;
        memcpy(&decoded_int_x, data.data(), sizeof(decoded_int_x));  // Copy the first 4 bytes into decoded_int_x
        decoded_int_x = ntohl(decoded_int_x);  // Convert from network byte order to host byte order
        std::cout << "Decoded int X: " << decoded_int_x << std::endl;

        // Unpacking the next 4 bytes (big-endian integer)
        int decoded_int_y;
        memcpy(&decoded_int_y, data.data() + 4, sizeof(decoded_int_y));  // Copy the next 4 bytes into decoded_int_y
        decoded_int_y = ntohl(decoded_int_y);  // Convert from network byte order to host byte order
        std::cout << "Decoded int Y: " << decoded_int_y << std::endl;

        std::vector<std::vector<double>> decoded_data(decoded_int_x, std::vector<double>(decoded_int_y));

        size_t offset = sizeof(decoded_int_x) + sizeof(decoded_int_y);  // Start after the two integers

        // Loop over all interleaved signals (doubles)
        for (int i = 0; i < decoded_int_x; ++i) {
            for (int j = 0; j < decoded_int_y; ++j) {
                // Convert the next 8 bytes (Big Endian) to a double
                decoded_data[i][j] = convertBigEndianToDouble(data.data() + offset);
                offset += sizeof(double);  // Move the offset by 8 bytes (size of a double)
            }
        }

        // Print the decoded data (for verification)
        std::cout << "Decoded Data (N x P):" << std::endl;
        for (int i = 0; i < decoded_int_x; ++i) {
            for (int j = 0; j < decoded_int_y; ++j) {
                std::cout << decoded_data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    return 0;
}

void tcp::closeSocket() {
    if (sockfd_ != INVALID_SOCKET) {
        closesocket(sockfd_);
        std::cout << "Connection closed." << std::endl;
    }
    WSACleanup();  // Clean up Winsock
}
