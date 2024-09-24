#include "file_utils.hpp"
#include <functional>
#include <algorithm>
#include <unordered_set>

namespace FileUtils {
  bool is_source_file(const fs::path& path) {
    static const std::vector<std::string> extensions = { 
        ".c", ".cpp", ".cxx", ".cc", ".h", ".hpp", ".hxx", ".py", ".java",
        ".js", ".ts", ".rb", ".php", ".go", ".rs", ".swift", ".kt",
        ".vim", ".txt", ".pl", ".mk", ".sh", ".m4", ".yaml", ".yml", ".md",
        ".license", ".def", ".awk", ".html", ".rc", ".lua", ".csh", ".cs",
        ".el", ".forth", ".idl", ".json", ".m", ".svg", ".sv", ".wl", ".xml",
        ".dts", ".dtsi", ".s", ".S", ".asm", ".bash", ".pl", ".dot", ".csv",
        ".xsl", ".y", ".l", ".ld", ".cmake", ".ac", ".uc", ".smk", ".css",
        ".service", ".bzl", ".hex", ".ini", ".sed", ".feature", ".mat", ".rb",
        ".toml", ".tex", ".xsd", ".proto", ".cf", ".dockerfile", ".ps1",
        ".gotmpl", ".jsonl", ".zsh", ".hh", ".hpp", ".hxx",
        ".mm", ".scala", ".groovy", ".f", ".f90", ".f95", ".hs",
        ".jl", ".lisp", ".lsp", ".pas", ".r", ".sql", ".tcl", ".vb", ".vhdl",
    };

    static const std::unordered_set<std::string> special_files = {
        "WORKSPACE", "BUILD.bazel", "meson.build", "CMakeLists.txt", 
				"configure.ac", "Makefile", "LICENSE", "LICENSE.txt", "LICENSE.md",
				"COPYING", "README.md", "README"
    };

    std::string filename = path.filename().string();
    if (special_files.find(filename) != special_files.end()) {
        return true;
    }

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
            } else if (fs::is_regular_file(current_path) && is_source_file(current_path)) {
                file_queue.push(current_path);
                total_files++;
            }
        }
    };

    traverse(dir_path, 0);
}

} // namespace FileUtils
