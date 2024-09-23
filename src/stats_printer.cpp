#include "stats_printer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>

void StatsPrinter::print_total_lines(const LineCount& total_count) {
    std::cout << "\033[1;34mTotal Lines\033[0m\n";
    std::cout << "Code:     " << format_number(total_count.code.load()) << '\n';
    std::cout << "Comments: " << format_number(total_count.comments.load()) << '\n';
    std::cout << "Total:    " << format_number(total_count.code.load() + total_count.comments.load()) << "\n\n";
}

void StatsPrinter::print_language_stats(const LanguageStats& language_stats) {
    std::cout << "\033[1;34mLanguages\033[0m\n";
    auto sorted_stats = language_stats.get_sorted_stats();
    size_t total_lines = language_stats.get_total_lines();

    for (const auto& [language, lines] : sorted_stats) {
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;
        if (percentage >= 0.1) {
            std::cout << std::left << std::setw(20) << language 
                      << std::right << std::setw(5) << std::fixed << std::setprecision(1) << percentage 
                      << "% (" << format_number(lines) << " lines)\n";
        }
    }
}

std::string StatsPrinter::format_number(size_t number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}
