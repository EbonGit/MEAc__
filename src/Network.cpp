#include "Network.h"

Network::Network(SignalType signalType) : tcp(IP, PORT), signalType(signalType) {

    std::vector<float> data = read_binary_file("data.bin");
    binaryData = reshape_data(data, 4);

    if (mode == Mode::GENERATE) {
        std::thread generateThread(&Network::launchGenerate, this);
        generateThread.detach();
    } else if (mode == Mode::TCP) {
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

    t++;

}

void Network::launchGenerate() {
    while (true) {
        generateNextPoint();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void Network::launchTCP() {
    if (connectSocket()) {
        receive();
        closeSocket();
    }
}