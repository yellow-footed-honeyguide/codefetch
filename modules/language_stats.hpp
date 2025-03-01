#pragma once

#include "codefetch_module_interface.hpp" // Include base module
#include "language_stats_lib.hpp"    // Include language stats

class LanguageStatsModule : public CodeFetchModule { // Derive from base module
private:
    LanguageStats stats;                             // tatistics storage

public:
    void process_file(const fs::path& file_path) override; // Process file implementation
    void print_stats() const override;                     // Print statistics implementation
};

