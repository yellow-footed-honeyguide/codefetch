#ifndef CODEFETCH_MODULE_HPP
#define CODEFETCH_MODULE_HPP

#include <filesystem>

namespace fs = std::filesystem;

class CodeFetchModule {
public:
    virtual ~CodeFetchModule() = default;
    virtual void process_file(const fs::path& file_path) = 0;
    virtual void print_stats() const = 0;
};

#endif // CODEFETCH_MODULE_HPP

