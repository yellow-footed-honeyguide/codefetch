#ifndef CODEFETCH_MODULE_HPP
#define CODEFETCH_MODULE_HPP

#include <filesystem> // [C++17]

namespace fs = std::filesystem; // [C++17] Namespace alias for filesystem

class CodeFetchModule {                                       // [C++11] Abstract base class for statistics modules
public:
    virtual ~CodeFetchModule() = default;                     // [C++11] Virtual destructor with default implementation
    virtual void process_file(const fs::path& file_path) = 0; // [C++17] Pure virtual method for file processing
    virtual void print_stats() const = 0;                     // [C++11] Pure virtual method for stats printing
};

#endif // CODEFETCH_MODULE_HPP
