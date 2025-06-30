#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem; 

class LanguageStats {
public:
    void add_file(const fs::path& file_path, size_t lines); // Add file to statistics
    void print_stats() const;                               // Print language statistics
    std::vector<std::pair<std::string, size_t>> get_sorted_stats() const;  // Get sorted statistics
    size_t get_total_lines() const { return total_lines; }  // Getter for total lines

private:
    std::unordered_map<std::string, size_t> language_lines; // Hash map for language line counts
    size_t total_lines = 0;                                 // Total lines counter with in-class init

    std::string detect_language(const fs::path& file_path) const; // Detect file language
};
