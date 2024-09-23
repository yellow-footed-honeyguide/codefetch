#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <iomanip>
#include "file_utils.hpp"

namespace fs = std::filesystem;

struct LineCount {
    std::atomic<size_t> code{0};
    std::atomic<size_t> comments{0};
};

class ThreadSafeQueue {
private:
    std::queue<fs::path> queue;
    mutable std::mutex mutex;
    std::condition_variable cond;

public:
    void push(const fs::path& item) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(item);
        cond.notify_one();
    }

    bool pop(fs::path& item) {
        std::unique_lock<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        item = queue.front();
        queue.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

ThreadSafeQueue file_queue;
LineCount total_count;


void count_lines(const fs::path& file_path, LineCount& count) {
    std::ifstream file(file_path);
    std::string line;
    bool in_multiline_comment = false;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty()) continue;

        if (!in_multiline_comment) {
            if (line.starts_with("//")) {
                count.comments++;
            } else if (line.starts_with("/*")) {
                count.comments++;
                in_multiline_comment = true;
                if (line.find("*/") != std::string::npos) {
                    in_multiline_comment = false;
                }
            } else {
                count.code++;
            }
        } else {
            count.comments++;
            if (line.find("*/") != std::string::npos) {
                in_multiline_comment = false;
            }
        }
    }
}

void process_files() {
    fs::path file_path;
    while (file_queue.pop(file_path) || !file_queue.empty()) {
        LineCount local_count;
        count_lines(file_path, local_count);
        total_count.code += local_count.code;
        total_count.comments += local_count.comments;
    }
}

void traverse_directory(const fs::path& dir_path) {
    for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
        const auto& path = entry.path();
        if (fs::is_regular_file(path) && is_source_file(path) && !path.filename().string().starts_with(".")) {
            file_queue.push(path);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return 1;
    }

    fs::path dir_path(argv[1]);
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    traverse_directory(dir_path);

    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Total lines: " << std::setw(8) << total_count.code.load() << " (code) + "
              << std::setw(8) << total_count.comments.load() << " (comments) = "
              << std::setw(10) << (total_count.code.load() + total_count.comments.load()) << std::endl;

    return 0;
}
