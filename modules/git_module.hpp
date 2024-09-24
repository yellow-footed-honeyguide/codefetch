#ifndef GIT_MODULE_HPP
#define GIT_MODULE_HPP

#include "statistics_module.hpp"
#include <git2.h>
#include <string>
#include <vector>
#include <map>

class GitModule : public StatisticsModule {
private:
    git_repository* repo;
    std::string first_commit_date;
    std::string last_commit_date;
    size_t commit_count;
    std::map<std::string, size_t> contributor_commits;

    void process_commits();
    static int commit_callback(const git_commit* commit, void* payload);
    std::string format_number(size_t number) const;

public:
    GitModule();
    ~GitModule();

    void process_file(const fs::path& file_path) override;
    void print_stats() const override;
};

#endif // GIT_MODULE_HPP