#include <iostream>  
#include <sstream>
#include <string>   
#include <locale>    
#include <memory>    
#include <array> 
#include <thread> 
#include <format>  // Modern string formatting library (C++20)
  

#include "git_statistics.hpp"       
#include "../src/output_formatter.hpp"


extern std::string dir_for_analysis;

// Constructor initializing with number of contributors to display
GitModule::GitModule(size_t contributors_count) 
    : contributors_count(contributors_count) {  // Initialize member variable
}

// Executes shell command and returns its output as string
std::string GitModule::exec_command(const std::string& cmd) const {
    std::array<char, 4096> buffer;  // Typical system page size
    std::string result;             // Accumulates command output
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
    // Prepare optimized git commands with performance flags
    std::string count_cmd = dir_for_analysis.empty() 
        ? "git --no-pager rev-list HEAD --count 2>/dev/null"
        : std::format("git --no-pager -C {} rev-list HEAD --count 2>/dev/null", dir_for_analysis);
    
    std::string shortlog_cmd = dir_for_analysis.empty()
        ? std::format("git --no-pager shortlog -sn HEAD 2>/dev/null | head -n {}", contributors_count)
        : std::format("git --no-pager -C {} shortlog -sn HEAD 2>/dev/null | head -n {}", dir_for_analysis, contributors_count);
    
    // Combined command for both first and last commit dates
    std::string dates_cmd = dir_for_analysis.empty()
        ? R"(git --no-pager log HEAD --pretty=format:"%ci" --reverse 2>/dev/null | head -n 1 && echo "SEPARATOR" && git --no-pager log HEAD --pretty=format:"%ci" -n 1 2>/dev/null)"
        : std::format(R"(git --no-pager -C {} log HEAD --pretty=format:"%ci" --reverse 2>/dev/null | head -n 1 && echo "SEPARATOR" && git --no-pager -C {} log HEAD --pretty=format:"%ci" -n 1 2>/dev/null)", dir_for_analysis, dir_for_analysis);

    // Storage for parallel execution results
    std::string count_result, shortlog_result, dates_result;
    
    // Execute three git commands in parallel threads
    std::thread dates_thread([&]() { dates_result = exec_command(dates_cmd); });
    std::thread count_thread([&]() { count_result = exec_command(count_cmd); });
    std::thread shortlog_thread([&]() { shortlog_result = exec_command(shortlog_cmd); });
    
    // Wait for all threads to complete
    dates_thread.join();
    count_thread.join();
    shortlog_thread.join();
    
    // Parse total commit count
    size_t total_commits = 0;
    if (!count_result.empty()) {
        total_commits = std::stoull(count_result);  // Convert string to unsigned long long
    }
    
    // Parse first and last commit dates from combined output
    std::string first_date = "N/A", last_date = "N/A";
    if (!dates_result.empty()) {
        size_t separator_pos = dates_result.find("SEPARATOR");
        if (separator_pos != std::string::npos) {
            // Extract first date (before separator)
            first_date = dates_result.substr(0, separator_pos);
            // Extract last date (after separator)
            last_date = dates_result.substr(separator_pos + 10);  // 10 = length of "SEPARATOR"
            
            // Trim dates to YYYY-MM-DD format and remove newlines
            if (first_date.length() >= 10) first_date = first_date.substr(0, 10);
            if (last_date.length() >= 10) last_date = last_date.substr(0, 10);
            
            // Clean up any remaining whitespace
            first_date.erase(first_date.find_last_not_of(" \t\n\r") + 1);
            last_date.erase(last_date.find_last_not_of(" \t\n\r") + 1);
        }
    }
    
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
    
    // Print contributors section header
    std::cout << "☺ Top Contributors" << std::endl;
    
    // Handle case when no contributors found
    if (shortlog_result.empty()) {
        std::cout << "  ╰─ No contributors found\n" << std::endl;
        return;
    }
    
    // Parse contributors output line by line
    std::stringstream ss(shortlog_result);
    std::string line;
    size_t processed = 0;  // Tracks number of contributors processed
    
    // Pre-create locale object once for performance
    static std::locale system_locale("");
    
    while (std::getline(ss, line) && processed < contributors_count) {
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
            comm_ss.imbue(system_locale);  // Use pre-created locale
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
