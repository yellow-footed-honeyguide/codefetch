#include <fstream>
#include <sstream>

#include "license_detect.hpp"
#include "../src/output_formatter.hpp"

void LicenseModule::process_file(const fs::path &file_path) { // Process potential license file
    std::string filename = file_path.filename().string();     // Get filename from path
    if (filename == "LICENSE" || filename == "LICENSE.txt" || 
        filename == "LICENSE.md" || filename == "COPYING" || 
        filename == "README.md" || filename == "README") {    // Check for license files
        std::ifstream file(file_path);                        // Open file using path
        if (file.is_open()) {
            // Read entire file content using iterators
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            detect_license(content);                          // Analyze content for license
        }
    }
}

void LicenseModule::detect_license(const std::string &content) { // Detect license type from content
    // Structured binding for pattern matching
    for (const auto &[license, pattern] : license_patterns) {
        if (std::regex_search(content, pattern)) {  // Search for license pattern
            detected_license = license;             // Store detected license
            return;
        }
    }
}

void LicenseModule::print_stats() const {  // Print license information
    if (!detected_license.empty()) {       // Print only if license was detected
        std::vector<std::pair<std::string, std::string>> items = { // Format items for output
            {"License", detected_license}
        };
        // Print formatted license info
        OutputFormatter::print_section("License", "§", items);
    }
}