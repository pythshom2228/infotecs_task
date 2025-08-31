#include <Receiver/log_server.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    auto usage_str = "Использование: ./receiver <ip> <port> <N> <Tsec>\n";
    if (argc < 5) {
        std::cerr << usage_str;
        return EXIT_FAILURE;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    int N = std::atoi(argv[3]);
    int T = std::atoi(argv[4]);
    
    if(!port || !N || !T) {
        std::cerr << usage_str;
        return EXIT_FAILURE;
    }

    LogServer server(ip, port, N, T);
    server.run();
    return EXIT_SUCCESS;
}
