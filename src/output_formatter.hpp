#pragma once

#include <string>
#include <vector>
#include <iomanip>

class OutputFormatter {
public:
    // Static method for printing sections with title and icon
    static void print_section(const std::string& title, const std::string& icon, 
        const std::vector<std::pair<std::string, std::string>>& items);

    // Static method for printing language statistics with percentages
    static void print_language_stats(const std::vector<std::pair<std::string, double>>& stats, size_t total_lines);

    // Static method for printing contributor statistics
    static void print_contributor_stats(const std::vector<std::pair<std::string, double>>& stats);

    static std::string format_large_number(size_t number);    // Format numbers with locale-aware separators
    static std::string format_percentage(double percentage);  // Format percentage with fixed precision
    static std::string truncate(const std::string& str, size_t width);  // Truncate string with ellipsis
};
