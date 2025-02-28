#include <iostream>
#include <algorithm>

#include "metabuild_system_module.hpp"
#include "../src/output_formatter.hpp"

void MetabuildSystemModule::process_file(const fs::path& file_path) { // Process file for build system detection
    std::string full_path = fs::canonical(file_path).string();        // Get canonical path as string
    std::replace(full_path.begin(), full_path.end(), '\\', '/');  // Normalize path separators

    for (const auto& [system, pattern] : build_system_patterns) {  // Structured binding for pattern matching
        if (std::regex_search(full_path, pattern)) {  // Search for build system pattern
            detected_systems.insert(system);          // Add detected build system
        }
    }
}

void MetabuildSystemModule::print_stats() const {  // Print build system information
    if (!detected_systems.empty()) {               // Only print if systems were detected
        std::vector<std::pair<std::string, std::string>> items = {  // Format items for output
            {"Build", *detected_systems.begin()}
        };
        OutputFormatter::print_section("Build System", "⚒", items);  // Print formatted build system info
    }
}