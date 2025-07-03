#include <iostream>  
#include <sstream>
#include <string>   
#include <locale>    
#include <memory>    
#include <array>  
#include <format>  // Modern string formatting library (C++20)

#include "git_statistics.hpp"       
#include "../src/output_formatter.hpp"  // Custom output formatting utilities


extern std::string dir_for_analysis;

// Constructor initializing with number of contributors to display
GitModule::GitModule(size_t contributors_count) 
    : contributors_count(contributors_count) {  // Initialize member variable
}

// Executes shell command and returns its output as string
std::string GitModule::exec_command(const std::string& cmd) const {
    std::array<char, 128> buffer;  // Fixed-size buffer for reading command output
    std::string result;            // Accumulates command output
    // Smart pointer managing pipe lifecycle (auto-closes with pclose)
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.c_str(), "r"), pclose);

    // Check if pipe opened successfully
    if (!pipe) {
        return "";  // Return empty string on failure
    }
    
    // Read output line by line until EOF
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();  // Append each line to result
    }
    
    return result;  // Return complete command output
}

// Placeholder method for file processing (unused in current implementation)
void GitModule::process_file(const fs::path& file_path) {
    // No operation - all logic is in print_stats()
}

// Main method to calculate and display git repository statistics
void GitModule::print_stats() const {
    // Get total commit count using git rev-list
    std::string count_output = exec_command(
        dir_for_analysis.empty() 
            ? "git rev-list HEAD --count 2>/dev/null" 
            : std::format("git -C {} rev-list HEAD --count 2>/dev/null", dir_for_analysis)
    );

    size_t total_commits = 0;  // Initialize commit counter
    if (!count_output.empty()) {
        total_commits = std::stoull(count_output);  // Convert string to unsigned long long
    }
    
    // Get first and last commit dates using git log
    std::string first_date = exec_command(
    dir_for_analysis.empty()
        ? R"(git log --reverse --format="%ci" 2>/dev/null | head -n 1)"
        : std::format(R"(git -C {} log --reverse --format="%ci" 2>/dev/null | head -n 1)", dir_for_analysis)
    );


    std::string last_date = exec_command(
    dir_for_analysis.empty()
        ? R"(git log -1 --format="%ci" 2>/dev/null)"
        : std::format(R"(git -C {} log -1 --format="%ci" 2>/dev/null)", dir_for_analysis)
    );
    
    // Trim dates to YYYY-MM-DD format
    if (first_date.length() >= 10) first_date = first_date.substr(0, 10);
    if (last_date.length() >= 10) last_date = last_date.substr(0, 10);
    
    // Handle empty dates (uninitialized repo case)
    if (first_date.empty()) first_date = "N/A";
    if (last_date.empty()) last_date = "N/A";
    
    // Print basic git statistics header
    std::cout << "♦ Git Stats" << std::endl;
    // Format output line with commit count and date range
    std::cout << std::format("╰─ {} (Overall Commits) | {} (First) / {} (Last)\n\n", 
                            OutputFormatter::format_large_number(total_commits), 
                            first_date, 
                            last_date);                        
    
    // Prepare command to get top contributors (limited by contributors_count)
    size_t limit = contributors_count;

    std::string shortlog_cmd;
    if (dir_for_analysis.empty()){
        shortlog_cmd = std::format("git shortlog -sn HEAD 2>/dev/null | head -n {}", limit);
    }
    else {
        shortlog_cmd = std::format("git -C {} shortlog -sn HEAD 2>/dev/null | head -n {}", dir_for_analysis, limit); 
    }

    std::string shortlog_output = exec_command(shortlog_cmd);
    
    // Print contributors section header
    std::cout << "☺ Top Contributors" << std::endl;
    
    // Handle case when no contributors found
    if (shortlog_output.empty()) {
        std::cout << "  ╰─ No contributors found\n" << std::endl;
        return;
    }
    
    // Parse contributors output line by line
    std::stringstream ss(shortlog_output);
    std::string line;
    size_t processed = 0;  // Tracks number of contributors processed
    
    while (std::getline(ss, line) && processed < limit) {
        if (line.empty()) continue;  // Skip empty lines
        
        // Parse tab-separated line format: " 46988\tbors"
        size_t tab_pos = line.find('\t');
        if (tab_pos == std::string::npos) continue;  // Skip malformed lines
        
        // Extract commit count and author name
        std::string commits_str = line.substr(0, tab_pos);
        std::string name = line.substr(tab_pos + 1);
        
        // Trim whitespace from both parts
        commits_str.erase(0, commits_str.find_first_not_of(" \t"));
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t\n\r") + 1);
        
        try {
            size_t commits = std::stoull(commits_str);  // Convert commit count to number
            
            // Truncate long names with ellipsis
            if (name.length() > 25) {
                name = name.substr(0, 22) + "...";
            }
            
            // Calculate contributor's percentage of total commits
            double percentage = total_commits > 0 ? (static_cast<double>(commits) / total_commits) * 100.0 : 0.0;
            
            // Format commit count with locale-aware thousands separator
            std::stringstream comm_ss;
            comm_ss.imbue(std::locale(""));  // Apply system locale
            comm_ss << commits;
            std::string formatted_commits = comm_ss.str();
            
            // Print contributor line with formatted stats
            std::cout << std::format("  ╰─ {:<25} : {:>6.1f}% ({:>7} commits)\n",
                                    name, percentage, formatted_commits);
            
            processed++;  // Increment processed counter
        } catch (...) {
            // Silently handle parse errors (malformed number strings)
        }
    }
    
    std::cout << std::endl;  // Final newline for clean output
}
