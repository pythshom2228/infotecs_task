#include "assert_true.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <Logger/logger.hpp>
#include <Logger/file_sink.hpp>

using namespace logging;

// Вспомогательный sink (в память, потокобезопасный) 
class TestSink final : public Sink {
public:
    void write(const std::string& formatted_message, Level) override {
        std::lock_guard<std::mutex> lk(m_);
        msgs_.push_back(formatted_message);
    }
    std::size_t size() const {
        std::lock_guard<std::mutex> lk(m_);
        return msgs_.size();
    }
    std::vector<std::string> snapshot() const {
        std::lock_guard<std::mutex> lk(m_);
        return msgs_;
    }
private:
    mutable std::mutex m_;
    std::vector<std::string> msgs_;
};

// Тесты Logger (фильтрация) 
void test_logger_level_filter() {
    Logger logger(Level::Warning);
    auto sink = std::make_unique<TestSink>();
    auto* raw = sink.get();
    logger.add_sink(std::move(sink));

    logger.log("info_should_be_dropped", Level::Info);
    logger.log("warn_should_pass",       Level::Warning);
    logger.log("error_should_pass",      Level::Error);

    ASSERT_TRUE(raw->size() == 2, "Logger must drop messages below current level");

    auto msgs = raw->snapshot();
    ASSERT_TRUE(msgs[0].find("warn_should_pass")  != std::string::npos, "Warning must be logged");
    ASSERT_TRUE(msgs[1].find("error_should_pass") != std::string::npos, "Error must be logged");
}

// Тесты Logger (смена уровня на лету)
void test_logger_set_level_runtime() {
    Logger logger(Level::Error);
    auto sink = std::make_unique<TestSink>();
    auto* raw = sink.get();
    logger.add_sink(std::move(sink));

    logger.log("will_be_dropped_info", Level::Info);
    logger.log("will_be_dropped_warn", Level::Warning);
    ASSERT_TRUE(raw->size() == 0, "Nothing must be logged at Error level yet");

    logger.set_level(Level::Info);
    logger.log("now_info_passes", Level::Info);

    ASSERT_TRUE(raw->size() == 1, "After set_level(Info) one message must be logged");
    auto msgs = raw->snapshot();
    ASSERT_TRUE(msgs[0].find("now_info_passes") != std::string::npos, "Logged message must match");
}

//  Тесты Logger (потокобезопасность) 
void test_logger_thread_safety() {
    Logger logger(Level::Info);
    auto sink = std::make_unique<TestSink>();
    auto* raw = sink.get();
    logger.add_sink(std::move(sink));

    constexpr int kThreads = 8;
    constexpr int kPerThread = 200;

    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([t, &logger] {
            for (int i = 0; i < kPerThread; ++i) {
                logger.log("t" + std::to_string(t) + "_m" + std::to_string(i), Level::Info);
            }
        });
    }
    for (auto& th : workers) th.join();

    ASSERT_TRUE(raw->size() == static_cast<std::size_t>(kThreads * kPerThread),
                "All concurrent log() calls must be recorded exactly once");
}

// Тесты FileSink (создание и перезапись)
void test_file_sink_create_and_overwrite() {
    namespace fs = std::filesystem;
    const std::string path = "test_logger_file_sink.log";
    if (fs::exists(path)) fs::remove(path);

    {
        auto sink = FileSink::create(path);
        sink->write("first_line", Level::Info);
    }
    ASSERT_TRUE(std::filesystem::exists(path), "FileSink must create file if it does not exist");

    {
        std::ifstream in(path);
        std::string line;
        std::getline(in, line);
        ASSERT_TRUE(line.find("first_line") != std::string::npos, "First write must be present");
    }

    // Повторное создание должно перезаписать файл по умолчанию
    {
        auto sink = FileSink::create(path);
        sink->write("second_line", Level::Info);
    }
    {
        std::ifstream in(path);
        std::string line;
        std::getline(in, line);
        ASSERT_TRUE(line.find("second_line") != std::string::npos,
                    "Second write must overwrite previous content");
    }
}


// Run Tests
int main() {
    test_logger_level_filter();
    test_logger_set_level_runtime();
    test_logger_thread_safety();
    test_file_sink_create_and_overwrite();

    std::cout << "test_logger: All tests passed!\n";
    return 0;
}
