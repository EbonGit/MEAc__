#include "Network.h"

Network::Network(SignalType signalType) : tcp(IP, PORT), PythonAPI(), signalType(signalType) {

    std::vector<float> data = read_binary_file("data.bin");
    binaryData = reshape_data(data, 4);

    if (mode == Mode::GENERATE) {
        std::thread generateThread(&Network::launchGenerate, this);
        generateThread.detach();
    } else if (mode == Mode::TCP || mode == Mode::MEA) {
        std::thread tcpThread(&Network::launchTCP, this);
        tcpThread.detach();
    }
}

float Network::signalProcessing(float x) {
    switch (signalType) {
        case SignalType::RAW:
            return x;
        case SignalType::ABS:
            return std::abs(x);
        default:
            return x;
    }
}

void Network::generateNextPoint() {
    std::lock_guard<std::mutex> lock(signalsMutex);
    std::lock_guard<std::mutex> lock2(lastSignalMutex);

    for (int i = 0; i < numImages; i++) {
        if (i == 0){
            int idx = (int) t % (int)(binaryData[0].size()/10);
            float x = binaryData[0][idx*10] * 1000;
            signals[i].push(signalProcessing(x));

        }
        else if (i % 10 == 0){
            float x = ::generateSpikePoint(t + i * 10, (float)(4096 - i*50)/10 );
            x = addNoise(x, 1);
            signals[i].push(signalProcessing(x));
        }
        else if (i % 2 == 0){
            float x = ::generateSinusoidalPoint(t + i * 10, (float)(4096 - i*50) );
            signals[i].push(signalProcessing(x));
        } else {
            float x = ::generateSquareWavePoint(t + i * 10, (float)(4096 - i*50) );
            signals[i].push(signalProcessing(x));
        }
        lastSignal[i] = signals[i].peek();
    }

    if (saving) {
        write_single_point("save/data.bin", lastSignal);
    }

    t++;

}

void Network::launchGenerate() {
    while (true) {
        generateNextPoint();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int Network::receive() {
    int totalBytes = 8 * N * P + 8;

    while (true) {
        // Buffer to store the received data
        std::vector<char> data(totalBytes);
        int bytes_received = receiveData(data.data(), totalBytes);
        if (bytes_received <= 0) {
            std::cerr << "Error receiving data!" << std::endl;
            closeSocket();
            WSACleanup();
            return -1;
        }

        // Unpacking the first 4 bytes (big-endian integer)
        int decoded_int_x;
        memcpy(&decoded_int_x, data.data(), sizeof(decoded_int_x));
        decoded_int_x = ntohl(decoded_int_x);

        // Unpacking the next 4 bytes (big-endian integer)
        int decoded_int_y;
        memcpy(&decoded_int_y, data.data() + 4, sizeof(decoded_int_y));
        decoded_int_y = ntohl(decoded_int_y);

        if (decoded_int_x != N || decoded_int_y != P) {
            std::cerr << "Error: received data does not match the expected dimensions!" << std::endl;
            std::cout << "Expected: " << N << "x" << P << ", Received: " << decoded_int_x << "x" << decoded_int_y << std::endl;
            closeSocket();
            WSACleanup();
            return -1;
        }

        t += decoded_int_y;

        std::vector<std::vector<double>> decoded_data(decoded_int_x, std::vector<double>(decoded_int_y));

        size_t offset = sizeof(decoded_int_x) + sizeof(decoded_int_y);  // Start after the two integers

        for (int i = 0; i < decoded_int_x; ++i) {
            for (int j = 0; j < decoded_int_y; ++j) {
                decoded_data[i][j] = convertBigEndianToDouble(data.data() + offset);
                offset += sizeof(double);
            }
        }

        std::unique_lock<std::mutex> lock(signalsMutex);
        std::unique_lock<std::mutex> lock2(lastSignalMutex);

        // push data on top of each stack
        for (int i = 0; i < decoded_int_x; ++i) {
            for (int j = 0; j < decoded_int_y; ++j) {
                signals[i].push((float)decoded_data[i][j]);
            }
            lastSignal[i] = signals[i].peek();
        }

        int delta = numImages - decoded_int_x;
        for (int i = 0; i < delta; i++) {
            for (int j = 0; j < decoded_int_y; ++j) {
                signals[decoded_int_x + i].push(0.0);
            }
            lastSignal[decoded_int_x + i] = 0.0;
        }

        if (mode == Mode::MEA && !msg.isOpen) {
            msg.isOpen = true;
            char* buf = msg.buffer;
            sendData(buf, 3);
        }

        if (saving) {
            std::vector<std::vector<float>> saving_data;
            for (int i = 0; i < decoded_int_x; ++i) {
                saving_data.push_back(signals[i].get_last_n(P));
            }

            for (int i = 0; i < P; ++i) {
                std::vector<float> new_points;
                for (int j = 0; j < decoded_int_x; ++j) {
                    new_points.push_back(saving_data[j][i]);
                }
                write_single_point("save/data.bin", new_points);
            }
        }

        lock.unlock();
        lock2.unlock();
    }

    return 0;
}

void Network::launchTCP() {
    if (connectSocket()) {
        receive();
        closeSocket();
    }
}