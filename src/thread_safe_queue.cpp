#include "thread_safe_queue.hpp"

void ThreadSafeQueue::push(const std::filesystem::path& item) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(item);
    cond.notify_one();
}

bool ThreadSafeQueue::pop(std::filesystem::path& item) {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty() && !finished) {
        cond.wait(lock);
    }
    if (!queue.empty()) {
        item = queue.front();
        queue.pop();
        return true;
    }
    return false;
}

void ThreadSafeQueue::finish() {
    std::lock_guard<std::mutex> lock(mutex);
    finished = true;
    cond.notify_all();
}

bool ThreadSafeQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}
