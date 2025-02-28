#include "thread_safe_queue.hpp"

void ThreadSafeQueue::push(const std::filesystem::path& item) {  // Thread-safe method to add item to queue
    std::lock_guard<std::mutex> lock(mutex);  // RAII lock for thread safety
    queue.push(item);                             // Add item to underlying queue
    cond.notify_one();                            // Notify one waiting thread about new data
}

bool ThreadSafeQueue::pop(std::filesystem::path& item) {  // Thread-safe method to remove item from queue
    std::unique_lock<std::mutex> lock(mutex); // RAII lock with ability to unlock
    while (queue.empty() && !finished) {          // Wait while queue is empty and not finished
        cond.wait(lock);                      // Release lock and wait for notification
    }
    if (!queue.empty()) {                         // Check if queue has data
        item = queue.front();                     // Get first item
        queue.pop();                              // Remove first item
        return true;
    }
    return false;                                 // Return false if queue is empty and finished
}

void ThreadSafeQueue::finish() {                  // Signal that no more items will be added
    std::lock_guard<std::mutex> lock(mutex);  // RAII lock for thread safety
    finished = true;                              // Set finish flag
    cond.notify_all();                            // Wake up all waiting threads
}

bool ThreadSafeQueue::empty() const {             // Thread-safe check if queue is empty
    std::lock_guard<std::mutex> lock(mutex);  // RAII lock for thread safety
    return queue.empty();                         // Return empty status
}