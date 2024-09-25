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

    double total_percentage = 0.0;
    size_t total_lines = stats.get_total_lines();
    size_t top_languages = 0;

    // Process top 5 languages (excluding "Other")
    for (const auto& [language, lines] : sorted_stats) {
        if (top_languages >= 5) break;
        if (language != "Other") {
            double percentage = (static_cast<double>(lines) / total_lines) * 100.0;
            formatted_stats.emplace_back(language, percentage);
            total_percentage += percentage;
            ++top_languages;
        }
    }

    // Add "Others" category
    double others_percentage = 100.0 - total_percentage;
    formatted_stats.emplace_back("Others", others_percentage);

    OutputFormatter::print_language_stats(formatted_stats, total_lines);
}
