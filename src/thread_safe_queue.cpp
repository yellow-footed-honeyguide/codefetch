#include "thread_safe_queue.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void ThreadSafeQueue::push(const fs::path& item) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(item);
    cond.notify_one();
}

bool ThreadSafeQueue::pop(fs::path& item, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex);
    if (!cond.wait_for(lock, timeout, [this] { return !queue.empty(); })) {
        return false;  // Timeout occurred
    }
    item = queue.front();
    queue.pop();
    return true;
}

bool ThreadSafeQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}
