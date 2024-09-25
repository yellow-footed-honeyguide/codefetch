#ifndef OUTPUT_FORMATTER_HPP
#define OUTPUT_FORMATTER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

class OutputFormatter {
public:
    static void print_section(const std::string& title, const std::string& icon, const std::vector<std::pair<std::string, std::string>>& items);
    static void print_language_stats(const std::vector<std::pair<std::string, double>>& stats, size_t total_lines);
    static void print_contributor_stats(const std::vector<std::pair<std::string, double>>& stats);
    static std::string format_large_number(size_t number);
    static std::string format_percentage(double percentage);
    static std::string truncate(const std::string& str, size_t width);
};

#endif // OUTPUT_FORMATTER_HPP
