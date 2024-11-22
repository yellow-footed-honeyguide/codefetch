#ifndef OUTPUT_FORMATTER_HPP
#define OUTPUT_FORMATTER_HPP

#include <string>       // [C++98]
#include <vector>       // [C++98]
#include <iostream>     // [C++98]
#include <iomanip>      // [C++98]
#include <sstream>      // [C++98]
#include <algorithm>    // [C++98]

class OutputFormatter {
public:
    // [C++11] Static method for printing sections with title and icon
    static void print_section(const std::string& title, const std::string& icon, 
        const std::vector<std::pair<std::string, std::string>>& items);

    // [C++11] Static method for printing language statistics with percentages
    static void print_language_stats(const std::vector<std::pair<std::string, double>>& stats, size_t total_lines);

    // [C++11] Static method for printing contributor statistics
    static void print_contributor_stats(const std::vector<std::pair<std::string, double>>& stats);

    static std::string format_large_number(size_t number);         // [C++11] Format numbers with locale-aware separators
    static std::string format_percentage(double percentage);       // [C++11] Format percentage with fixed precision
    static std::string truncate(const std::string& str, size_t width); // [C++11] Truncate string with ellipsis
};

#endif // OUTPUT_FORMATTER_HPP