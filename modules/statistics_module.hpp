#ifndef STATISTICS_MODULE_HPP
#define STATISTICS_MODULE_HPP

#include <filesystem>

namespace fs = std::filesystem;

class StatisticsModule {
public:
    virtual ~StatisticsModule() = default;
    virtual void process_file(const fs::path& file_path) = 0;
    virtual void print_stats() const = 0;
};

#endif // STATISTICS_MODULE_HPP

