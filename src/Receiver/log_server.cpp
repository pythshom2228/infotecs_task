#include <Receiver/log_server.hpp>
#include <iostream>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace logging;

LogServer::LogServer(const std::string& ip, int port, int N, int T, std::size_t buffer_size)
    :   ip_(ip),
        port_(port),
        N_(N),
        T_(T),
        BUFFER_SIZE_(buffer_size) {}

LogServer::~LogServer() {
    running_ = false;
    if (timer_thread_.joinable()) timer_thread_.join();
    if (client_fd_ >= 0) close(client_fd_);
    if (server_fd_ >= 0) close(server_fd_);
}

void LogServer::run() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        perror("socket");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);

    if (inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Неверный IP адрес: " << ip_ << "\n";
        return;
    }

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return;
    }
    if (listen(server_fd_, 1) < 0) {
        perror("listen");
        return;
    }

    std::cout << "Сервер слушает на " << ip_ << ":" << port_ << "...\n";

    running_ = true;
    timer_thread_ = std::thread(&LogServer::timer_loop, this);

    accept_loop();
}

void LogServer::accept_loop() {
    client_fd_ = accept(server_fd_, nullptr, nullptr);
    if (client_fd_ < 0) {
        perror("accept");
        return;
    }
    
    // Если bytes реаллоцируется buffer инвалидируется
    std::vector<char> bytes(BUFFER_SIZE_);
    char* buffer = bytes.data();
    
    int messages = 0;

    while (running_) {
        ssize_t n = recv(client_fd_, buffer, BUFFER_SIZE_ * sizeof(buffer)  - 1, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        std::string msg(buffer);

        Level lvl = Level::Info;
        if (msg.find("ERROR") != std::string::npos) lvl = Level::Error;
        else if (msg.find("WARN") != std::string::npos) lvl = Level::Warning;

        collector_.add_message(msg, lvl);
        std::cout << msg << std::endl;
        cv_.notify_one();

        messages++;
        if (messages % N_ == 0) {
            collector_.print(std::cout);
        }
    }
}

void LogServer::timer_loop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&] {return collector_.has_changed();} );
        std::this_thread::sleep_for(std::chrono::seconds(T_));
        collector_.print(std::cout);
    }
}
