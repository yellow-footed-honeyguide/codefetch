#pragma once

#include "codefetch_module_interface.hpp"
#include "../src/line_count_util.hpp"

class LineCounterModule : public CodeFetchModule {  // Module for counting code lines
private:
    LineCounter counter;                            // Line counter instance

public:
    void process_file(const fs::path& file_path) override; // Process file implementation
    void print_stats() const override;              // Print statistics implementation
};

