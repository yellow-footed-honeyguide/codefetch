#ifndef GIT_MODULE_HPP
#define GIT_MODULE_HPP

#include <map>         // [C++98]
#include <string>      // [C++98]
#include <vector>      // [C++98]
#include <atomic>      // [C++11]

#include <git2.h>      // [External] libgit2 header
#include "statistics_module.hpp"

class GitModule : public CodeFetchModule {
private:
    git_repository* repo;                  // Git repository handle
    std::string first_commit_date;         // [C++11] Store first commit date
    std::string last_commit_date;          // [C++11] Store last commit date
    size_t commit_count;                   // [C++11] Total number of commits
    
    // [C++11] Nested map for contributor statistics: name -> email -> commit count
    std::map<std::string, std::map<std::string, size_t>> contributor_commits;

    static std::atomic<bool> interrupt_requested;   // [C++11] Atomic flag for interrupting processing

    void process_commits();                         // Process all commits in repository
    static int commit_callback(const git_commit* commit, void* payload);     // [C++11] Static callback for commit processing
    std::string format_number(size_t number) const; // [C++11] Format numbers with locale
    std::string truncate_string(const std::string& str, size_t width) const; // [C++11] Truncate long strings

public:
    GitModule();  // [C++11] Constructor
    ~GitModule(); // [C++11] Destructor with libgit cleanup

    void process_file(const fs::path& file_path) override; // [C++17] Process file implementation
    void print_stats() const override;                     // [C++11] Print statistics implementation
    static void interrupt();                               // [C++11] Interrupt processing method
};

#endif // GIT_MODULE_HPP