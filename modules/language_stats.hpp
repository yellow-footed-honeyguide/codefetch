#ifndef LANGUAGE_STATS_HPP
#define LANGUAGE_STATS_HPP

#include <string>               // [C++98]
#include <vector>               // [C++98]
#include <unordered_map>        // [C++11]
#include <filesystem>           // [C++17]

namespace fs = std::filesystem; // [C++17] Namespace alias for filesystem

class LanguageStats {
public:
    void add_file(const fs::path& file_path, size_t lines); // [C++17] Add file to statistics
    void print_stats() const;                               // [C++11] Print language statistics
    std::vector<std::pair<std::string, size_t>> get_sorted_stats() const;  // [C++11] Get sorted statistics
    size_t get_total_lines() const { return total_lines; }  // [C++11] Getter for total lines

private:
    std::unordered_map<std::string, size_t> language_lines; // [C++11] Hash map for language line counts
    size_t total_lines = 0;                                 // [C++11] Total lines counter with in-class init

    std::string detect_language(const fs::path& file_path) const; // [C++17] Detect file language
};

#endif // LANGUAGE_STATS_HPP