#include "language_stats.hpp"
#include "../src/line_count_util.hpp"
#include "../src/output_formatter.hpp"

LanguageStatsModule::LanguageStatsModule(size_t languages_count): languages_count(languages_count) {}

// Declaration for compatibility with the base module (unused)
void LanguageStatsModule::print_stats() const{ print_stats(this->languages_count); }

// Implementation of print statistics with an argument
void LanguageStatsModule::process_file(const fs::path& file_path) { // Process single file statistics
    stats.add_file(file_path, LineCounter::count_lines_in_file(file_path)); // Count lines and add to statistics
}

void LanguageStatsModule::print_stats(size_t languages_count) const{  // Print language statistics
    // Get sorted language stats
    auto sorted_stats = stats.get_sorted_stats();
    std::vector<std::pair<std::string, double>> formatted_stats; // Container for formatted statistics

    double total_percentage = 0.0;                 // Track total percentage
    size_t total_lines = stats.get_total_lines();  // Get total lines
    size_t top_languages = 0;                      // Track number of top languages

    // Process top 5 languages (excluding "Other")
    for (const auto& [language, lines] : sorted_stats) {  // Structured binding
        if (top_languages >= languages_count) break;  // Limit printing number languages
        if (language != "Other") {      // Exclude "Other" category
            double percentage = (static_cast<double>(lines) / total_lines) * 100.0;  // Calculate percentage
            formatted_stats.emplace_back(language, percentage);  // Add language stats
            total_percentage += percentage;  // Update total percentage
            ++top_languages;                 // Increment counter
        }
    }

    // Add "Others" category with remaining percentage
    double others_percentage = 100.0 - total_percentage;
    formatted_stats.emplace_back("Others", others_percentage);   // Add others category

    OutputFormatter::print_language_stats(formatted_stats, total_lines); // Print formatted statistics
}
