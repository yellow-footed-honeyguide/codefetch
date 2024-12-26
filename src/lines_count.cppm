module;  
#include <atomic>       
#include <filesystem>   
#include <fstream>      
#include <string>       
#include <algorithm> 
export module lines_count;


//namespace fs = std::filesystem;      

export struct LineCount {
    std::atomic<size_t> code{0}; // Atomic counter for code lines with in-class initializer
    std::atomic<size_t> comments{0}; // Atomic counter for comment lines with in-class initializer
};

export class LineCounter {
private:
    LineCount total_count; // Total line counts using atomic counters

public:
    // Count lines using filesystem path
    void count_lines(const std::filesystem::path& file_path) { 
        std::ifstream file(file_path); // Open file using path object
        std::string line; // String for storing each line
        bool in_multiline_comment = false; // Track multiline comment state
        LineCount local_count; // Local counter for atomic operations

        while (std::getline(file, line)) { // Read file line by line
            line.erase(0, line.find_first_not_of(" \t\r\n")); // Trim whitespace from start
            if (line.empty()) continue; // Skip empty lines

            if (!in_multiline_comment) { // Process line based on comment state
                if (line.starts_with("//")) { // Check for single-line comment
                    local_count.comments++; // Increment comment count
                } else if (line.starts_with("/*")) { // Check for multi-line comment start
                    local_count.comments++; // Increment comment count
                    in_multiline_comment = true; // Set multiline comment flag
                    if (line.find("*/") != std::string::npos) { // Check for comment end on same line
                        in_multiline_comment = false; // Reset multiline comment flag
                    }
                } else {
                    local_count.code++; // Increment code line count
                }
            } else {
                local_count.comments++; // Increment comment count in multiline
                if (line.find("*/") != std::string::npos) { // Check for multiline comment end
                    in_multiline_comment = false; // Reset multiline comment flag
                }
            }
        }
        total_count.code += local_count.code; // Update atomic total code count
        total_count.comments += local_count.comments; // Update atomic total comment count
    }

    // Count total lines in file
    static size_t count_lines_in_file(const std::filesystem::path& file_path) { 
        std::ifstream file(file_path); // Open file using path object
        return std::count(std::istreambuf_iterator<char>(file), // Count newlines using iterators
                        std::istreambuf_iterator<char>(), '\n') + 1;
    }

    // Getter for total counts
    const LineCount& get_total_count() const { return total_count; } 
};


