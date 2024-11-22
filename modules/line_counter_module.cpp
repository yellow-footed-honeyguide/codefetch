#include "line_counter_module.hpp"
#include "../src/stats_printer.hpp"
#include "../src/output_formatter.hpp"

void LineCounterModule::process_file(const fs::path& file_path) {  // [C++17] Process single file for line counting
    counter.count_lines(file_path);                                // Count lines in file
}

void LineCounterModule::print_stats() const {                      // [C++11] Print line counting statistics
    const auto& total_count = counter.get_total_count();           // [C++11] Get total line counts
    size_t total = total_count.code + total_count.comments;        // Calculate total lines
    size_t other = total - total_count.code - total_count.comments; // Calculate other lines

    // [C++11] Prepare formatted statistics items
    std::vector<std::pair<std::string, std::string>> items = {
        {"Total", OutputFormatter::format_large_number(total)},
        {"Code", OutputFormatter::format_large_number(total_count.code)},
        {"Comm.", OutputFormatter::format_large_number(total_count.comments)},
        {"Other", OutputFormatter::format_large_number(other)}
    };

    OutputFormatter::print_section("Lines of Code", "⚑", items); // Print formatted statistics
}
