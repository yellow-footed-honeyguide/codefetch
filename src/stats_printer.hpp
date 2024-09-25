#ifndef STATS_PRINTER_HPP
#define STATS_PRINTER_HPP

#include "line_count_util.hpp"
#include "language_stats.hpp"

class StatsPrinter {
public:
    static void print_total_lines(const LineCount& total_count);
    static void print_language_stats(const LanguageStats& language_stats);

private:
    static std::string format_number(size_t number);
};

#endif // STATS_PRINTER_HPP

