#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>
#include <chrono>
#include <atomic>
#include <iostream>
#include <algorithm>
#include <format>

#include <locale> // For thousands separators
#include <sstream> // For std::stringstream

// Function to format a number with thousands separators
std::string format_with_commas(size_t number) {
    std::stringstream ss;
    ss.imbue(std::locale("")); // Use the system's locale for thousands separators
    ss << number;
    return ss.str();
}


#include "git_statistics.hpp"
#include "../src/output_formatter.hpp"

std::atomic<bool> GitModule::interrupt_requested(false);  // Atomic flag for interrupting processing

GitModule::GitModule(size_t contributors_count) :  // Constructor with member initializers
                   contributors_count(contributors_count),
                   repo(nullptr),
                   commit_count(0)
{
    git_libgit2_init();  // Initialize libgit2 library
}

GitModule::~GitModule() {  // Destructor with resource cleanup
    if (repo) {
        git_repository_free(repo);
    }
    git_libgit2_shutdown();
}

void GitModule::process_file(const fs::path& file_path) {  // Process file using filesystem path
    if (!repo) {
        try {
            if (git_repository_open(&repo, file_path.parent_path().c_str()) != 0) {  // Open git repo from path
                return;
            }
            process_commits();               // Process all commits in repository
        } catch (const std::exception& e) {  // Exception handling
            std::cerr << "Error processing Git repository: " << e.what() << std::endl;
        }
    }
}

// Static callback function for commit processing
int GitModule::commit_callback(const git_commit* commit, void* payload) {
    GitModule* self = static_cast<GitModule*>(payload);  // Cast payload to GitModule pointer
    self->commit_count++;                                // Increment commit counter

    const git_signature* author = git_commit_author(commit); // Get commit author information
    std::string name(author->name);                      // Convert author name to string
    std::string email(author->email);                    // Convert author email to string
    self->contributor_commits[name][email]++;            // Update commit count for author

    git_time_t time = git_commit_time(commit);           // Get commit timestamp
    char time_str[20];                                   // Buffer for formatted time
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time));  // Format timestamp
    std::string commit_date(time_str);                   // Convert to string

    // Update first and last commit dates
    if (self->first_commit_date.empty() || commit_date < self->first_commit_date) {
        self->first_commit_date = commit_date;
    }
    if (self->last_commit_date.empty() || commit_date > self->last_commit_date) {
        self->last_commit_date = commit_date;
    }

    return 0;
}

void GitModule::process_commits() {                            // Process all commits in repository
    git_revwalk* walker;
    if (git_revwalk_new(&walker, repo) != 0) {                 // Create revision walker
        std::cerr << "Failed to create revision walker." << std::endl;
        return;
    }

    git_revwalk_push_head(walker);                             // Start from HEAD

    git_oid oid;
    size_t processed_commits = 0;                              // Track processed commits
    const size_t max_commits = 200000;                         // Maximum commits to process
    const auto start_time = std::chrono::steady_clock::now();  // Track processing time
    const auto max_duration = std::chrono::minutes(10);   // Maximum processing duration

    // Process commits with time and count limits
    while (git_revwalk_next(&oid, walker) == 0 && processed_commits < max_commits && !interrupt_requested.load()) {
        git_commit* commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            commit_callback(commit, this);
            git_commit_free(commit);
            processed_commits++;
        }

        if (processed_commits % 1000 == 0) {  // Progress check every 1000 commits
            auto current_time = std::chrono::steady_clock::now();  // Get current time
            if (current_time - start_time > max_duration) {        // Check time limit
                std::cout << "\nReached time limit. Stopping commit processing." << std::endl;
                break;
            }
        }
    }

    git_revwalk_free(walker);  // Free revision walker
}

void GitModule::interrupt() {            // Interrupt commit processing
    interrupt_requested.store(true);  // Set atomic interrupt flag
}

std::string GitModule::format_number(size_t number) const { // Format numbers with locale
    std::stringstream ss;                                   // String stream for formatting
    ss.imbue(std::locale(""));                      // Use system locale
    ss << std::fixed << number;                             // Format with fixed notation
    return ss.str();
}

// String truncation utility
std::string GitModule::truncate_string(const std::string& str, size_t width) const {
    if (str.length() > width) {                           // Check if truncation needed
        return str.substr(0, width - 3) + "...";  // Add ellipsis
    }
    return str;
}

// Print git statistics with formatting
void GitModule::print_stats() const {
    std::string total_result_str = std::format("{} (Overall Commits) | {} (First) / {} (Last)\n",
                                 OutputFormatter::format_large_number(commit_count),
                                 first_commit_date.substr(0, 10),
                                 last_commit_date.substr(0, 10));
    std::cout << "♦ Git Stats" << std::endl;
    std::cout << "╰─ " << total_result_str  << std::endl;

    // Collect and sort contributor statistics
    std::vector<std::tuple<std::string, double, size_t>> contributor_stats;
    // Structured binding
    for (const auto& [name, emails] : contributor_commits) {
        size_t total_commits = 0;
        // Structured binding
        for (const auto& [email, count] : emails) {
            total_commits += count;
        }
        double percentage = (static_cast<double>(total_commits) / commit_count) * 100.0;
        contributor_stats.emplace_back(name, percentage, total_commits);
    }

    // Sort contributors by commit count
    std::sort(contributor_stats.begin(), contributor_stats.end(),
              [](const auto& a, const auto& b) { return std::get<1>(a) > std::get<1>(b); });

    std::cout << "☺ Top Contributors" << std::endl;
    // Limit length of contributors list
    size_t top_contributors = std::min(contributor_stats.size(), static_cast<size_t>(contributors_count));
    double others_percentage = 0.0;
    size_t others_commits = 0;

    for (size_t i = 0; i < top_contributors; ++i) {
        const auto& [name, percentage, commits] = contributor_stats[i]; // Structured binding for tuple

        // Format name with truncation if needed
        std::string display_name = name;
        if (display_name.length() > 25) {
            display_name = display_name.substr(0, 22) + "...";
        }

        // Format commits with thousands separators
        std::stringstream ss;
        ss.imbue(std::locale(""));  // Use system locale for thousands separator
        ss << commits;
        std::string formatted_commits = ss.str();

        // Output the formatted line
        std::cout << std::format(
            "  ╰─ {:<25} : {:>6.1f}% ({:>7} commits)\n",
            display_name,      // Name (padded/truncated to 25 chars)
            percentage,             // Percentage with 1 decimal place
            formatted_commits  // Commits with thousands separators
        );
    }

    // Calculate statistics for remaining contributors
    for (size_t i = top_contributors; i < contributor_stats.size(); ++i) {
        // Structured binding
        const auto& [_, percentage, commits] = contributor_stats[i];
        others_percentage += percentage;
        others_commits += commits;
    }

    // Print "Others" category if there are remaining contributors
    if (contributor_stats.size() > top_contributors) {
        std::stringstream ss;  // Format Others line using the same format as individual contributors
        ss.imbue(std::locale("")); // Use system locale for thousands separator
        ss << others_commits;
        std::string formatted_others_commits = ss.str();

        std::cout << std::format(
            "\033[3m  ╰─ {:<25} : {:>6.1f}% ({:>7} commits)\033[0m\n", // italic
            "Others (total)",         // Fixed name
            others_percentage,        // Percentage with 1 decimal place
            formatted_others_commits  // Commits with thousands separators
        );
    }

    std::cout << std::endl;
}