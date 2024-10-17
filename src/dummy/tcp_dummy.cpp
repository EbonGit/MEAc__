#include "tcp_dummy.h"

__int64 t = 0;

TCPServer::TCPServer() : sockfd(INVALID_SOCKET), client_sock(INVALID_SOCKET) {
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        throw std::runtime_error("WSAStartup failed");
    }
}

TCPServer::~TCPServer() {
    // Close sockets and cleanup
    if (client_sock != INVALID_SOCKET) {
        closesocket(client_sock);
    }
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
    }
    WSACleanup();
}

void TCPServer::start() {
    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        throw std::runtime_error("Error creating socket");
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on any incoming address

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        throw std::runtime_error("Bind failed");
    }

    // Start listening for connections
    if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Waiting for a connection on port " << PORT << "..." << std::endl;

    // Accept and handle connections
    while (true) {
        client_sock = accept(sockfd, NULL, NULL);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            throw std::runtime_error("Accept failed");
        }

        std::cout << "Connection established. Sending data..." << std::endl;
        // Send data to the client at regular intervals
        while (true) {
            this->send_data(client_sock);

            // Check if the client has closed the connection
            char buffer[1];
            int recv_result = recv(client_sock, buffer, sizeof(buffer), 0);

            if (recv_result == 0) {
                std::cout << "Client disconnected" << std::endl;
                break;  // Client has closed the connection, exit the inner loop
            }
            if (recv_result == SOCKET_ERROR) {
                std::cerr << "Error receiving data from client" << std::endl;
                break;  // Error occurred, exit the inner loop
            }

            Sleep(25); // Wait for 250 ms before sending new data
        }

        // Close the client socket when done
        closesocket(client_sock);
        std::cout << "Waiting for a new connection..." << std::endl;
    }
}

float generateSinusoidalPoint(float t, float amplitude) {
    return (std::sin(t / 10.f)) * amplitude;
}

void TCPServer::send_data(SOCKET client_sock) {
    // Generate the 4 signals, each with P points
    std::vector<std::vector<double>> signals(N, std::vector<double>(P));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            signals[i][j] = generateSinusoidalPoint(((float)t*P + j)/10.f, 10*(double)i + 1);
        }
    }
    t++;

    std::vector<double> interleaved_signals(P * N);

    for (int j = 0; j < N; j++) {
        for (int i = 0; i < P; i++) {
            interleaved_signals[j * P + i] = signals[j][i];
        }
    }

    // Encoding the two integers (N and P) in Big Endian
    int32_t int_x = htonl(N);  // Network byte order (Big Endian)
    int32_t int_y = htonl(P);

    // Calculate the size of the data to send
    size_t total_size = sizeof(int_x) + sizeof(int_y) + P * N * sizeof(double);
    std::vector<char> buffer(total_size);

    // Copy the integers (N and P) into the buffer
    memcpy(buffer.data(), &int_x, sizeof(int_x));
    memcpy(buffer.data() + sizeof(int_x), &int_y, sizeof(int_y));

    // Copy the interleaved signal data (doubles) into the buffer
    for (size_t i = 0; i < P * N; ++i) {
        uint64_t signal_be;
        memcpy(&signal_be, &interleaved_signals[i], sizeof(double)); // Copy the double into raw format
        signal_be = _byteswap_uint64(signal_be);   // Convert to Big Endian (Windows specific)
        memcpy(buffer.data() + sizeof(int_x) + sizeof(int_y) + i * sizeof(signal_be), &signal_be, sizeof(signal_be));
    }

    // Send all the data at once
    send(client_sock, buffer.data(), total_size, 0);
}