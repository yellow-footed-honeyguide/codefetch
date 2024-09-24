#ifndef METABUILD_SYSTEM_MODULE_HPP
#define METABUILD_SYSTEM_MODULE_HPP

#include "statistics_module.hpp"
#include <unordered_set>
#include <string>
#include <regex>
#include <unordered_map>

class MetabuildSystemModule : public StatisticsModule {
private:
    std::unordered_set<std::string> detected_systems;
    std::unordered_map<std::string, std::regex> build_system_patterns = {
        {"CMake", std::regex("(^|/)CMakeLists\\.txt$")},
        {"Meson", std::regex("(^|/)meson\\.build$")},
        {"Autotools", std::regex("(^|/)configure\\.ac$")},
        {"Bazel", std::regex("(^|/)(BUILD|\\.bazel|\\.bazelrc|\\.bazelversion|WORKSPACE)$")}
    };

public:
    void process_file(const fs::path& file_path) override;
    void print_stats() const override;
    bool has_detected_systems() const { return !detected_systems.empty(); }
};

#endif // METABUILD_SYSTEM_MODULE_HPP
