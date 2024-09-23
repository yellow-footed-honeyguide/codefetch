#include "line_counter_module.hpp"
#include "../src/stats_printer.hpp"

void LineCounterModule::process_file(const fs::path& file_path) {
    counter.count_lines(file_path);
}

void LineCounterModule::print_stats() const {
    StatsPrinter::print_total_lines(counter.get_total_count());
}
