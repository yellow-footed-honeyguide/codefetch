#include "line_counter.hpp"
#include <fstream>
#include <string>
#include <algorithm>

void LineCounter::count_lines(const fs::path& file_path) {
    std::ifstream file(file_path);
    std::string line;
    bool in_multiline_comment = false;
    LineCount local_count;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        if (line.empty()) continue;

        if (!in_multiline_comment) {
            if (line.starts_with("//")) {
                local_count.comments++;
            } else if (line.starts_with("/*")) {
                local_count.comments++;
                in_multiline_comment = true;
                if (line.find("*/") != std::string::npos) {
                    in_multiline_comment = false;
                }
            } else {
                local_count.code++;
            }
        } else {
            local_count.comments++;
            if (line.find("*/") != std::string::npos) {
                in_multiline_comment = false;
            }
        }
    }

    total_count.code += local_count.code;
    total_count.comments += local_count.comments;
}

size_t LineCounter::count_lines_in_file(const fs::path& file_path) {
    std::ifstream file(file_path);
    return std::count(std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>(), '\n') + 1;
}
