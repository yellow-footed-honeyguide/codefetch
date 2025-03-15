#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <filesystem>
#include <chrono>

#include "file_utils.hpp"
#include "args_parser.hpp"
#include "license_detect.hpp"
#include "codefetch_module_interface.hpp"
#include "thread_safe_queue.hpp"
#include "modules/git_statistics.hpp"
#include "total_lines.hpp"
#include "language_stats.hpp"
#include "metabuild_system.hpp"


namespace fs = std::filesystem;

// Formatting the output of program running time
std::string formatDuration(std::chrono::milliseconds ms) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hrs = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hrs);

    std::string result;
    if (hrs.count() > 0) result += std::to_string(hrs.count()) + "h ";
    if (mins.count() > 0) result += std::to_string(mins.count()) + "m ";
    if (secs.count() > 0) result += std::to_string(secs.count()) + "s ";
    if (ms.count() > 0) result += std::to_string(ms.count()) + "ms";
    return result.empty() ? "0s" : result;
}

ThreadSafeQueue file_queue;                 // Thread-safe queue for parallel file processing
std::atomic<size_t> files_processed{0};  // Atomic counter for tracking progress
std::atomic<size_t> total_files{0};      // Atomic counter for total file count

// Worker function for parallel processing
void process_files(std::vector<std::unique_ptr<CodeFetchModule>> &modules) {  
    fs::path file_path;
    while (file_queue.pop(file_path)) {  // Process files until queue is empty
        for (auto &module : modules) {  // Apply each analysis module to the file
            module->process_file(file_path);
        }
        files_processed++;
    }
}

int main(int argc, char *argv[]) {
    // Start timer;
    auto start = std::chrono::high_resolution_clock::now();

    ArgsParser parser("CodeFetch", PROJECT_VERSION);  // Initialize parser
    std::string dir_path;                                            // Directory path storage

    bool show_total_lines = false;
    bool show_languages = false;
    bool show_git = false;
    bool show_metabuild_system = false;
    bool show_license = false;

    // Register all flags
    parser.add_flag("total_lines", &show_total_lines);
    parser.add_flag("t", &show_total_lines);
    parser.add_flag("languages", &show_languages);
    parser.add_flag("l", &show_languages);
    parser.add_flag("git-statistics", &show_git);
    parser.add_flag("g", &show_git);
    parser.add_flag("metabuild_system", &show_metabuild_system);
    parser.add_flag("m", &show_metabuild_system);
    parser.add_flag("license", &show_license);
    parser.add_flag("i", &show_license);

    try {
        parser.parse(argc, argv);           // Parse arguments
        dir_path = parser.get_directory();  // Get directory path
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {  // Validate input directory
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<CodeFetchModule>> modules;  // Container for analysis modules
    // Create module instances using make_unique
    // Default: enable all modules
    if (!show_languages && !show_license && !show_metabuild_system && !show_git && !show_total_lines) {
        modules.push_back(std::make_unique<LineCounterModule>());
        modules.push_back(std::make_unique<LanguageStatsModule>(3));
        modules.push_back(std::make_unique<GitModule>(3));
        modules.push_back(std::make_unique<MetabuildSystemModule>());
        modules.push_back(std::make_unique<LicenseModule>());
    } else { // Enable selected modules
        if (show_total_lines) modules.push_back(std::make_unique<LineCounterModule>());
        if (show_languages) modules.push_back(std::make_unique<LanguageStatsModule>(1000));
        if (show_metabuild_system) modules.push_back(std::make_unique<MetabuildSystemModule>());
        if (show_license) modules.push_back(std::make_unique<LicenseModule>());
        if (show_git) modules.push_back(std::make_unique<GitModule>(20)); // all possible languages
    }

    for (const auto &entry : fs::recursive_directory_iterator(dir_path)) {  // Collect files recursively
        if (fs::is_regular_file(entry) && FileUtils::is_source_file(entry.path())) {  // Filter source files
            file_queue.push(entry.path());
            total_files++;
        }
    }

    if (total_files == 0) {  // Validate file collection results
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish();  // Signal end of file queueing

    unsigned int num_threads = std::thread::hardware_concurrency();  // Get optimal thread count
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {  // Create worker threads
        threads.emplace_back(process_files, std::ref(modules));
    }

    for (auto &thread : threads) {  // Wait for all threads to complete
        thread.join();
    }

    for (const auto &module : modules) {  // Print results from all modules
        module->print_stats();
    }

    // End timer and print result
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "⏲ " << ms / 1000 << "s " << ms % 1000 << "ms\n";
    // std::cout << "⏲ 7,200 files | 2h 15m 30s" << std::endl;

    return 0;
}
