// thread_safe_queue.hpp
#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <filesystem>

class ThreadSafeQueue {
private:
    std::queue<std::filesystem::path> queue;
    mutable std::mutex mutex;
    std::condition_variable cond;
    bool finished = false;

public:
    void push(const std::filesystem::path& item);
    bool pop(std::filesystem::path& item);
    void finish();
    bool empty() const;
};

#endif // THREAD_SAFE_QUEUE_HPP
