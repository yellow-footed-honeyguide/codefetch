#include "git_module.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <regex>
#include <numeric>
#include <tabulate/table.hpp>

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
    git_revwalk_new(&walker, repo);
    git_revwalk_push_head(walker);

    git_oid oid;
    size_t processed_commits = 0;
    const size_t max_commits = 100000; // Ограничение на количество обрабатываемых коммитов

    while (git_revwalk_next(&oid, walker) == 0 && processed_commits < max_commits) {
        git_commit* commit;
        if (git_commit_lookup(&commit, repo, &oid) == 0) {
            commit_callback(commit, this);
            git_commit_free(commit);
            processed_commits++;
        }

        // Добавляем проверку на каждые 1000 коммитов
        if (processed_commits % 1000 == 0) {
            std::cout << "\rProcessed " << processed_commits << " commits..." << std::flush;
        }
    }

    std::cout << "\rProcessed " << processed_commits << " commits. Done.           " << std::endl;

    git_revwalk_free(walker);
}

std::string GitModule::format_number(size_t number) const {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << number;
    return ss.str();
}

void GitModule::print_stats() const {
    tabulate::Table git_table;

    git_table.add_row({"Commits", format_number(commit_count)});
    git_table.add_row({"First commit", first_commit_date});
    git_table.add_row({"Last commit", last_commit_date});

    git_table.format()
        .font_style({tabulate::FontStyle::bold})
        .border_top(" ")
        .border_bottom(" ")
        .border_left(" ")
        .border_right(" ")
        .corner(" ");

    std::cout << "\n\033[1;34mGIT\033[0m\n";
    std::cout << git_table << "\n";

    tabulate::Table contributors_table;
    contributors_table.add_row({"Name", "Email", "Commits", "Percentage"});

    std::vector<std::tuple<std::string, std::string, size_t>> sorted_contributors;
    for (const auto& [name, emails] : contributor_commits) {
        for (const auto& [email, count] : emails) {
            sorted_contributors.emplace_back(name, email, count);
        }
    }

    std::sort(sorted_contributors.begin(), sorted_contributors.end(),
              [](const auto& a, const auto& b) { return std::get<2>(a) > std::get<2>(b); });

    size_t total_commits = commit_count;
    size_t other_commits = 0;
    size_t displayed_contributors = 0;

    for (const auto& [name, email, commits] : sorted_contributors) {
        if (displayed_contributors < 5) {
            double percentage = (static_cast<double>(commits) / total_commits) * 100.0;
            contributors_table.add_row({
                name,
                email,
                format_number(commits),
                std::to_string(percentage).substr(0, 5) + "%"
            });
            displayed_contributors++;
        } else {
            other_commits += commits;
        }

        if (displayed_contributors >= 5 && other_commits > 0) {
            break;
        }
    }

    if (other_commits > 0) {
        double other_percentage = (static_cast<double>(other_commits) / total_commits) * 100.0;
        contributors_table.add_row({
            "Others",
            "",
            format_number(other_commits),
            std::to_string(other_percentage).substr(0, 5) + "%"
        });
    }

    contributors_table.format()
        .font_style({tabulate::FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    std::cout << "Contributors\n";
    std::cout << contributors_table << "\n";
}
