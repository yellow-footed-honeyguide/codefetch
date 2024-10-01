#ifndef LINE_COUNTER_MODULE_HPP
#define LINE_COUNTER_MODULE_HPP

#include "statistics_module.hpp"
#include "../src/line_count_util.hpp"

class LineCounterModule : public CodeFetchModule {
private:
    LineCounter counter;

public:
    void process_file(const fs::path& file_path) override;
    void print_stats() const override;
};

#endif // LINE_COUNTER_MODULE_HPP

