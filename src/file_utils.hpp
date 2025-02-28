#pragma once

#include <atomic>
#include <filesystem>

#include "thread_safe_queue.hpp"

namespace fs = std::filesystem;

namespace FileUtils {
    bool is_source_file(const fs::path& path);  // Check if file is a supported source file
    
    // Traverse directory and collect source files with depth limit
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, 
        std::atomic<size_t>& total_files, int max_depth = 10);
}

