#include <sstream>
#include <iostream>
#include "output_formatter.hpp"

// Print formatted section with title and items using modern string collections
void OutputFormatter::print_section(const std::string& title, const std::string& icon, 
    const std::vector<std::pair<std::string, std::string>>& items) {
    std::cout << icon << " " << title << std::endl;  // Print section header with icon
    for (const auto& [key, value] : items) {  // Structured binding for pair iteration
        // Format output with alignment
        std::cout << "  ╰─ " << std::left << std::setw(10) << key << ": " << value << std::endl; 
    }
    std::cout << std::endl;
}

// Print language statistics with distribution percentages
void OutputFormatter::print_language_stats(const std::vector<std::pair<std::string, double>>& stats, 
    size_t total_lines) {
    std::cout << "⚐ Lines by Language  [incl: Code, Comments, Blanks]" << std::endl;
    for (const auto& [language, percentage] : stats) {  // Structured binding for stats pairs
        // Calculate absolute lines count
        size_t lines = static_cast<size_t>(percentage * total_lines / 100); 
        // Format output with proper alignment and number formatting
        std::cout << "  ╰─ " << std::left << std::setw(16) << truncate(language, 15)   
                  << ": " << std::right << std::setw(5) << format_percentage(percentage) 
                  << "  (" << format_large_number(lines) << " lines)" << std::endl;


    }
    std::cout << std::endl;
}

void OutputFormatter::print_contributor_stats(
    // Print contributor stats with percentages
    const std::vector<std::pair<std::string, double>>& stats) { 
    std::cout << "☺ Top Contributors" << std::endl;
    // Structured binding for contributor data
    for (const auto& [name, percentage] : stats) {  
        // Format contributor info with proper alignment
        std::cout << "  ╰─ " << std::left << std::setw(14) << truncate(name, 14)   
                  << ": " << std::right << std::setw(6) << format_percentage(percentage) << std::endl;
    }
    std::cout << std::endl;
}

// Format percentage with fixed precision
std::string OutputFormatter::format_percentage(double percentage) { 
    std::ostringstream oss;  // String stream for number formatting

    // Format with 1 decimal place
    oss << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%"; 
    return oss.str();
}

// Format large numbers with locale-aware formatting
std::string OutputFormatter::format_large_number(size_t number) {  
    std::ostringstream oss;      // String stream for number formatting
    oss.imbue(std::locale(""));  // Use system locale for number formatting
    oss << number;
    return oss.str();
}

// Truncate string with ellipsis
std::string OutputFormatter::truncate(const std::string& str, size_t width) {  
    if (str.length() > width) {  // Check if truncation is needed
        return str.substr(0, width - 3) + "...";  // Add ellipsis for truncated strings
    }
    return str;
}
