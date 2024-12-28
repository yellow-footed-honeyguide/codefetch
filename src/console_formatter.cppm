module;  
#include <print>       
#include <string>       
#include <vector>       
#include <iostream>     
#include <iomanip>     
#include <sstream>      
#include <algorithm>    
export module console_formatter;


export class OutputFormatter {
public:
    // Print formatted section with title and items using modern string collections
    static void print_section(const std::string& title, const std::string& icon, 
        const std::vector<std::pair<std::string, std::string>>& items) {
        std::println("{} {}", icon, title); // Print section header with icon
        for (const auto& [key, value] : items) { // Structured binding for pair iteration
            // Format output with alignment
            std::println("  ╰─ {:<10}: {}", key, value); 
        }
        std::println(); 
    }

    // Print language statistics with distribution percentages
    static void print_language_stats(const std::vector<std::pair<std::string, double>>& stats, 
    size_t total_lines) {
        std::println("⚐ Languages"); 
        for (const auto& [language, percentage] : stats) { // Structured binding for stats pairs
            size_t lines = static_cast<size_t>(percentage * total_lines / 100); // Calculate absolute lines count
            // Format output with proper alignment and number formatting
            std::println("  ╰─ {:<10}: {:>5}  ({} lines)", truncate(language, 10), 
                                                           format_percentage(percentage),
                                                           format_large_number(lines));
        }
        std::println();
    }

    // Print contributor stats with percentages
    static void print_contributor_stats(const std::vector<std::pair<std::string, double>>& stats) {
        std::println("☺ Top Contributors");
        for (const auto& [name, percentage] : stats) { // Structured binding for contributor data
            // Format contributor info with proper alignment
            std::println("  ╰─ {:<14}: {:>6}", truncate(name, 14), 
                                               format_percentage(percentage));
        }
    std::println();
    }

    // Format large numbers with locale-aware formatting
    static std::string format_large_number(size_t number) { 
        std::ostringstream oss; // String stream for number formatting
        oss.imbue(std::locale("")); // Use system locale for number formatting
        oss << number;
        return oss.str();
    }
    
    static std::string format_percentage(double percentage) { // Format percentage with fixed precision
        std::ostringstream oss; // String stream for number formatting
        oss << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%"; // Format with 1 decimal place
        return oss.str();
    }
    
    static std::string truncate(const std::string& str, size_t width) { // Truncate string with ellipsis
        if (str.length() > width) { // Check if truncation is needed
            return str.substr(0, width - 3) + "..."; // Add ellipsis for truncated strings
        }
        return str;
    }

};
