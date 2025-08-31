#include <Logger/socket_sink.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace logging {

std::unique_ptr<Sink> SocketSink::create(const std::string& host, int port) {
    return std::unique_ptr<Sink>(new SocketSink(host, port));
}

SocketSink::SocketSink(const std::string& host, int port) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) return;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        ::close(sock_);
        sock_ = -1;
        return;
    }

    if (connect(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(sock_);
        sock_ = -1;
        return;
    }

    valid_ = true;
}

SocketSink::~SocketSink() {
    if (sock_ >= 0) {
        ::close(sock_);
    }
}

void SocketSink::write(const std::string& formatted_message, Level) {
    if (!valid_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    ::send(sock_, formatted_message.c_str(), formatted_message.size(), 0);
}

} // namespace logging
