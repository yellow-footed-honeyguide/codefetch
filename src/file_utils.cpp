#include "file_utils.hpp"
#include <functional>

namespace FileUtils {

bool is_source_file(const fs::path& path) {
    static const std::vector<std::string> extensions = {
        ".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".py", ".java",
        ".js", ".ts", ".rb", ".php", ".go", ".rs", ".swift", ".kt"
        // Добавьте другие расширения по необходимости
    };
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}

void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, std::atomic<size_t>& total_files, int max_depth) {
    std::function<void(const fs::path&, int)> traverse = [&](const fs::path& path, int depth) {
        if (depth > max_depth) return;

        for (const auto& entry : fs::directory_iterator(path)) {
            const auto& current_path = entry.path();

            if (fs::is_directory(current_path)) {
                traverse(current_path, depth + 1);
            } else if (fs::is_regular_file(current_path) && is_source_file(current_path) && !current_path.filename().string().starts_with(".")) {
                file_queue.push(current_path);
                total_files++;
            }
        }
    };

    traverse(dir_path, 0);
}

} // namespace FileUtils
