#include "thread_safe_queue.hpp"

void ThreadSafeQueue::push(const std::filesystem::path& item) {  // [C++17] Thread-safe method to add item to queue
    std::lock_guard<std::mutex> lock(mutex);                     // [C++11] RAII lock for thread safety
    queue.push(item);                                            // [C++11] Add item to underlying queue
    cond.notify_one();                                           // [C++11] Notify one waiting thread about new data
}

bool ThreadSafeQueue::pop(std::filesystem::path& item) {         // [C++17] Thread-safe method to remove item from queue
    std::unique_lock<std::mutex> lock(mutex);                    // [C++11] RAII lock with ability to unlock
    while (queue.empty() && !finished) {                         // [C++11] Wait while queue is empty and not finished
        cond.wait(lock);                                         // [C++11] Release lock and wait for notification
    }
    if (!queue.empty()) {                                        // [C++11] Check if queue has data
        item = queue.front();                                    // [C++11] Get first item
        queue.pop();                                             // [C++11] Remove first item
        return true;
    }
    return false;                                                // [C++11] Return false if queue is empty and finished
}

void ThreadSafeQueue::finish() {                                 // [C++11] Signal that no more items will be added
    std::lock_guard<std::mutex> lock(mutex);                     // [C++11] RAII lock for thread safety
    finished = true;                                             // [C++11] Set finish flag
    cond.notify_all();                                           // [C++11] Wake up all waiting threads
}

bool ThreadSafeQueue::empty() const {                            // [C++11] Thread-safe check if queue is empty
    std::lock_guard<std::mutex> lock(mutex);                     // [C++11] RAII lock for thread safety
    return queue.empty();                                        // [C++11] Return empty status
}