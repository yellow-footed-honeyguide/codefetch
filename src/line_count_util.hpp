#ifndef LINE_COUNTER_HPP
#define LINE_COUNTER_HPP

#include <atomic>       // [C++11] For atomic operations
#include <filesystem>   // [C++17] For filesystem paths

namespace fs = std::filesystem;      // [C++17] Namespace alias for filesystem

struct LineCount {
    std::atomic<size_t> code{0};     // [C++11] Atomic counter for code lines with in-class initializer
    std::atomic<size_t> comments{0}; // [C++11] Atomic counter for comment lines with in-class initializer
};

class LineCounter {
private:
    LineCount total_count;           // [C++11] Total line counts using atomic counters

public:
    void count_lines(const fs::path& file_path);                  // [C++17] Method for counting lines in file
    static size_t count_lines_in_file(const fs::path& file_path); // [C++17] Static method for total line count
    const LineCount& get_total_count() const { return total_count; }  // [C++11] Getter for total counts
};

#endif // LINE_COUNTER_HPP