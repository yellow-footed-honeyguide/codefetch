#include "metabuild_system_module.hpp"
#include <iostream>

void MetabuildSystemModule::process_file(const fs::path& file_path) {
    std::string filename = file_path.filename().string();
    for (const auto& [system, pattern] : build_system_patterns) {
        if (std::regex_search(filename, pattern)) {
            detected_systems.insert(system);
            break;
        }
    }
}

void MetabuildSystemModule::print_stats() const {
    std::cout << "\n\033[1;34mMetabuild System\033[0m\n";  // Добавлен \n в начале
    if (detected_systems.empty()) {
        std::cout << "None detected\n";
    } else {
        for (const auto& system : detected_systems) {
            std::cout << system << '\n';
        }
    }
    std::cout << '\n';
}
