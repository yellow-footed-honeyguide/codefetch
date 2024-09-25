#include "git_module.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>
#include <numeric>
#include <chrono>
#include <atomic>
#include <signal.h>
#include "../src/output_formatter.hpp"

std::atomic<bool> GitModule::interrupt_requested(false);

GitModule::GitModule() : repo(nullptr), commit_count(0) {
    git_libgit2_init();
}

GitModule::~GitModule() {
    if (repo) {
        git_repository_free(repo);
    }
    git_libgit2_shutdown();
}

void GitModule::process_file(const fs::path& file_path) {
    if (!repo) {
        try {
            if (git_repository_open(&repo, file_path.parent_path().c_str()) != 0) {
                //std::cerr << "Failed to open Git repository." << std::endl;
                return;
            }
            process_commits();
        } catch (const std::exception& e) {
            std::cerr << "Error processing Git repository: " << e.what() << std::endl;
        }
    }
}

int GitModule::commit_callback(const git_commit* commit, void* payload) {
    GitModule* self = static_cast<GitModule*>(payload);
    self->commit_count++;

    const git_signature* author = git_commit_author(commit);
    std::string name(author->name);
    std::string email(author->email);
    self->contributor_commits[name][email]++;

    git_time_t time = git_commit_time(commit);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&time));
    std::string commit_date(time_str);

    if (self->first_commit_date.empty() || commit_date < self->first_commit_date) {
        self->first_commit_date = commit_date;
    }
    if (self->last_commit_date.empty() || commit_date > self->last_commit_date) {
        self->last_commit_date = commit_date;
    }

    return 0;
}

void GitModule::process_commits() {
    git_revwalk* walker;
    if (git_revwalk_new(&walker, repo) != 0) {
        std::cerr << "Failed to create revision walker." << std::endl;
        return;
    }

    git_revwalk_push_head(walker);

    git_oid oid;
    size_t processed_commits = 0;
    const size_t max_commits = 200000; // Увеличено до 200,000 коммитов
    const auto start_time = std::chrono::steady_clock::now();
    const auto max_duration = std::chrono::minutes(10); // Увеличено до 10 минут

    while (git_revwalk_next(&oid, walker) == 0 && processed_commits < max_commits && !interrupt_requested.load()) {
        git_commit* commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            commit_callback(commit, this);
            git_commit_free(commit);
            processed_commits++;
        }

        if (processed_commits % 1000 == 0) {
            //std::cout << "\rProcessed " << processed_commits << " commits..." << std::flush;

            auto current_time = std::chrono::steady_clock::now();
            if (current_time - start_time > max_duration) {
                std::cout << "\nReached time limit. Stopping commit processing." << std::endl;
                break;
            }
        }
    }

    //std::cout << "\rProcessed " << processed_commits << " commits. Done.           " << std::endl;

    git_revwalk_free(walker);
}

void GitModule::interrupt() {
    interrupt_requested.store(true);
}

std::string GitModule::format_number(size_t number) const {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}

std::string GitModule::truncate_string(const std::string& str, size_t width) const {
    if (str.length() > width) {
        return str.substr(0, width - 3) + "...";
    }
    return str;
}

void GitModule::print_stats() const {
    std::vector<std::pair<std::string, std::string>> git_items = {
        {"Commits", OutputFormatter::format_large_number(commit_count)},
        {"First", first_commit_date.substr(0, 10)},
        {"Last", last_commit_date.substr(0, 10)}
    };
    OutputFormatter::print_section("Git Stats", "♦", git_items);

    std::vector<std::pair<std::string, double>> contributor_stats;
    for (const auto& [name, emails] : contributor_commits) {
        size_t total_commits = 0;
        for (const auto& [email, count] : emails) {
            total_commits += count;
        }
        double percentage = (static_cast<double>(total_commits) / commit_count) * 100.0;
        contributor_stats.emplace_back(name, percentage);
    }

    std::sort(contributor_stats.begin(), contributor_stats.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    if (contributor_stats.size() > 3) {
        double others_percentage = 0;
        for (size_t i = 3; i < contributor_stats.size(); ++i) {
            others_percentage += contributor_stats[i].second;
        }
        contributor_stats.resize(3);
        contributor_stats.emplace_back("Others", others_percentage);
    }

    OutputFormatter::print_contributor_stats(contributor_stats);
}
