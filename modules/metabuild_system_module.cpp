#include "metabuild_system_module.hpp"
#include <iostream>
#include <algorithm>
#include "../src/output_formatter.hpp"

void MetabuildSystemModule::process_file(const fs::path& file_path) {
    std::string full_path = fs::canonical(file_path).string();
    std::replace(full_path.begin(), full_path.end(), '\\', '/'); // Normalize path separators

    for (const auto& [system, pattern] : build_system_patterns) {
        if (std::regex_search(full_path, pattern)) {
            detected_systems.insert(system);
        }
    }
}

void MetabuildSystemModule::print_stats() const {
    if (!detected_systems.empty()) {
        std::vector<std::pair<std::string, std::string>> items = {
            {"Build", *detected_systems.begin()}
        };
        OutputFormatter::print_section("Build System", "⚒", items);
    }
}

