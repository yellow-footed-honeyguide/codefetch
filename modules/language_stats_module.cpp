#include "language_stats_module.hpp"
#include "../src/line_count_util.hpp"
#include "../src/stats_printer.hpp"

void LanguageStatsModule::process_file(const fs::path& file_path) {
    stats.add_file(file_path, LineCounter::count_lines_in_file(file_path));
}

void LanguageStatsModule::print_stats() const {
    StatsPrinter::print_language_stats(stats);
}
