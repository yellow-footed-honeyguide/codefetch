import file_utils;
import args_parser;
import analyzer_base;
import concurrent_queue;

import git_analyzer;
import lines_analyzer;
import license_analyzer;
import language_analyzer;
import metabuild_sys_analyzer;

#include <iostream>   
#include <memory>     
#include <thread>     
#include <vector>     
#include <atomic>     
#include <filesystem>

ThreadSafeQueue file_queue; // Thread-safe queue for parallel file processing
std::atomic<size_t> files_processed{0}; // Atomic counter for tracking progress
std::atomic<size_t> total_files{0}; // Atomic counter for total file count

// Worker function for parallel processing
void process_files(std::vector<std::unique_ptr<CodeFetchModule>> &modules) {  
    std::filesystem::path file_path;
    while (file_queue.pop(file_path)) { // Process files until queue is empty
        for (auto &module : modules) { // Apply each analysis module to the file
            module->process_file(file_path);
        }
        files_processed++;
    }
}

int main(int argc, char *argv[]) {
    ArgsParser parser("CodeFetch", PROJECT_VERSION); // Initialize parser
    std::string dir_path; // Directory path storage

    // CLI options
    bool show_line_counter = false;                  
    bool show_languages = false;
    bool show_git = false;
    bool show_metabuild_system = false;
    bool show_license = false;

    // Register all options
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
        parser.parse(argc, argv); // Parse arguments
        dir_path = parser.get_directory(); // Get directory path
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Validate input directory
    if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {  
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<CodeFetchModule>> modules; // Container for analysis modules
    if (!show_languages && !show_license && !show_metabuild_system && !show_git && !show_line_counter) {
        modules.push_back(std::make_unique<LineCounterModule>()); // Create module instances using make_unique 
        modules.push_back(std::make_unique<LanguageStatsModule>()); // Default: enable all modules
        modules.push_back(std::make_unique<GitModule>());
        modules.push_back(std::make_unique<MetabuildSystemModule>());
        modules.push_back(std::make_unique<LicenseModule>());
    } else { // Enable selected modules
        if (show_line_counter) modules.push_back(std::make_unique<LineCounterModule>()); 
        if (show_languages) modules.push_back(std::make_unique<LanguageStatsModule>());
        if (show_metabuild_system) modules.push_back(std::make_unique<MetabuildSystemModule>());
        if (show_license) modules.push_back(std::make_unique<LicenseModule>());
        if (show_git) modules.push_back(std::make_unique<GitModule>());
    }
    
    // Collect files recursively
    for (const auto &entry : std::filesystem::recursive_directory_iterator(dir_path)) {  
        if (std::filesystem::is_regular_file(entry) && // Filter source files
            FileUtils::is_source_file(entry.path())) {  
                file_queue.push(entry.path());
                total_files++;
        }
    }

    if (total_files == 0) { // Validate file collection results
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish(); // Signal end of file queueing

    unsigned int num_threads = std::thread::hardware_concurrency(); // Get optimal thread count
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) { // Create worker threads
        threads.emplace_back(process_files, std::ref(modules));
    }

    for (auto &thread : threads) { // Wait for all threads to complete
        thread.join();
    }

    for (const auto &module : modules) { // Print results from all modules
        module->print_stats();
    }

    return 0;
}