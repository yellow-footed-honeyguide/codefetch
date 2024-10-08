#include <fstream>
#include <iostream>
#include <sstream>

#include "license_module.hpp"
#include "../src/output_formatter.hpp"


void LicenseModule::process_file(const fs::path &file_path) {
    std::string filename = file_path.filename().string();
    if (filename == "LICENSE" || filename == "LICENSE.txt" || filename == "LICENSE.md" ||
        filename == "COPYING" || filename == "README.md" || filename == "README") {
        std::ifstream file(file_path);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            detect_license(content);
        }
    }
}

void LicenseModule::detect_license(const std::string &content) {
    for (const auto &[license, pattern] : license_patterns) {
        if (std::regex_search(content, pattern)) {
            detected_license = license;
            return;
        }
    }
}

void LicenseModule::print_stats() const {
    if (!detected_license.empty()) {
        std::vector<std::pair<std::string, std::string>> items = {
            {"License", detected_license}
        };
        OutputFormatter::print_section("License", "§", items);
    }
}

