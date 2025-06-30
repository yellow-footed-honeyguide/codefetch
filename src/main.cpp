#include <iostream>  
#include <memory>     
#include <thread>     
#include <vector>     
#include <atomic>
#include <filesystem> 
#include <chrono>     

#include "file_utils.hpp"                 // File traversal utilities
#include "args_parser.hpp"                // Command line argument parsing
#include "license_detect.hpp"             // License detection
#include "codefetch_module_interface.hpp" // Module interface
#include "thread_safe_queue.hpp"          // Thread-safe queue implementation
#include "modules/git_statistics.hpp"     // Git statistics module
#include "total_lines.hpp"                // Line counting functionality
#include "language_stats.hpp"             // Language statistics
#include "metabuild_system.hpp"           // Build system detection

namespace fs = std::filesystem;  // Filesystem namespace alias for brevity



// Converts milliseconds to human-readable format (h m s ms)
std::string formatDuration(std::chrono::milliseconds ms) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);   // Extract whole seconds
    ms -= duration_cast<milliseconds>(secs);  // Remainder milliseconds
    auto mins = duration_cast<minutes>(secs); // Extract whole minutes
    secs -= duration_cast<seconds>(mins);     // Remainder seconds
    auto hrs = duration_cast<hours>(mins);    // Extract whole hours
    mins -= duration_cast<minutes>(hrs);      // Remainder minutes

    std::string result;
    if (hrs.count() > 0) result += std::to_string(hrs.count()) + "h ";   // Add hours if present
    if (mins.count() > 0) result += std::to_string(mins.count()) + "m "; // Add minutes
    if (secs.count() > 0) result += std::to_string(secs.count()) + "s "; // Add seconds
    if (ms.count() > 0) result += std::to_string(ms.count()) + "ms";     // Add milliseconds
    return result.empty() ? "0s" : result;                               // Default to "0s" if empty
}

// Global thread-safe queue for file paths
ThreadSafeQueue file_queue;
// Atomic counters for progress tracking
std::atomic<size_t> files_processed{0};  // Tracks processed files
std::atomic<size_t> total_files{0};      // Tracks total files found

// Worker function for parallel file processing
void process_files(std::vector<std::unique_ptr<CodeFetchModule>> &modules) {  
    fs::path file_path;  // Stores current file path
    // Process files until queue is empty
    while (file_queue.pop(file_path)) {
        // Pass file through all active modules
        for (auto &module : modules) {
            module->process_file(file_path);  // Module-specific processing
        }
        files_processed++;  // Increment processed files counter
    }
}

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();  // Start timing execution

    // Initialize argument parser with program name and version
    ArgsParser parser("CodeFetch", PROJECT_VERSION);
    std::string dir_path;  // Stores target directory path

    // Flags for module activation
    bool show_total_lines = false;        // -t/--total_lines
    bool show_languages = false;          // -l/--languages
    bool show_git = false;                // -g/--git-statistics
    bool show_metabuild_system = false;   // -m/--metabuild_system
    bool show_license = false;            // -i/--license

    // Register command line flags (short and long versions)
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
        parser.parse(argc, argv);           // Parse command line arguments
        dir_path = parser.get_directory();  // Get target directory path
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;  // Print parsing errors
        return 1;                           // Exit on error
    }

    // Validate directory path exists and is accessible
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    // Collection of active modules
    std::vector<std::unique_ptr<CodeFetchModule>> modules;
    
    // If no flags specified, enable all modules with default settings
    if (!show_languages && !show_license && !show_metabuild_system && !show_git && !show_total_lines) {
        modules.push_back(std::make_unique<LineCounterModule>());     // Line counter
        modules.push_back(std::make_unique<LanguageStatsModule>(5));  // Top 5 languages
        modules.push_back(std::make_unique<GitModule>(5));            // Top 5 git contributors
        modules.push_back(std::make_unique<MetabuildSystemModule>()); // Build system detection
        modules.push_back(std::make_unique<LicenseModule>());         // License detection
    } else {
        // Add modules based on specified flags
        if (show_total_lines) modules.push_back(std::make_unique<LineCounterModule>());
        if (show_languages) modules.push_back(std::make_unique<LanguageStatsModule>(1000));  // High threshold
        if (show_metabuild_system) modules.push_back(std::make_unique<MetabuildSystemModule>());
        if (show_license) modules.push_back(std::make_unique<LicenseModule>());
        if (show_git) modules.push_back(std::make_unique<GitModule>(20));  // Top 20 contributors
    }

    // Traverse directory and populate file queue (single filesystem pass)
    FileUtils::traverse_directory(dir_path, file_queue, total_files);

    // Check if any files were found
    if (total_files == 0) {
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish();  // Signal that no more files will be added

    // Determine optimal thread count (use all available cores)
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;  // Fallback to 4 threads if detection fails
    
    std::vector<std::thread> threads;  // Worker thread container
    threads.reserve(num_threads);      // Pre-allocate memory
    
    // Create worker threads
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files, std::ref(modules));  // Pass modules by reference
    }

    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }

    // Print statistics from all modules
    for (const auto &module : modules) {
        module->print_stats();
    }

    // Calculate and print execution time
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "⏲ " << ms / 1000 << "s " << ms % 1000 << "ms\n";

    return 0;  
}
