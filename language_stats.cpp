#include "language_stats.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

void LanguageStats::add_file(const fs::path& file_path, size_t lines) {
    std::string language = detect_language(file_path);
    language_lines[language] += lines;
    total_lines += lines;
}

void LanguageStats::print_stats() const {
    std::cout << "\nLanguages:\n";

    std::vector<std::pair<std::string, size_t>> sorted_stats = get_sorted_stats();

    for (const auto& [language, lines] : sorted_stats) {
        double percentage = (static_cast<double>(lines) / total_lines) * 100.0;
        if (percentage >= 1.0) {
            std::cout << std::setw(20) << std::left << language
                      << std::setw(10) << std::right << std::fixed << std::setprecision(1) << percentage << "%\n";
        }
    }
}

std::vector<std::pair<std::string, size_t>> LanguageStats::get_sorted_stats() const {
    std::vector<std::pair<std::string, size_t>> sorted_stats(language_lines.begin(), language_lines.end());
    std::sort(sorted_stats.begin(), sorted_stats.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    return sorted_stats;
}

std::string LanguageStats::detect_language(const fs::path& file_path) const {
    static const std::unordered_map<std::string, std::string> extension_to_language = {
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

    std::string ext = file_path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = extension_to_language.find(ext);
    if (it != extension_to_language.end()) {
        return it->second;
    }
    return "Other";
}
