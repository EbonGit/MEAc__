#include "tcp_dummy.h"

int main() {
    try {
        TCPServer server;
        server.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
