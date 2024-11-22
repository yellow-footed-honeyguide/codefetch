#ifndef LINE_COUNTER_MODULE_HPP
#define LINE_COUNTER_MODULE_HPP

#include "statistics_module.hpp"
#include "../src/line_count_util.hpp"

class LineCounterModule : public CodeFetchModule {         // [C++11] Module for counting code lines
private:
    LineCounter counter;                                   // [C++11] Line counter instance

public:
    void process_file(const fs::path& file_path) override; // [C++17] Process file implementation
    void print_stats() const override;                     // [C++11] Print statistics implementation
};

#endif // LINE_COUNTER_MODULE_HPP