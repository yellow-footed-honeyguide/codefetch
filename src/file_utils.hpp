#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <filesystem>
#include <atomic>
#include "thread_safe_queue.hpp"

namespace fs = std::filesystem;

namespace FileUtils {
    bool is_source_file(const fs::path& path);
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, std::atomic<size_t>& total_files, int max_depth = 10);
}

#endif // FILE_UTILS_HPP

