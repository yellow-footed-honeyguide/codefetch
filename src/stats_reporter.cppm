module;  
#include <print>       
#include <locale>       
#include <vector>       
#include <sstream>      
#include <iomanip>      
#include <iostream>     
#include <algorithm>
#include <filesystem>     
export module stats_reporter;
import language_info;
import lines_count;

export class StatsPrinter {
public:
    // Show line counter statistics (-c, --line_counter)
    static void print_total_lines(const LineCount& total_count) {  
        std::println("\033[1;34mTotal Lines\033[0m"); 
        std::println("Code:     {}", format_number(total_count.code.load()));   
        std::println("Comments: {}", format_number(total_count.comments.load())); 
        std::println("Total:    {}\n", format_number(total_count.code.load() + total_count.comments.load()));
    }    
    
    // Show language statistics (-l, --languages)
    static void print_language_stats(const LanguageStats& language_stats) {
        std::println("\033[1;34mLanguages\033[0m"); 
        auto sorted_stats = language_stats.get_sorted_stats(); // Get sorted language statistics
        size_t total_lines = language_stats.get_total_lines(); // Get total line count
        size_t other_lines = 0; // Track remaining languages' lines
        size_t displayed_languages = 0; // Counter for displayed languages

        for (const auto& [language, lines] : sorted_stats) { // Structured binding for language stats
            double percentage = (static_cast<double>(lines) / total_lines) * 100.0; // Calculate percentage
            if (displayed_languages < 5) { // Display top 5 languages
                // Format output with proper alignment
                std::println("{:<20}{:>5.1f}% ({} lines)", language, percentage, format_number(lines));    
                displayed_languages++;
            } else {
                other_lines += lines; // Accumulate remaining lines
            }
        }

        if (other_lines > 0) {  // Print remaining languages as "Others"
            // Calculate others percentage
            double other_percentage = (static_cast<double>(other_lines) / total_lines) * 100.0;  
            // Format output for "Others" category
            //std::cout << std::left << std::setw(20) << "Others" << std::right << std::setw(5) << std::fixed << std::setprecision(1) << other_percentage << "% (" << format_number(other_lines) << " lines)\n";
            std::println(":<20Others {:>5.1f}% ({} lines)", other_percentage, format_number(other_lines));       
        }
    }


private:
    static std::string format_number(size_t number) {  // Format numbers with locale-aware separators
        std::stringstream ss; // String stream for number formatting
        ss.imbue(std::locale("")); // Use system locale for number formatting
        ss << std::fixed << number; // Format number with fixed notation
        return ss.str();
    }                     
};











