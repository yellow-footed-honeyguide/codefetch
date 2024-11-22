#ifndef STATS_PRINTER_HPP
#define STATS_PRINTER_HPP

#include "line_count_util.hpp"  // [C++11] For LineCount structure
#include "language_stats.hpp"   // [C++11] For language statistics

class StatsPrinter {
public:
    static void print_total_lines(const LineCount& total_count);            // [C++11] Print total line statistics
    static void print_language_stats(const LanguageStats& language_stats);  // [C++11] Print language distribution

private:
    static std::string format_number(size_t number);                        // [C++11] Format numbers with locale support
};

#endif // STATS_PRINTER_HPP