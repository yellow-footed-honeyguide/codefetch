#include "total_lines.hpp"
#include "../src/output_formatter.hpp"

#include <iostream>
#include <format> // для std::format (C++20)

void LineCounterModule::process_file(const fs::path& file_path) {  // Process single file for line counting
    counter.count_lines(file_path);                                // Count lines in file
}

void LineCounterModule::print_stats() const {                      // Print line counting statistics
    const auto& total_count = counter.get_total_count();           // Get total line counts
    size_t total = total_count.code + total_count.comments;      // Calculate total lines
    size_t other = total - total_count.code - total_count.comments; // Calculate other lines

    // create string with results for "Total Lines" section
    std::string total_result_str = std::format("{} (Total) = {} (Code) + {} (Comm.) + {} (Other)\n",
                                 OutputFormatter::format_large_number(total),
                                 OutputFormatter::format_large_number(total_count.code),
                                 OutputFormatter::format_large_number(total_count.comments),
                                 OutputFormatter::format_large_number(other));

    std::cout << "⚑ Total Lines" << std::endl;
    std::cout << "╰─ " << total_result_str  << std::endl;
}
