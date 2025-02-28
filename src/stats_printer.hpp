#pragma once

#include "line_count_util.hpp"  // For LineCount structure
#include "language_stats.hpp"   // For language statistics

class StatsPrinter {
public:
    static void print_total_lines(const LineCount& total_count);            // Print total line statistics
    static void print_language_stats(const LanguageStats& language_stats);  // Print language distribution

private:
    static std::string format_number(size_t number);                        // Format numbers with locale support
};

