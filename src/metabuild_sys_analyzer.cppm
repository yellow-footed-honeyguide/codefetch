module;  
#include <iostream>    
#include <algorithm> 
#include <string>             
#include <regex>              
#include <unordered_map>      
#include <unordered_set> 
#include <filesystem>     
export module metabuild_sys_analyzer;
import console_formatter;
import analyzer_base;

  
export class MetabuildSystemModule : public CodeFetchModule {
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
    void process_file(const std::filesystem::path& file_path) override{ // Process file for build system detection
        std::string full_path = std::filesystem::canonical(file_path).string(); // Get canonical path as string
        std::replace(full_path.begin(), full_path.end(), '\\', '/'); // Normalize path separators

        for (const auto& [system, pattern] : build_system_patterns) {// Structured binding for pattern matching
            if (std::regex_search(full_path, pattern)) { // Search for build system pattern
                detected_systems.insert(system); // Add detected build system
            }
        }
    }

    void print_stats() const override{ // Print build system information
        if (!detected_systems.empty()) { // Only print if systems were detected
            std::vector<std::pair<std::string, std::string>> items = { // Format items for output
                {"Build", *detected_systems.begin()}
            };
            OutputFormatter::print_section("Build System", "⚒", items); // Print formatted build system info
        }
    }

    bool has_detected_systems() const { return !detected_systems.empty(); } // Check if systems were detected

};
