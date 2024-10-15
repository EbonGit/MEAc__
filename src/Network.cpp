#include "Network.h"
#include <utility>

Network::Network(std::string IP, int port, SignalType signalType) : IP(std::move(IP)), port(port), signalType(signalType) {
    std::cout << "Network instance created with IP: " << this->IP << " and port: " << this->port << std::endl;

    std::vector<float> data = read_binary_file("data.bin");
    binaryData = reshape_data(data, 4);
}

float Network::signalProcessing(float x) {
    switch (signalType) {
        case SignalType::RAW:
            return x;
        case SignalType::ABSOLUTE:
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