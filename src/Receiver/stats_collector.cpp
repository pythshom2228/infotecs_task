#include <Logger/sink.hpp>
#include <Receiver/stats_collector.hpp>


void StatsCollector::add_message(const std::string& msg, logging::Level lvl) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.total++;
    stats_.per_level[lvl]++;
    changed_ = true;

    size_t len = msg.size();
    if (len < stats_.min_len) stats_.min_len = len;
    if (len > stats_.max_len) stats_.max_len = len;
    stats_.avg_len = ((stats_.avg_len * (stats_.total - 1)) + len) / stats_.total;

    auto now = std::chrono::system_clock::now();
    history_.push_back({now, len});

    // очистка старых сообщений старше 1 часа
    auto cutoff = now - std::chrono::hours(1);
    while (!history_.empty() && history_.front().first < cutoff) {
        history_.erase(history_.begin());
    }
    stats_.last_hour = history_.size();
}

void StatsCollector::print(std::ostream& os) {
    std::lock_guard<std::mutex> lock(mutex_);
    os << "\n========= Статистика ========= \n";
    os << " Всего сообщений: " << stats_.total << "\n";
    os << " Сообщений за последний час: " << stats_.last_hour << "\n";
    os << "По уровням:\n";
    os << "  Info:    " << stats_.per_level[logging::Level::Info] << "\n";
    os << "  Warning: " << stats_.per_level[logging::Level::Warning] << "\n";
    os << "  Error:   " << stats_.per_level[logging::Level::Error] << "\n";
    os << "Длины сообщений:\n";
    os << "  Мин: " << (stats_.min_len == SIZE_MAX ? 0 : stats_.min_len) << "\n";
    os << "  Макс: " << stats_.max_len << "\n";
    os << "  Средняя: " << stats_.avg_len << "\n";
    os << "==========================\n\n";
    reset_changed();
}


bool StatsCollector::has_changed() const { return changed_; }

void StatsCollector::reset_changed() { changed_ = false; }
