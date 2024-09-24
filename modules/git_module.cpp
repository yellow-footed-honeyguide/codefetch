#include "git_module.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>

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
    std::string name(author->name);
    self->contributor_commits[name]++;

    git_time_t time = git_commit_time(commit);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%c", localtime(&time));

    if (self->commit_count == 1) {
        self->first_commit_date = time_str;
    }
    self->last_commit_date = time_str;

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

void GitModule::print_stats() const {
    std::cout << "\n\033[1;34mGit\033[0m\n";
    std::cout << "First/Last commit:              " << first_commit_date << "/" << last_commit_date << "\n";
    std::cout << "Number of commits:              " << commit_count << "\n";
    std::cout << "Major contributors:\n";

    std::vector<std::pair<std::string, size_t>> sorted_contributors(contributor_commits.begin(), contributor_commits.end());
    std::sort(sorted_contributors.begin(), sorted_contributors.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    size_t total_commits = commit_count;
    size_t other_commits = total_commits;
    size_t displayed_contributors = 0;

    for (const auto& [name, commits] : sorted_contributors) {
        if (displayed_contributors < 5) {
            double percentage = (static_cast<double>(commits) / total_commits) * 100.0;
            std::cout << std::left << std::setw(30) << name
                      << std::right << std::fixed << std::setprecision(1) << percentage << "%\n";
            other_commits -= commits;
            displayed_contributors++;
        }
    }

    if (displayed_contributors == 5 && other_commits > 0) {
        double other_percentage = (static_cast<double>(other_commits) / total_commits) * 100.0;
        std::cout << std::left << std::setw(30) << "Others"
                  << std::right << std::fixed << std::setprecision(1) << other_percentage << "%\n";
    }
}
