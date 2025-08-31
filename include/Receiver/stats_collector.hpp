#pragma once
#include <string>
#include <chrono>
#include <map>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>

#include <Logger/sink.hpp> 

struct Stats {
    size_t total = 0;
    std::map<logging::Level, size_t> per_level;
    size_t last_hour = 0;

    size_t min_len = SIZE_MAX;
    size_t max_len = 0;
    double avg_len = 0.0;
};

class StatsCollector {
public:
    void add_message(const std::string& msg, logging::Level lvl);

    void print(std::ostream& os);

    bool has_changed() const;
    void reset_changed();

private:
    std::mutex mutex_;
    std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> history_;
    Stats stats_;
    std::atomic<bool> changed_{false};
};
