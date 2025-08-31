#pragma once
#include <Receiver/stats_collector.hpp>

#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

class LogServer {
public:
    LogServer(const std::string& ip, int port, int N, int T, std::size_t buffer_size=1024);
    ~LogServer();

    void run();   

private:
    void accept_loop();   // ожидание подключения и чтение сообщений
    void timer_loop();    // периодический вывод статистики

private:
    std::string ip_;
    int port_;
    int N_;
    int T_;

    int server_fd_{-1};
    int client_fd_{-1};

    StatsCollector collector_;
    std::thread timer_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};

    const std::size_t BUFFER_SIZE_;
};
