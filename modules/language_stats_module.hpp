#ifndef LANGUAGE_STATS_MODULE_HPP
#define LANGUAGE_STATS_MODULE_HPP

#include "statistics_module.hpp" // [C++11] Include base module
#include "language_stats.hpp"    // [C++11] Include language stats

class LanguageStatsModule : public CodeFetchModule { // [C++11] Derive from base module
private:
    LanguageStats stats;                             // [C++11] Statistics storage

public:
    void process_file(const fs::path& file_path) override; // [C++17] Process file implementation
    void print_stats() const override;                     // [C++11] Print statistics implementation
};

#endif // LANGUAGE_STATS_MODULE_HPP
