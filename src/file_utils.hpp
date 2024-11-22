#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <atomic>       // [C++11] For atomic counter
#include <filesystem>   // [C++17] For filesystem operations

#include "thread_safe_queue.hpp"

namespace fs = std::filesystem; // [C++17] Namespace alias for filesystem

namespace FileUtils {
    bool is_source_file(const fs::path& path); // [C++17] Check if file is a supported source file
    
    // [C++11] Traverse directory and collect source files with depth limit
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, 
        std::atomic<size_t>& total_files, int max_depth = 10);
}

#endif // FILE_UTILS_HPP