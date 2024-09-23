#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <memory>
#include "file_utils.hpp"
#include "thread_safe_queue.hpp"
#include "../modules/statistics_module.hpp"
#include "../modules/line_counter_module.hpp"
#include "../modules/language_stats_module.hpp"

namespace fs = std::filesystem;

// Global variables
ThreadSafeQueue file_queue;
std::atomic<size_t> files_processed{0};
std::atomic<size_t> total_files{0};

// Function to process files using all registered modules
void process_files(std::vector<std::unique_ptr<StatisticsModule>>& modules) {
    fs::path file_path;
    while (file_queue.pop(file_path, std::chrono::seconds(5)) || !file_queue.empty()) {
        for (auto& module : modules) {
            module->process_file(file_path);
        }
        files_processed++;
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

    // Traverse the directory and populate the file queue
    FileUtils::traverse_directory(dir_path, file_queue, total_files);

    if (total_files == 0) {
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    // Initialize statistics modules
    std::vector<std::unique_ptr<StatisticsModule>> modules;
    modules.push_back(std::make_unique<LineCounterModule>());
    modules.push_back(std::make_unique<LanguageStatsModule>());

    // Create and start worker threads
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files, std::ref(modules));
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Print statistics from all modules
    for (const auto& module : modules) {
        module->print_stats();
    }

    return 0;
}
