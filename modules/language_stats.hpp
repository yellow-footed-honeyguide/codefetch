#ifndef LANGUAGE_STATS_HPP
#define LANGUAGE_STATS_HPP

#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class LanguageStats {
public:
    void add_file(const fs::path& file_path, size_t lines);
    void print_stats() const;
    std::vector<std::pair<std::string, size_t>> get_sorted_stats() const;
    size_t get_total_lines() const { return total_lines; }

private:
    std::unordered_map<std::string, size_t> language_lines;
    size_t total_lines = 0;

    std::string detect_language(const fs::path& file_path) const;
};

#endif // LANGUAGE_STATS_HPP
