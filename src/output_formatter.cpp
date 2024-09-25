#include "output_formatter.hpp"

void OutputFormatter::print_section(const std::string& title, const std::string& icon, const std::vector<std::pair<std::string, std::string>>& items) {
    std::cout << icon << " " << title << std::endl;
    for (const auto& [key, value] : items) {
        std::cout << "  ╰─ " << std::left << std::setw(10) << key << ": " << value << std::endl;
    }
    std::cout << std::endl;
}

void OutputFormatter::print_language_stats(const std::vector<std::pair<std::string, double>>& stats, size_t total_lines) {
    std::cout << "⚐ Languages" << std::endl;
    for (const auto& [language, percentage] : stats) {
        size_t lines = static_cast<size_t>(percentage * total_lines / 100);
        std::cout << "  ╰─ " << std::left << std::setw(10) << truncate(language, 10) 
                  << ": " << std::right << std::setw(5) << format_percentage(percentage) 
                  << "  (" << format_large_number(lines) << " lines)" << std::endl;
    }
    std::cout << std::endl;
}

void OutputFormatter::print_contributor_stats(const std::vector<std::pair<std::string, double>>& stats) {
    std::cout << "☺ Top Contributors" << std::endl;
    for (const auto& [name, percentage] : stats) {
        std::cout << "  ╰─ " << std::left << std::setw(14) << truncate(name, 14) 
                  << ": " << std::right << std::setw(6) << format_percentage(percentage) << std::endl;
    }
    std::cout << std::endl;
}

std::string OutputFormatter::format_percentage(double percentage) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    return oss.str();
}

std::string OutputFormatter::format_large_number(size_t number) {
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    oss << number;
    return oss.str();
}

std::string OutputFormatter::truncate(const std::string& str, size_t width) {
    if (str.length() > width) {
        return str.substr(0, width - 3) + "...";
    }
    return str;
}
