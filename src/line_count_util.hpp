#pragma once

#include <atomic>
#include <filesystem>

namespace fs = std::filesystem;

struct LineCount {
    std::atomic<size_t> code{0};     // Atomic counter for code lines with in-class initializer
    std::atomic<size_t> comments{0}; // Atomic counter for comment lines with in-class initializer
};

class LineCounter {
private:
    LineCount total_count;              // Total line counts using atomic counters

public:
    void count_lines(const fs::path& file_path);                  // Method for counting lines in file
    static size_t count_lines_in_file(const fs::path& file_path); // Static method for total line count
    const LineCount& get_total_count() const { return total_count; }  // Getter for total counts
};

