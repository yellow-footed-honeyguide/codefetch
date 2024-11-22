#include <iostream>      // [C++98]
#include <memory>        // [C++11]
#include <thread>        // [C++11]
#include <vector>        // [C++11]
#include <atomic>        // [C++11]
#include <filesystem>    // [C++17]

//#include <CLI/CLI.hpp>   // [External]

#include "file_utils.hpp"
#include "license_module.hpp"
#include "statistics_module.hpp"
#include "thread_safe_queue.hpp"
#include "modules/git_module.hpp"
#include "line_counter_module.hpp"
#include "language_stats_module.hpp"
#include "metabuild_system_module.hpp"
#include "args_parser.hpp"


namespace fs = std::filesystem;          // [C++17] Alias for filesystem namespace for cleaner code

ThreadSafeQueue file_queue;              // [C++11] Thread-safe queue for parallel file processing
std::atomic<size_t> files_processed{0};  // [C++11] Atomic counter for tracking progress
std::atomic<size_t> total_files{0};      // [C++11] Atomic counter for total file count

// [C++11] Worker function for parallel processing
void process_files(std::vector<std::unique_ptr<CodeFetchModule>> &modules) {  
    fs::path file_path;
    while (file_queue.pop(file_path)) {  // [C++11] Process files until queue is empty
        for (auto &module : modules) {   // [C++11] Apply each analysis module to the file
            module->process_file(file_path);
        }
        files_processed++;
    }
}

int main(int argc, char *argv[]) {
    ArgsParser parser("CodeFetch", PROJECT_VERSION);          // [C++11] Initialize parser
    std::string dir_path;                                     // [C++11] Directory path storage

    bool show_line_counter = false;                           // [C++11] Analysis flags
    bool show_languages = false;
    bool show_git = false;
    bool show_metabuild_system = false;
    bool show_license = false;

    // [C++11] Register all flags
    parser.add_flag("line_counter", &show_line_counter);      
    parser.add_flag("c", &show_line_counter);
    parser.add_flag("languages", &show_languages);
    parser.add_flag("l", &show_languages);
    parser.add_flag("git-statistics", &show_git);
    parser.add_flag("g", &show_git);
    parser.add_flag("metabuild_system", &show_metabuild_system);
    parser.add_flag("m", &show_metabuild_system);
    parser.add_flag("license", &show_license);
    parser.add_flag("i", &show_license);

    try {
        parser.parse(argc, argv);                             // [C++11] Parse arguments
        dir_path = parser.get_directory();                    // [C++11] Get directory path
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {  // [C++17] Validate input directory
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<CodeFetchModule>> modules;          // [C++11] Container for analysis modules
    if (!show_languages && !show_license && !show_metabuild_system && !show_git && !show_line_counter) {
        modules.push_back(std::make_unique<LineCounterModule>());   // [C++14] Create module instances using make_unique
        modules.push_back(std::make_unique<LanguageStatsModule>()); // [C++14] Default: enable all modules
        modules.push_back(std::make_unique<GitModule>());
        modules.push_back(std::make_unique<MetabuildSystemModule>());
        modules.push_back(std::make_unique<LicenseModule>());
    } else {
        if (show_line_counter) modules.push_back(std::make_unique<LineCounterModule>()); // [C++14] Enable selected modules
        if (show_languages) modules.push_back(std::make_unique<LanguageStatsModule>());
        if (show_metabuild_system) modules.push_back(std::make_unique<MetabuildSystemModule>());
        if (show_license) modules.push_back(std::make_unique<LicenseModule>());
        if (show_git) modules.push_back(std::make_unique<GitModule>());
    }

    for (const auto &entry : fs::recursive_directory_iterator(dir_path)) {  // [C++17] Collect files recursively
        if (fs::is_regular_file(entry) && FileUtils::is_source_file(entry.path())) {  // [C++17] Filter source files
            file_queue.push(entry.path());
            total_files++;
        }
    }

    if (total_files == 0) {  // Validate file collection results
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish();  // [C++11] Signal end of file queueing

    unsigned int num_threads = std::thread::hardware_concurrency();  // [C++11] Get optimal thread count
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {  // [C++11] Create worker threads
        threads.emplace_back(process_files, std::ref(modules));
    }

    for (auto &thread : threads) {        // [C++11] Wait for all threads to complete
        thread.join();
    }

    for (const auto &module : modules) {  // [C++11] Print results from all modules
        module->print_stats();
    }

    return 0;
}