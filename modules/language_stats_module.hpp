#ifndef LANGUAGE_STATS_MODULE_HPP
#define LANGUAGE_STATS_MODULE_HPP

#include "statistics_module.hpp"
#include "language_stats.hpp"

class LanguageStatsModule : public CodeFetchModule {
private:
    LanguageStats stats;

public:
    void process_file(const fs::path& file_path) override;
    void print_stats() const override;
};

#endif // LANGUAGE_STATS_MODULE_HPP
