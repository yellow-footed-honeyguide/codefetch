#include <memory>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include <filesystem>

#include <CLI/CLI.hpp>

#include "file_utils.hpp"
#include "license_module.hpp"
#include "statistics_module.hpp"
#include "thread_safe_queue.hpp"
#include "modules/git_module.hpp"
#include "line_counter_module.hpp"
#include "language_stats_module.hpp"
#include "metabuild_system_module.hpp"

namespace fs = std::filesystem;

ThreadSafeQueue file_queue;
std::atomic<size_t> files_processed{0};
std::atomic<size_t> total_files{0};

void process_files(std::vector<std::unique_ptr<CodeFetchModule>> &modules) {
    fs::path file_path;
    while (file_queue.pop(file_path)) {
        for (auto &module : modules) {
            module->process_file(file_path);
        }
        files_processed++;
    }
}

int main(int argc, char *argv[]) {
    // 1 STEP. PARSE ARGUMETNS
    CLI::App app{"CodeFetch - A code statistics tool"};
    app.set_version_flag("-v,--version", std::string(PROJECT_VERSION));

    std::string dir_path;
    app.add_option("directory", dir_path, "Directory to analyze")->required();

    bool show_line_counter = false;
    app.add_flag("-c,--line_counter", show_line_counter, "Show line counter statistics");

    bool show_languages = false;
    app.add_flag("-l,--languages", show_languages, "Show language statistics");

    bool show_git = false;
    app.add_flag("-g,--git-statistics", show_git, "Show git statistics information");

    bool show_metabuild_system = false;
    app.add_flag("-m,--metabuild_system", show_metabuild_system,
                 "Show metabuild system information");

    bool show_license = false;
    app.add_flag("-i,--license", show_license, "Show license information");

    CLI11_PARSE(app, argc, argv);

		// 2 STEP. CHECK THE DIRECTORY IS VALID
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

		// 3 STEP. CREATE ARRAY OF MODULES
    std::vector<std::unique_ptr<CodeFetchModule>> modules;
    if (!show_languages && !show_license && !show_metabuild_system
									 	&& !show_git && !show_line_counter) {
        modules.push_back(std::make_unique<LineCounterModule>());
        modules.push_back(std::make_unique<LanguageStatsModule>());
        modules.push_back(std::make_unique<GitModule>());
        modules.push_back(std::make_unique<MetabuildSystemModule>());
        modules.push_back(std::make_unique<LicenseModule>());
    } else {
        if (show_line_counter) {
            modules.push_back(std::make_unique<LineCounterModule>());
        }
        if (show_languages) {
            modules.push_back(std::make_unique<LanguageStatsModule>());
        }
        if (show_metabuild_system) {
            modules.push_back(std::make_unique<MetabuildSystemModule>());
        }
        if (show_license) {
            modules.push_back(std::make_unique<LicenseModule>());
        }
        if (show_git) {
            modules.push_back(std::make_unique<GitModule>());
        }
    }

		// STEP 4. FILE COLLECTION AND QUEUEING 
    for (const auto &entry : fs::recursive_directory_iterator(dir_path)) {
        if (fs::is_regular_file(entry) && FileUtils::is_source_file(entry.path())) {
            file_queue.push(entry.path());
            total_files++;
        }
    }

    if (total_files == 0) {
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish();

		// STEP 5. THREAD CREATION AND LAUNCH
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files, std::ref(modules));
    }

    for (auto &thread : threads) {
        thread.join();
    }

		// STEP 6. DISPLAYING RESULTS
    for (const auto &module : modules) {
        module->print_stats();
    }

    return 0;
}
