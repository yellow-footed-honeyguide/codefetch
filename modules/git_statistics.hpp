#pragma once

#include <map>
#include <string>
#include <atomic>

#include <git2.h>
#include "codefetch_module_interface.hpp"

class GitModule : public CodeFetchModule {
private:
    git_repository* repo;           // Git repository handle
    std::string first_commit_date;  // Store first commit date
    std::string last_commit_date;   // Store last commit date
    size_t commit_count;            // Total number of commits
    size_t contributors_count;      // Number of contributors displayed in Git statistics
    
    //  Nested map for contributor statistics: name -> email -> commit count
    std::map<std::string, std::map<std::string, size_t>> contributor_commits;

    static std::atomic<bool> interrupt_requested;  // Atomic flag for interrupting processing

    void process_commits();  // Process all commits in repository
    static int commit_callback(const git_commit* commit, void* payload);  // Static callback for commit processing
    std::string format_number(size_t number) const; // Format numbers with locale
    std::string truncate_string(const std::string& str, size_t width) const; // Truncate long strings

public:
    GitModule(size_t contributors_count);
    ~GitModule();  // Destructor with libgit cleanup

    void process_file(const fs::path& file_path) override;  // Process file implementation
    void print_stats() const override;  // If the module uses -g, display 20 contributors instead of 3
    static void interrupt();  // Interrupt processing method
};

