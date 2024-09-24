#include "git_module.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>

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
        git_repository_open(&repo, file_path.parent_path().c_str());
        if (repo) {
            process_commits();
        }
    }
}

int GitModule::commit_callback(const git_commit* commit, void* payload) {
    GitModule* self = static_cast<GitModule*>(payload);
    self->commit_count++;

    const git_signature* author = git_commit_author(commit);
    std::string email(author->email);
    self->contributor_commits[email]++;

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
    git_revwalk_new(&walker, repo);
    git_revwalk_push_head(walker);

    git_oid oid;
    while (git_revwalk_next(&oid, walker) == 0) {
        git_commit* commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            commit_callback(commit, this);
            git_commit_free(commit);
        }
    }

    git_revwalk_free(walker);
}

std::string GitModule::format_number(size_t number) const {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}

void GitModule::print_stats() const {
    std::cout << "\n\033[1;34mGIT\033[0m\n";
    std::cout << "Total number: " << format_number(commit_count) << "\n";
    std::cout << "First / Last commit:  " << first_commit_date << " / " << last_commit_date << "\n\n";
    std::cout << "Contributors\n";

    std::vector<std::pair<std::string, size_t>> sorted_contributors(contributor_commits.begin(), contributor_commits.end());
    std::sort(sorted_contributors.begin(), sorted_contributors.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    size_t total_commits = commit_count;

    for (const auto& [email, commits] : sorted_contributors) {
        double percentage = (static_cast<double>(commits) / total_commits) * 100.0;
        std::cout << "<" << email << "> "
                  << std::fixed << std::setprecision(1) << percentage << "%\n";
    }
}
