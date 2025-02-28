#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>
#include <chrono>
#include <atomic>
#include <iostream>
#include <algorithm>

#include "git_module.hpp"
#include "../src/output_formatter.hpp"

std::atomic<bool> GitModule::interrupt_requested(false);  // Atomic flag for interrupting processing

GitModule::GitModule() : repo(nullptr), commit_count(0) {    // Constructor with member initializers
    git_libgit2_init();                                      // Initialize libgit2 library
}

GitModule::~GitModule() {                                    // Destructor with resource cleanup
    if (repo) {
        git_repository_free(repo);
    }
    git_libgit2_shutdown();
}

void GitModule::process_file(const fs::path& file_path) {    // Process file using filesystem path
    if (!repo) {
        try {
            if (git_repository_open(&repo, file_path.parent_path().c_str()) != 0) { // [C++17] Open git repo from path
                return;
            }
            process_commits();                               // Process all commits in repository
        } catch (const std::exception& e) {                  // Exception handling
            std::cerr << "Error processing Git repository: " << e.what() << std::endl;
        }
    }
}

// Static callback function for commit processing
int GitModule::commit_callback(const git_commit* commit, void* payload) {
    GitModule* self = static_cast<GitModule*>(payload);        // [C++11] Cast payload to GitModule pointer
    self->commit_count++;                                      // Increment commit counter

    const git_signature* author = git_commit_author(commit);   // Get commit author information
    std::string name(author->name);                            // [C++11] Convert author name to string
    std::string email(author->email);                          // [C++11] Convert author email to string
    self->contributor_commits[name][email]++;                  // Update commit count for author

    git_time_t time = git_commit_time(commit);                 // Get commit timestamp
    char time_str[20];                                         // Buffer for formatted time
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time));  // Format timestamp
    std::string commit_date(time_str);                         // [C++11] Convert to string

    // [C++11] Update first and last commit dates
    if (self->first_commit_date.empty() || commit_date < self->first_commit_date) {
        self->first_commit_date = commit_date;
    }
    if (self->last_commit_date.empty() || commit_date > self->last_commit_date) {
        self->last_commit_date = commit_date;
    }

    return 0;
}

void GitModule::process_commits() {                        // Process all commits in repository
    git_revwalk* walker;
    if (git_revwalk_new(&walker, repo) != 0) {             // Create revision walker
        std::cerr << "Failed to create revision walker." << std::endl;
        return;
    }

    git_revwalk_push_head(walker);                         // Start from HEAD

    git_oid oid;
    size_t processed_commits = 0;                          // [C++11] Track processed commits
    const size_t max_commits = 200000;                     // [C++11] Maximum commits to process
    const auto start_time = std::chrono::steady_clock::now();  // [C++11] Track processing time
    const auto max_duration = std::chrono::minutes(10);    // [C++11] Maximum processing duration

    // [C++11] Process commits with time and count limits
    while (git_revwalk_next(&oid, walker) == 0 && processed_commits < max_commits && !interrupt_requested.load()) {
        git_commit* commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            commit_callback(commit, this);
            git_commit_free(commit);
            processed_commits++;
        }

        if (processed_commits % 1000 == 0) {              // Progress check every 1000 commits
            auto current_time = std::chrono::steady_clock::now();  // [C++11] Get current time
            if (current_time - start_time > max_duration) {        // [C++11] Check time limit
                std::cout << "\nReached time limit. Stopping commit processing." << std::endl;
                break;
            }
        }
    }

    git_revwalk_free(walker);                             // Free revision walker
}

void GitModule::interrupt() {                             // [C++11] Interrupt commit processing
    interrupt_requested.store(true);                      // [C++11] Set atomic interrupt flag
}

std::string GitModule::format_number(size_t number) const { // [C++11] Format numbers with locale
    std::stringstream ss;                                   // [C++11] String stream for formatting
    ss.imbue(std::locale(""));                              // [C++11] Use system locale
    ss << std::fixed << number;                             // [C++11] Format with fixed notation
    return ss.str();
}

std::string GitModule::truncate_string(const std::string& str, size_t width) const {  // [C++11] String truncation utility
    if (str.length() > width) {                            // [C++11] Check if truncation needed
        return str.substr(0, width - 3) + "...";           // [C++11] Add ellipsis
    }
    return str;
}

// [C++11] Print git statistics with formatting
void GitModule::print_stats() const {
    std::vector<std::pair<std::string, std::string>> git_items = { // [C++11] Prepare stats items
        {"Commits", OutputFormatter::format_large_number(commit_count)},
        {"First", first_commit_date.substr(0, 10)},
        {"Last", last_commit_date.substr(0, 10)}
    };
    OutputFormatter::print_section("Git Stats", "♦", git_items);   // Print general statistics

    // [C++11] Collect and sort contributor statistics
    std::vector<std::tuple<std::string, double, size_t>> contributor_stats;
    for (const auto& [name, emails] : contributor_commits) {      // [C++17] Structured binding
        size_t total_commits = 0;
        for (const auto& [email, count] : emails) {               // [C++17] Structured binding
            total_commits += count;
        }
        double percentage = (static_cast<double>(total_commits) / commit_count) * 100.0;
        contributor_stats.emplace_back(name, percentage, total_commits);
    }

    // [C++11] Sort contributors by commit count
    std::sort(contributor_stats.begin(), contributor_stats.end(),
              [](const auto& a, const auto& b) { return std::get<1>(a) > std::get<1>(b); });

    std::cout << "☺ Top Contributors" << std::endl;

    size_t top_contributors = std::min(contributor_stats.size(), static_cast<size_t>(5)); // [C++11] Limit to top 5
    double others_percentage = 0.0;
    size_t others_commits = 0;

    // [C++11] Print top contributors
    for (size_t i = 0; i < top_contributors; ++i) {
        const auto& [name, percentage, commits] = contributor_stats[i]; // [C++17] Structured binding
        // [C++11] Format contributor statistics
        std::cout << "  ╰─ " << std::left << std::setw(20) << OutputFormatter::truncate(name, 20)
                  << ": " << std::right << std::setw(5) << OutputFormatter::format_percentage(percentage)
                  << "  (" << OutputFormatter::format_large_number(commits) << " commits)" << std::endl;
    }

    // [C++11] Calculate statistics for remaining contributors
    for (size_t i = top_contributors; i < contributor_stats.size(); ++i) {
        const auto& [_, percentage, commits] = contributor_stats[i]; // [C++17] Structured binding
        others_percentage += percentage;
        others_commits += commits;
    }

    if (contributor_stats.size() > 5) { // Print "Others" category if needed
        // [C++11] Format "Others" statistics
        std::cout << "  ╰─ " << std::left << std::setw(20) << "Others"
                  << ": " << std::right << std::setw(5) << OutputFormatter::format_percentage(others_percentage)
                  << "  (" << OutputFormatter::format_large_number(others_commits) << " commits)" << std::endl;
    }

    std::cout << std::endl;
}