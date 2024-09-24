#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "file_utils.hpp"
#include "thread_safe_queue.hpp"
#include "statistics_module.hpp"
#include "line_counter_module.hpp"
#include "language_stats_module.hpp"
#include "metabuild_system_module.hpp"
#include "license_module.hpp"

namespace fs = std::filesystem;

// Глобальные переменные
ThreadSafeQueue file_queue;
std::atomic<size_t> files_processed{0};
std::atomic<size_t> total_files{0};

// Функция для обработки файлов с использованием всех зарегистрированных модулей
void process_files(std::vector<std::unique_ptr<StatisticsModule>>& modules) {
    fs::path file_path;
    while (file_queue.pop(file_path)) {
        for (auto& module : modules) {
            module->process_file(file_path);
        }
        files_processed++;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return 1;
    }

    fs::path dir_path(argv[1]);
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        std::cerr << "Error: Invalid directory path." << std::endl;
        return 1;
    }

    // Инициализация модулей статистики
    std::vector<std::unique_ptr<StatisticsModule>> modules;
    modules.push_back(std::make_unique<LineCounterModule>());
    modules.push_back(std::make_unique<LanguageStatsModule>());
    modules.push_back(std::make_unique<MetabuildSystemModule>());
    modules.push_back(std::make_unique<LicenseModule>());

    // Обход директории и заполнение очереди файлов
    for (const auto& entry : fs::recursive_directory_iterator(dir_path)) {
        if (fs::is_regular_file(entry) && FileUtils::is_source_file(entry.path())) {
            file_queue.push(entry.path());
            total_files++;
        }
    }

    if (total_files == 0) {
        std::cerr << "Error: No source files found in the specified directory." << std::endl;
        return 1;
    }

    file_queue.finish();  // Сигнализируем о завершении добавления файлов

    // Создание и запуск рабочих потоков
    unsigned int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_files, std::ref(modules));
    }

    // Отображение прогресса
    while (files_processed < total_files) {
        std::cout << "\rProcessing files: " << files_processed << " / " << total_files
                  << " (" << (files_processed * 100 / total_files) << "%)" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "\rProcessing files: " << total_files << " / " << total_files << " (100%)" << std::endl;

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Вывод статистики от всех модулей
    for (const auto& module : modules) {
        module->print_stats();
    }

    return 0;
}
