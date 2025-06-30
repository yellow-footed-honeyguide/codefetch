#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>

#include "language_stats_lib.hpp"
#include "file_extension_to_language_map.hpp"

// Add file with line count to stats
void LanguageStats::add_file(const fs::path& file_path, size_t lines) {  
    std::string language = detect_language(file_path);  // Get language from file extension
    language_lines[language] += lines;                  // Update language line count
    total_lines += lines;                               // Update total lines
}

void LanguageStats::print_stats() const {               // Print language distribution
    std::cout << "\nLanguages:\n";

    // Get sorted statistics
    std::vector<std::pair<std::string, size_t>> sorted_stats = get_sorted_stats(); 

    for (const auto& [language, lines] : sorted_stats) {  // Structured binding for stat pairs
        // Calculate percentage
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;  
        if (percentage >= 1.0) {  // Only show languages with ≥1% share
            // Format output with aligned columns
            std::cout << std::setw(20) << std::left << language
                      << std::setw(10) << std::right << std::fixed << std::setprecision(1) << percentage << "%\n";
        }
    }
}

// Get sorted statistics for languages
std::vector<std::pair<std::string, size_t>> LanguageStats::get_sorted_stats() const {
    // Copy to vector
    std::vector<std::pair<std::string, size_t>> sorted_stats(language_lines.begin(), language_lines.end());  
    // Sort by line count using lambda
    std::sort(sorted_stats.begin(), sorted_stats.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return sorted_stats;
}

// Detect language from file extension
std::string LanguageStats::detect_language(const fs::path& file_path) const { 
   
    std::string ext = file_path.extension().string();  // Get file extension
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); // Convert to lowercase

    auto it = extension_to_language.find(ext);  // Look up language
    if (it != extension_to_language.end()) {    // Return mapped language or "Other"
        return it->second.name;
    }
    return "Other";
}
