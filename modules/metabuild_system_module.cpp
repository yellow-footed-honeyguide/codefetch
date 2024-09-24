#include "metabuild_system_module.hpp"
#include <iostream>
#include <algorithm>

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
    std::cout << "\n\033[1;34mMetabuild System\033[0m\n";
    if (detected_systems.empty()) {
        std::cout << "None detected\n";
    } else {
        for (const auto& system : detected_systems) {
            std::cout << system << '\n';
        }
    }
    std::cout << '\n';
}
