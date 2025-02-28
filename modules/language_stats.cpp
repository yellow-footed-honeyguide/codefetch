#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

#include "language_stats.hpp"

void LanguageStats::add_file(const fs::path& file_path, size_t lines) {  // Add file with line count to stats
    std::string language = detect_language(file_path);  // Get language from file extension
    language_lines[language] += lines;                  // Update language line count
    total_lines += lines;                               // Update total lines
}

void LanguageStats::print_stats() const {               // Print language distribution
    std::cout << "\nLanguages:\n";

    std::vector<std::pair<std::string, size_t>> sorted_stats = get_sorted_stats(); // Get sorted statistics

    for (const auto& [language, lines] : sorted_stats) {  // Structured binding for stat pairs
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;  // Calculate percentage
        if (percentage >= 1.0) {                                         // Only show languages with ≥1% share
            // Format output with aligned columns
            std::cout << std::setw(20) << std::left << language
                      << std::setw(10) << std::right << std::fixed << std::setprecision(1) << percentage << "%\n";
        }
    }
}

// [C++11] Get sorted statistics for languages
std::vector<std::pair<std::string, size_t>> LanguageStats::get_sorted_stats() const {
    // [C++11] Copy to vector
    std::vector<std::pair<std::string, size_t>> sorted_stats(language_lines.begin(), language_lines.end());  
    // [C++11] Sort by line count using lambda
    std::sort(sorted_stats.begin(), sorted_stats.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return sorted_stats;
}

std::string LanguageStats::detect_language(const fs::path& file_path) const { // [C++17] Detect language from file extension
    static const std::unordered_map<std::string, std::string> extension_to_language = { // [C++11] Static extension mapping
        {".c", "C"}, {".cpp", "C++"}, {".cxx", "C++"}, {".cc", "C++"}, {".h", "C/C++ Header"},
        {".hpp", "C++ Header"}, {".hxx", "C++ Header"}, {".py", "Python"}, {".java", "Java"},
        {".js", "JavaScript"}, {".ts", "TypeScript"}, {".vim", "Vim script"}, {".pl", "Perl"},
        {".sh", "Shell"}, {".bash", "Shell"}, {".yaml", "YAML"}, {".yml", "YAML"},
        {".md", "Markdown"}, {".markdown", "Markdown"}, {".lua", "Lua"}, {".cs", "C#"},
        {".json", "JSON"}, {".xml", "XML"}, {".html", "HTML"}, {".htm", "HTML"},
        {".css", "CSS"}, {".rb", "Ruby"}, {".go", "Go"}, {".rs", "Rust"}, {".php", "PHP"},
        {".swift", "Swift"}, {".kt", "Kotlin"}, {".scala", "Scala"}, {".elm", "Elm"},
        {".erl", "Erlang"}, {".hs", "Haskell"}, {".ml", "OCaml"}, {".f90", "Fortran"},
        {".jl", "Julia"}, {".m", "MATLAB"}, {".R", "R"}, {".sql", "SQL"}
    };

    std::string ext = file_path.extension().string();               // [C++17] Get file extension
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); // [C++11] Convert to lowercase

    auto it = extension_to_language.find(ext);                      // [C++11] Look up language
    if (it != extension_to_language.end()) {                        // Return mapped language or "Other"
        return it->second;
    }
    return "Other";
}