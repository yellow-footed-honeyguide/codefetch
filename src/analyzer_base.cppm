module;  
#include <filesystem> 
export module analyzer_base;

export class CodeFetchModule { // Abstract base class for statistics modules
public:
    virtual ~CodeFetchModule() = default; // Virtual destructor with default implementation
    virtual void process_file(const std::filesystem::path& file_path) = 0; // Pure virtual method for file processing
    virtual void print_stats() const = 0; // Pure virtual method for stats printing
};

