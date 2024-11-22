#include <functional>     // [C++11]
#include <algorithm>      // [C++98]
#include <unordered_set>  // [C++11]

#include "file_utils.hpp"

namespace FileUtils {
    bool is_source_file(const fs::path& path) {  // [C++17] Check if file is a source file based on extension
        static const std::vector<std::string> extensions = { // [C++11] Static vector of supported extensions
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

        static const std::unordered_set<std::string> special_files = { // [C++11] Hash set of special filenames
            "WORKSPACE", "BUILD.bazel", "meson.build", "CMakeLists.txt", 
            "configure.ac", "Makefile", "LICENSE", "LICENSE.txt", "LICENSE.md",
            "COPYING", "README.md", "README"
        };

        std::string filename = path.filename().string();           // [C++17] Get filename from path
        if (special_files.find(filename) != special_files.end()) { // [C++11] Check if file is in special files set
            return true;
        }

        std::string ext = path.extension().string();                     // [C++17] Get file extension
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // [C++11] Convert extension to lowercase
        // [C++11] Check if extension is supported
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();  
    }

    // [C++11] Recursively traverse directory with depth limit
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, 
        std::atomic<size_t>& total_files, int max_depth) {
        // [C++11] Lambda function for recursive directory traversal
        std::function<void(const fs::path&, int)> traverse = [&](const fs::path& path, int depth) {
            if (depth > max_depth) return;                            // Base case for recursion depth

            for (const auto& entry : fs::directory_iterator(path)) {  // [C++17] Iterate directory contents
                const auto& current_path = entry.path();              // [C++17] Get current entry path

                if (fs::is_directory(current_path)) {                 // [C++17] Recurse into subdirectories
                    traverse(current_path, depth + 1);
                // [C++17] Process source files
                } else if (fs::is_regular_file(current_path) && is_source_file(current_path)) {  
                    file_queue.push(current_path);                    // Add file to processing queue
                    total_files++;                                    // [C++11] Increment atomic counter
                }
            }
        };

        traverse(dir_path, 0);                                       // Start traversal from root directory
    }

} // namespace FileUtils