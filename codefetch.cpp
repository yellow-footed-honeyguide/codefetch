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
#include <locale>
#include <functional>
#include "file_utils.hpp"
#include "language_stats.hpp"
#include <indicators/progress_bar.hpp>
#include <iomanip>

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

    bool pop(fs::path& item, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (!cond.wait_for(lock, timeout, [this] { return !queue.empty(); })) {
            return false;  // Timeout occurred
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
LanguageStats language_stats;
std::mutex stats_mutex;
std::atomic<size_t> files_processed{0};
std::atomic<size_t> total_files{0};

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

    std::lock_guard<std::mutex> lock(stats_mutex);
    language_stats.add_file(file_path, count.code + count.comments);
}

void process_files() {
    fs::path file_path;
    while (file_queue.pop(file_path, std::chrono::seconds(5)) || !file_queue.empty()) {
        LineCount local_count;
        count_lines(file_path, local_count);
        total_count.code += local_count.code;
        total_count.comments += local_count.comments;
        files_processed++;
    }
}


void traverse_directory(const fs::path& dir_path, int max_depth = 10) {
    std::function<void(const fs::path&, int)> traverse = [&](const fs::path& path, int depth) {
        if (depth > max_depth) return;

        for (const auto& entry : fs::directory_iterator(path)) {
            const auto& current_path = entry.path();
            //std::cout << "Checking file: " << current_path << std::endl;

            if (fs::is_directory(current_path)) {
                traverse(current_path, depth + 1);
            } else if (fs::is_regular_file(current_path) && is_source_file(current_path) && !current_path.filename().string().starts_with(".")) {
                //std::cout << "Adding to queue: " << current_path << std::endl;
                file_queue.push(current_path);
                total_files++;
            }
        }
    };

    traverse(dir_path, 0);
}



std::string format_number(size_t number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}

void print_total_lines(const LineCount& total_count) {
    std::cout << "\033[1;34mTotal Lines:\033[0m\n";
    std::cout << "Code:     " << format_number(total_count.code.load()) << '\n';
    std::cout << "Comments: " << format_number(total_count.comments.load()) << '\n';
    std::cout << "Total:    " << format_number(total_count.code.load() + total_count.comments.load()) << "\n\n";
}

void print_language_stats(const LanguageStats& language_stats) {
    std::cout << "\033[1;34mLanguages:\033[0m\n";

    std::vector<std::pair<std::string, size_t>> sorted_stats = language_stats.get_sorted_stats();
    size_t total_lines = language_stats.get_total_lines();

    for (const auto& [language, lines] : sorted_stats) {
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;
        if (percentage >= 1.0) {
            std::cout << std::setw(20) << std::left << language;
            std::cout << std::setw(50) << std::left;
            int bar_width = static_cast<int>(percentage / 2);
            std::cout << std::string(bar_width, '=') << "> ";
            std::cout << std::fixed << std::setprecision(1) << percentage << "%\n";
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

    if (fs::is_empty(dir_path)) {
        std::cerr << "Error: The specified directory is empty." << std::endl;
        return 1;
    }

    traverse_directory(dir_path);

    if (total_files == 0) {
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    print_total_lines(total_count);
    print_language_stats(language_stats);

    return 0;
}
