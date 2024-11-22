#include <fstream>  // [C++98]
#include <sstream>  // [C++98]
#include <iostream> // [C++98]

#include "license_module.hpp"
#include "../src/output_formatter.hpp"

void LicenseModule::process_file(const fs::path &file_path) { // [C++17] Process potential license file
    std::string filename = file_path.filename().string();     // [C++17] Get filename from path
    if (filename == "LICENSE" || filename == "LICENSE.txt" || 
        filename == "LICENSE.md" || filename == "COPYING" || 
        filename == "README.md" || filename == "README") {    // Check for license files
        std::ifstream file(file_path);                        // [C++17] Open file using path
        if (file.is_open()) {
            // [C++11] Read entire file content using iterators
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            detect_license(content);                          // Analyze content for license
        }
    }
}

void LicenseModule::detect_license(const std::string &content) { // [C++11] Detect license type from content
    for (const auto &[license, pattern] : license_patterns) {    // [C++17] Structured binding for pattern matching
        if (std::regex_search(content, pattern)) {               // [C++11] Search for license pattern
            detected_license = license;                          // Store detected license
            return;
        }
    }
}

void LicenseModule::print_stats() const {                          // [C++11] Print license information
    if (!detected_license.empty()) {                               // Print only if license was detected
        std::vector<std::pair<std::string, std::string>> items = { // [C++11] Format items for output
            {"License", detected_license}
        };
        OutputFormatter::print_section("License", "§", items);     // Print formatted license info
    }
}