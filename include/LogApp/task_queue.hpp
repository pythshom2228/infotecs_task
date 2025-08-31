#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <optional>

template <typename T>
class TaskQueue {
public:
    TaskQueue() : running_(true) {}

    void push(const T& task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(task);
        }
        cv_.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]{ return !queue_.empty() || !running_; });

        if (!running_ && queue_.empty())
            return std::nullopt;

        T task = std::move(queue_.front());
        queue_.pop();
        return task;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_;
};
