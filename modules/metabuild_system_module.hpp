#ifndef METABUILD_SYSTEM_MODULE_HPP
#define METABUILD_SYSTEM_MODULE_HPP

#include <string>             // [C++98]
#include <regex>              // [C++11]
#include <unordered_map>      // [C++11]
#include <unordered_set>      // [C++11]
#include "statistics_module.hpp"

class MetabuildSystemModule : public CodeFetchModule {
private:
    std::unordered_set<std::string> detected_systems; // [C++11] Store unique detected build systems

    // [C++11] Map of build system patterns using regex
    std::unordered_map<std::string, std::regex> build_system_patterns = {
        {"CMake", std::regex("(^|/)CMakeLists\\.txt$")},
        {"Meson", std::regex("(^|/)meson\\.build$")},
        {"Autotools", std::regex("(^|/)configure\\.ac$")},
        {"Bazel", std::regex("(^|/)(BUILD|\\.bazel|\\.bazelrc|\\.bazelversion|WORKSPACE)$")}
    };

public:
    void process_file(const fs::path& file_path) override; // [C++17] Process file implementation
    void print_stats() const override;                     // [C++11] Print statistics implementation
    bool has_detected_systems() const { return !detected_systems.empty(); }  // [C++11] Check if systems were detected
};

#endif // METABUILD_SYSTEM_MODULE_HPP