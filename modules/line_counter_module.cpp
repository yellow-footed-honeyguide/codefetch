#include "line_counter_module.hpp"
#include "../src/stats_printer.hpp"
#include "../src/output_formatter.hpp"

void LineCounterModule::process_file(const fs::path& file_path) {
    counter.count_lines(file_path);
}

void LineCounterModule::print_stats() const {
    const auto& total_count = counter.get_total_count();
    size_t total = total_count.code + total_count.comments;
    size_t other = total - total_count.code - total_count.comments;

    std::vector<std::pair<std::string, std::string>> items = {
        {"Total", OutputFormatter::format_large_number(total)},
        {"Code", OutputFormatter::format_large_number(total_count.code)},
        {"Comm.", OutputFormatter::format_large_number(total_count.comments)},
        {"Other", OutputFormatter::format_large_number(other)}
    };

    OutputFormatter::print_section("Lines of Code", "⚑", items);
}
