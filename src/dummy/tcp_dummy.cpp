#include "tcp_dummy.h"

__int64 t = 0;

bool contains(const std::vector<int>& v, int x) {
    return std::find(v.begin(), v.end(), x) != v.end();
}

TCPServer::TCPServer(int P, int N, Mode mode) : sockfd(INVALID_SOCKET), client_sock(INVALID_SOCKET) {
    this->P = P;
    this->N = N;
    this->mode = mode;
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        throw std::runtime_error("WSAStartup failed");
    }

    if (mode == Mode::MEA) {
        for (int i = 0; i < nn_index.size(); i++) {
            nn.push_back(SimpleNeuralNetwork(2, 10, 1));
        }
        std::cout << "NN initialized" << std::endl;
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
    serv_addr.sin_addr.s_addr = INADDR_ANY;

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

        // Main loop for sending and receiving data
        while (true) {
            this->send_data(client_sock);  // Send data to the client

            // Check for incoming data using select
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(client_sock, &readfds);

            timeval timeout = { 0, 0 };  // No timeout, just an immediate check

            // Check if there is data available or if the connection is closed
            int activity = select(0, &readfds, NULL, NULL, &timeout);
            if (activity == SOCKET_ERROR) {
                std::cerr << "Select failed" << std::endl;
                break;
            }

            if (activity > 0 && FD_ISSET(client_sock, &readfds)) {
                char buffer[5] = {0};  // Buffer to store the 3 bytes
                int len = 1;
                if (mode == Mode::MEA) {
                    len = 3;
                }
                int recv_result = recv(client_sock, buffer, len, 0);

                if (recv_result == 0) {
                    // Client has closed the connection
                    std::cout << "Client disconnected" << std::endl;
                    break;
                }
                if (recv_result == SOCKET_ERROR) {
                    std::cerr << "Error receiving data from client" << std::endl;
                    break;
                }

                // Process the 3-byte data if the mode is "MEA"
                if (mode == Mode::MEA) {
                    std::cout << "Received data: " << buffer << std::endl;
                    noise = buffer[0] - '0';
                    io_in_previous[0] = io_in[0];
                    io_in_previous[1] = io_in[1];
                    io_in[0] = buffer[1] - '0';
                    io_in[1] = buffer[2] - '0';
                }
            }

            Sleep(50);  // Pause to avoid CPU overload
        }

        // Close the client socket when done
        closesocket(client_sock);
        std::cout << "Waiting for a new connection..." << std::endl;
    }
}



float generateSinusoidalPoint(float t, float amplitude) {
    return (std::sin(t / 10.f)) * amplitude;
}

float generateRandomSpikePoint(float t, float amplitude) {
    if (rand() % 1000 < 1) {
        return amplitude * (rand() % 100) / 100.f;
    }
    float noise = 0;
    if (rand() % 2 == 0) {
        noise = (rand() % 100) / 100.f;
    } else {
        noise = -(rand() % 100) / 100.f;
    }
    return noise;
}

void TCPServer::send_data(SOCKET client_sock) {
    // Generate the 4 signals, each with P points
    std::vector<std::vector<double>> signals(N, std::vector<double>(P));

    int nn_temp_index = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            switch (mode) {
            case Mode::SINUS:
                signals[i][j] = generateSinusoidalPoint(((float)t*P + j), 10*(double)i + 1);
                break;
            case Mode::SPIKE:
                signals[i][j] = generateRandomSpikePoint(((float) t * P + j), 10 * (double) i + 1);
                break;
            case Mode::MEA:
                if (contains(A_index, i)) {
                    signals[i][j] = t % 2 ? 0 : io_in[0] * 100;
                } else if (contains(B_index, i)) {
                    signals[i][j] = t % 2 ? 0 : io_in[1] * 100;
                }
                else if (contains(nn_index, i)) {
                    if (t % 2 == 1) {
                        signals[i][j] = 0;
                    }
                    else {
                        signals[i][j] = nn[nn_temp_index].forward({double(io_in[0]), double(io_in[1])})[0] * 100;
                    }

                    if (j == P - 1) {
                        nn_temp_index++;
                    }
                }
                else {
                    signals[i][j] = 0;
                }
            }
        }
    }
    t++;

    // MAJ NN
    if (mode == Mode::MEA && noise) {
        for (int k = 0; k < nn_index.size(); k++) {
            std::vector<double> input = {double(io_in_previous[0]), double(io_in_previous[1])};
            std::vector<double> output = {double(io_in_previous[0] ^ io_in_previous[1])};

            int r = rand() % 10; // difference during training
            for (int i = 0; i < r; i++) {
                nn[k].train(input, output);
            }
        }
    }

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