#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <filesystem>

class ThreadSafeQueue {
private:
    std::queue<std::filesystem::path> queue;  // Queue storing filesystem paths
    mutable std::mutex mutex;                 //  Mutex for thread-safe access
    std::condition_variable cond;             // Condition variable for thread signaling
    bool finished = false;                    // Flag for queue completion with in-class initializer

public:
    void push(const std::filesystem::path& item);  // Add path to queue
    bool pop(std::filesystem::path& item);         // Remove and return path from queue
    void finish();                                 // Mark queue as complete
    bool empty() const;                            // Check if queue is empty
};

