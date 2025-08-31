#pragma once
#include <Logger/file_logger.hpp>
#include <LogApp/task_queue.hpp>
#include <thread>

struct Task {
    std::string message;
    logging::Level level;
};

class LogApp {
public:
    LogApp(const std::string& logfile, logging::Level default_level);
    ~LogApp();

    void run_console();

private:
    void worker_loop();
    logging::Level parse_level(const std::string& token);

private:
    logging::FileLogger logger_;
    TaskQueue<Task> queue_;
    std::thread worker_thread_;
};
