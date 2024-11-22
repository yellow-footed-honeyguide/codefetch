#include <locale>       // [C++11] For locale-aware number formatting
#include <vector>       // [C++98]
#include <sstream>      // [C++98]
#include <iomanip>      // [C++98]
#include <iostream>     // [C++98]
#include <algorithm>    // [C++98]

#include "stats_printer.hpp"

// [C++11] Print formatted line count statistics
void StatsPrinter::print_total_lines(const LineCount& total_count) {  
    // [C++11] Print colored header using ANSI escape codes
    std::cout << "\033[1;34mTotal Lines\033[0m\n";                   
    // [C++11] Print code lines with atomic load
    std::cout << "Code:     " << format_number(total_count.code.load()) << '\n';    
    // [C++11] Print comment lines with atomic load
    std::cout << "Comments: " << format_number(total_count.comments.load()) << '\n'; 
    // [C++11] Print total
    std::cout << "Total:    " << format_number(total_count.code.load() + total_count.comments.load()) << "\n\n"; 
}

// [C++11] Print language statistics with percentages and line counts
void StatsPrinter::print_language_stats(const LanguageStats& language_stats) {
    std::cout << "\033[1;34mLanguages\033[0m\n";            // [C++11] Print colored header
    auto sorted_stats = language_stats.get_sorted_stats();  // [C++11] Get sorted language statistics
    size_t total_lines = language_stats.get_total_lines();  // [C++11] Get total line count
    size_t other_lines = 0;                                 // [C++11] Track remaining languages' lines
    size_t displayed_languages = 0;                         // [C++11] Counter for displayed languages

    for (const auto& [language, lines] : sorted_stats) {    // [C++17] Structured binding for language stats
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;  // [C++11] Calculate percentage
        if (displayed_languages < 5) {                      // [C++11] Display top 5 languages
            // [C++11] Format output with proper alignment
            std::cout << std::left << std::setw(20) << language 
                      << std::right << std::setw(5) << std::fixed << std::setprecision(1) << percentage 
                      << "% (" << format_number(lines) << " lines)\n";
            displayed_languages++;
        } else {
            other_lines += lines;                           // [C++11] Accumulate remaining lines
        }
    }

    if (other_lines > 0) {  // [C++11] Print remaining languages as "Others"
        // [C++11] Calculate others percentage
        double other_percentage = (static_cast<double>(other_lines) / total_lines) * 100.0;  
        // [C++11] Format output for "Others" category
        std::cout << std::left << std::setw(20) << "Others" 
                  << std::right << std::setw(5) << std::fixed << std::setprecision(1) << other_percentage 
                  << "% (" << format_number(other_lines) << " lines)\n";
    }
}

std::string StatsPrinter::format_number(size_t number) {  // [C++11] Format numbers with locale-aware separators
    std::stringstream ss;                                 // [C++11] String stream for number formatting
    ss.imbue(std::locale(""));                            // [C++11] Use system locale for number formatting
    ss << std::fixed << number;                           // [C++11] Format number with fixed notation
    return ss.str();
}