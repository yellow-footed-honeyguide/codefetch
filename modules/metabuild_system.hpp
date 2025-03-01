#pragma once

#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include "codefetch_module_interface.hpp"

class MetabuildSystemModule : public CodeFetchModule {
private:
    std::unordered_set<std::string> detected_systems; // Store unique detected build systems

    // Map of build system patterns using regex
    std::unordered_map<std::string, std::regex> build_system_patterns = {
        {"CMake", std::regex("(^|/)CMakeLists\\.txt$")},
        {"Meson", std::regex("(^|/)meson\\.build$")},
        {"Autotools", std::regex("(^|/)configure\\.ac$")},
        {"Bazel", std::regex("(^|/)(BUILD|\\.bazel|\\.bazelrc|\\.bazelversion|WORKSPACE)$")}
    };

public:
    void process_file(const fs::path& file_path) override; // Process file implementation
    void print_stats() const override;                     // Print statistics implementation
    bool has_detected_systems() const { return !detected_systems.empty(); }  // Check if systems were detected
};

