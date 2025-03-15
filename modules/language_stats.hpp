#pragma once

#include "codefetch_module_interface.hpp" // Include base module
#include "language_stats_lib.hpp"    // Include language stats

class LanguageStatsModule : public CodeFetchModule { // Derive from base module
private:
    LanguageStats stats;     // Statistics storage
    size_t languages_count;  // Number of supported languages in the printing statistics


public:
    LanguageStatsModule(size_t languages_count);
    void process_file(const fs::path& file_path) override;   // Process file implementation
    void print_stats() const override;  // Declaration for compatibility with the base module (unused)
    void print_stats(size_t languages_count) const;  // Implementation of print statistics with an argument
};

