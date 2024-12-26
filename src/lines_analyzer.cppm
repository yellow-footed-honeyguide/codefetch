module;
#include <filesystem> 
#include <vector>       
export module lines_analyzer;
import stats_reporter; 
import lines_count;
import console_formatter;
import analyzer_base;

export class LineCounterModule : public CodeFetchModule { // Module for counting code lines
private:
    LineCounter counter; // Line counter instance

public:
    void process_file(const std::filesystem::path& file_path) override{ // Process single file for line counting
        counter.count_lines(file_path); // Count lines in file
    }
    
    // Print line counting statistics
    void print_stats() const override{ 
        const auto& total_count = counter.get_total_count(); // Get total line counts
        size_t total = total_count.code + total_count.comments; // Calculate total lines
        size_t other = total - total_count.code - total_count.comments; // Calculate other lines

        // Prepare formatted statistics items
        std::vector<std::pair<std::string, std::string>> items = {
            {"Total", OutputFormatter::format_large_number(total)},
            {"Code", OutputFormatter::format_large_number(total_count.code)},
            {"Comm.", OutputFormatter::format_large_number(total_count.comments)},
            {"Other", OutputFormatter::format_large_number(other)}
        };

        OutputFormatter::print_section("Lines of Code", "⚑", items); // Print formatted statistics
    }


};
