#include <fstream>      // [C++98]
#include <string>       // [C++98]
#include <algorithm>    // [C++98]

#include "line_count_util.hpp"

void LineCounter::count_lines(const fs::path& file_path) {      // [C++17] Count lines using filesystem path
    std::ifstream file(file_path);                              // [C++17] Open file using path object
    std::string line;                                           // [C++98] String for storing each line
    bool in_multiline_comment = false;                          // [C++98] Track multiline comment state
    LineCount local_count;                                      // [C++11] Local counter for atomic operations

    while (std::getline(file, line)) {                          // [C++98] Read file line by line
        line.erase(0, line.find_first_not_of(" \t\r\n"));       // [C++11] Trim whitespace from start
        if (line.empty()) continue;                             // [C++11] Skip empty lines

        if (!in_multiline_comment) {                            // [C++98] Process line based on comment state
            if (line.starts_with("//")) {                       // [C++20] Check for single-line comment
                local_count.comments++;                         // [C++11] Increment comment count
            } else if (line.starts_with("/*")) {                // [C++20] Check for multi-line comment start
                local_count.comments++;                         // [C++11] Increment comment count
                in_multiline_comment = true;                    // [C++98] Set multiline comment flag
                if (line.find("*/") != std::string::npos) {     // [C++11] Check for comment end on same line
                    in_multiline_comment = false;               // [C++98] Reset multiline comment flag
                }
            } else {
                local_count.code++;                             // [C++11] Increment code line count
            }
        } else {
            local_count.comments++;                             // [C++11] Increment comment count in multiline
            if (line.find("*/") != std::string::npos) {         // [C++11] Check for multiline comment end
                in_multiline_comment = false;                   // [C++98] Reset multiline comment flag
            }
        }
    }

    total_count.code += local_count.code;                       // [C++11] Update atomic total code count
    total_count.comments += local_count.comments;               // [C++11] Update atomic total comment count
}

size_t LineCounter::count_lines_in_file(const fs::path& file_path) {  // [C++17] Count total lines in file
    std::ifstream file(file_path);                                    // [C++17] Open file using path object
    return std::count(std::istreambuf_iterator<char>(file),           // [C++11] Count newlines using iterators
                      std::istreambuf_iterator<char>(), '\n') + 1;
}