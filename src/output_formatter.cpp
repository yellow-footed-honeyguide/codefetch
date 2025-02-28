#include <iostream>
#include "output_formatter.hpp"

// Print formatted section with title and items using modern string collections
void OutputFormatter::print_section(const std::string& title, const std::string& icon, 
    const std::vector<std::pair<std::string, std::string>>& items) {
    std::cout << icon << " " << title << std::endl;                  // [C++11] Print section header with icon
    for (const auto& [key, value] : items) {                         // [C++17] Structured binding for pair iteration
        // Format output with alignment
        std::cout << "  ╰─ " << std::left << std::setw(10) << key << ": " << value << std::endl; 
    }
    std::cout << std::endl;
}

// Print language statistics with distribution percentages
void OutputFormatter::print_language_stats(const std::vector<std::pair<std::string, double>>& stats, 
    size_t total_lines) {
    std::cout << "⚐ Languages" << std::endl;
    for (const auto& [language, percentage] : stats) {               // [C++17] Structured binding for stats pairs
        size_t lines = static_cast<size_t>(percentage * total_lines / 100); // [C++11] Calculate absolute lines count
        // Format output with proper alignment and number formatting
        std::cout << "  ╰─ " << std::left << std::setw(10) << truncate(language, 10)   
                  << ": " << std::right << std::setw(5) << format_percentage(percentage) 
                  << "  (" << format_large_number(lines) << " lines)" << std::endl;
    }
    std::cout << std::endl;
}

void OutputFormatter::print_contributor_stats(
    const std::vector<std::pair<std::string, double>>& stats) { // Print contributor stats with percentages
    std::cout << "☺ Top Contributors" << std::endl;
    for (const auto& [name, percentage] : stats) {              // [C++17] Structured binding for contributor data
        // Format contributor info with proper alignment
        std::cout << "  ╰─ " << std::left << std::setw(14) << truncate(name, 14)   
                  << ": " << std::right << std::setw(6) << format_percentage(percentage) << std::endl;
    }
    std::cout << std::endl;
}

std::string OutputFormatter::format_percentage(double percentage) { // Format percentage with fixed precision
    std::ostringstream oss;                                         // String stream for number formatting
    oss << std::fixed << std::setprecision(1) << std::setw(5) << percentage << "%"; // Format with 1 decimal place
    return oss.str();
}

std::string OutputFormatter::format_large_number(size_t number) {  // Format large numbers with locale-aware formatting
    std::ostringstream oss;                                        // String stream for number formatting
    oss.imbue(std::locale(""));                            // Use system locale for number formatting
    oss << number;
    return oss.str();
}

std::string OutputFormatter::truncate(const std::string& str, size_t width) {  // Truncate string with ellipsis
    if (str.length() > width) {                                    // Check if truncation is needed
        return str.substr(0, width - 3) + "...";           // Add ellipsis for truncated strings
    }
    return str;
}