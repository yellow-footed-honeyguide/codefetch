#pragma once

#include <map>
#include <string>
#include <atomic>

#include <git2.h>
#include "statistics_module.hpp"

class GitModule : public CodeFetchModule {
private:
    git_repository* repo;           // Git repository handle
    std::string first_commit_date;  //  Store first commit date
    std::string last_commit_date;   // Store last commit date
    size_t commit_count;            // Total number of commits
    
    //  Nested map for contributor statistics: name -> email -> commit count
    std::map<std::string, std::map<std::string, size_t>> contributor_commits;

    static std::atomic<bool> interrupt_requested;  // Atomic flag for interrupting processing

    void process_commits();  // Process all commits in repository
    static int commit_callback(const git_commit* commit, void* payload);  // Static callback for commit processing
    std::string format_number(size_t number) const; // Format numbers with locale
    std::string truncate_string(const std::string& str, size_t width) const; // Truncate long strings

public:
    GitModule();  // Constructor
    ~GitModule(); // Destructor with libgit cleanup

    void process_file(const fs::path& file_path) override; // Process file implementation
    void print_stats() const override;                     // Print statistics implementation
    static void interrupt();                               // Interrupt processing method
};

