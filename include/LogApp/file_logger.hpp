#pragma once
#include <Logger/logger.hpp>
#include <LogApp/task_queue.hpp>
#include <thread>

struct Task {
    std::string message;
    logging::Level level;
};

class FileLogger {
public:
    FileLogger(const std::string& logfile, logging::Level default_level);
    ~FileLogger();

    void run_console();

private:
    void worker_loop();
    logging::Level parse_level(const std::string& token);

private:
    logging::Logger logger_;
    TaskQueue<Task> queue_;
    std::thread worker_thread_;
};
