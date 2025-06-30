#pragma once  

#include <string>      
#include <vector>     

#include "codefetch_module_interface.hpp"  // Base class interface

// GitModule implements code fetching functionality using Git
class GitModule : public CodeFetchModule {
private:
    size_t contributors_count;  // Stores number of contributors to track
    
    // Executes shell command and returns its output
    std::string exec_command(const std::string& cmd) const;
    
public:
    // Constructor taking number of contributors as parameter
    GitModule(size_t contributors_count);
    
    // Default destructor (no special cleanup needed)
    ~GitModule() = default;

    // Processes a single file at given path (override from base class)
    void process_file(const fs::path& file_path) override;
    
    // Prints collected statistics (override from base class)
    void print_stats() const override;
    
    // Empty static interrupt handler (can be called without instance)
    static void interrupt() {};
};
