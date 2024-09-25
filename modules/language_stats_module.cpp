#include "language_stats_module.hpp"
#include "../src/line_count_util.hpp"
#include "../src/stats_printer.hpp"
#include "../src/output_formatter.hpp"

void LanguageStatsModule::process_file(const fs::path& file_path) {
    stats.add_file(file_path, LineCounter::count_lines_in_file(file_path));
}

void LanguageStatsModule::print_stats() const {
    auto sorted_stats = stats.get_sorted_stats();
    std::vector<std::pair<std::string, double>> formatted_stats;

    for (const auto& [language, lines] : sorted_stats) {
        double percentage = (static_cast<double>(lines) / stats.get_total_lines()) * 100.0;
        formatted_stats.emplace_back(language, percentage);
    }

    OutputFormatter::print_language_stats(formatted_stats, stats.get_total_lines());
}
