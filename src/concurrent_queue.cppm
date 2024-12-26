module;  
#include <queue>              
#include <mutex>              
#include <condition_variable> 
#include <chrono>             
#include <filesystem>        
export module concurrent_queue;

export class ThreadSafeQueue {
private:
    std::queue<std::filesystem::path> queue; // Queue storing filesystem paths
    mutable std::mutex mutex; // Mutex for thread-safe access
    std::condition_variable cond; // Condition variable for thread signaling
    bool finished = false; // Flag for queue completion with in-class initializer

public:
    // Add path to queue
    void push(const std::filesystem::path& item) {  
        std::lock_guard<std::mutex> lock(mutex); // RAII lock for thread safety
        queue.push(item); // Add item to underlying queue
        cond.notify_one(); // Notify one waiting thread about new data
    }

    // Remove and return path from queue
    bool pop(std::filesystem::path& item) {        
        std::unique_lock<std::mutex> lock(mutex); // RAII lock with ability to unlock
        while (queue.empty() && !finished) { // Wait while queue is empty and not finished
            cond.wait(lock); // Release lock and wait for notification
        }
        if (!queue.empty()) { // Check if queue has data
            item = queue.front(); // Get first item
            queue.pop(); // Remove first item
            return true;
        }
        return false; // Return false if queue is empty and finished
    }

    // Mark queue as complete
    void finish() { // Signal that no more items will be added
        std::lock_guard<std::mutex> lock(mutex); // RAII lock for thread safety
        finished = true; // Set finish flag
        cond.notify_all(); // Wake up all waiting threads
    }

    // Check if queue is empty
    bool empty() const { // Thread-safe check if queue is empty
        std::lock_guard<std::mutex> lock(mutex); // RAII lock for thread safety
        return queue.empty(); // Return empty status
    }
};
