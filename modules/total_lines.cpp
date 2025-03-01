#include "total_lines.hpp"
#include "../src/output_formatter.hpp"

void LineCounterModule::process_file(const fs::path& file_path) {  // Process single file for line counting
    counter.count_lines(file_path);                                // Count lines in file
}

void LineCounterModule::print_stats() const {                      // Print line counting statistics
    const auto& total_count = counter.get_total_count();           // Get total line counts
    size_t total = total_count.code + total_count.comments;      // Calculate total lines
    size_t other = total - total_count.code - total_count.comments; // Calculate other lines

    // Prepare formatted statistics items
    std::vector<std::pair<std::string, std::string>> items = {
        {"Overall", OutputFormatter::format_large_number(total)},
        {"Code", OutputFormatter::format_large_number(total_count.code)},
        {"Comm.", OutputFormatter::format_large_number(total_count.comments)},
        {"Other", OutputFormatter::format_large_number(other)}
    };

    OutputFormatter::print_section("Total Lines", "⚑", items); // Print formatted statistics
}
