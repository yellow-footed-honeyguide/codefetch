#ifndef LINE_COUNTER_HPP
#define LINE_COUNTER_HPP

#include <atomic>
#include <filesystem>

namespace fs = std::filesystem;

struct LineCount {
    std::atomic<size_t> code{0};
    std::atomic<size_t> comments{0};
};

class LineCounter {
private:
    LineCount total_count;

public:
    void count_lines(const fs::path& file_path);
    const LineCount& get_total_count() const { return total_count; }

    // Static method to count lines in a single file
    static size_t count_lines_in_file(const fs::path& file_path);
};

#endif // LINE_COUNTER_HPP
