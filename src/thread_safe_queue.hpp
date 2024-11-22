#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <queue>                     // [C++98] Standard queue container
#include <mutex>                     // [C++11] Mutex for thread synchronization
#include <condition_variable>        // [C++11] Condition variable for thread waiting
#include <chrono>                    // [C++11] Time utilities
#include <filesystem>                // [C++17] Filesystem path handling

class ThreadSafeQueue {
private:
    std::queue<std::filesystem::path> queue;  // [C++17] Queue storing filesystem paths
    mutable std::mutex mutex;                 // [C++11] Mutex for thread-safe access
    std::condition_variable cond;             // [C++11] Condition variable for thread signaling
    bool finished = false;                    // [C++11] Flag for queue completion with in-class initializer

public:
    void push(const std::filesystem::path& item);  // [C++17] Add path to queue
    bool pop(std::filesystem::path& item);         // [C++17] Remove and return path from queue
    void finish();                                 // [C++11] Mark queue as complete
    bool empty() const;                            // [C++11] Check if queue is empty
};

#endif // THREAD_SAFE_QUEUE_HPP