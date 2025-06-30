#include "total_lines.hpp"
#include "../src/output_formatter.hpp"

#include <iostream>
#include <format>  // std::format (C++20)

// Process single file for line counting
void LineCounterModule::process_file(const fs::path& file_path) {  
    counter.count_lines(file_path);  // Count lines in file
}

// Print line counting statistics
void LineCounterModule::print_stats() const {      
    const auto& total_count = counter.get_total_count();     // Get total line counts
    size_t total = total_count.code + total_count.comments;  // Calculate total lines

    // Create string with results for "Total Lines" section
    std::string total_result_str = std::format("{}\n",
                                 OutputFormatter::format_large_number(total));

    std::cout << "⚑ Total Lines  [All Extensions: Code + Comments + Blanks]" << std::endl;
    std::cout << "╰─ " << total_result_str  << std::endl;
}
