#include <functional>
#include <algorithm>
#include <unordered_set>
#include <string>
#include "file_utils.hpp"
#include "../modules/file_extension_to_language_map.hpp"

namespace FileUtils {
    bool is_source_file(const fs::path& path) {  // Check if file is a source file based on extension
        // static const std::vector<std::string> extensions = { // Static vector of supported extensions

        // static const std::vector<std::string> extensions = {

        std::vector<std::string> extensions;
        for (const auto& ext : extension_to_language) {
            extensions.push_back(ext.first);
        }


        static const std::unordered_set<std::string> special_files = { // Hash set of special filenames
            "WORKSPACE", "BUILD.bazel", "meson.build", "CMakeLists.txt", 
            "configure.ac", "Makefile", "LICENSE", "LICENSE.txt", "LICENSE.md",
            "COPYING", "README.md", "README"
        };

        std::string filename = path.filename().string();           // Get filename from path
        if (special_files.find(filename) != special_files.end()) { // Check if file is in special files set
            return true;
        }

        std::string ext = path.extension().string();               // Get file extension
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // Convert extension to lowercase
        // Check if extension is supported
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();  
    }

    // Recursively traverse directory with depth limit
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, 
        std::atomic<size_t>& total_files, int max_depth) {
        // Lambda function for recursive directory traversal
        std::function<void(const fs::path&, int)> traverse = [&](const fs::path& path, int depth) {
            if (depth > max_depth) return;                            // Base case for recursion depth

            for (const auto& entry : fs::directory_iterator(path)) {  // Iterate directory contents
                const auto& current_path = entry.path();              // Get current entry path

                if (fs::is_directory(current_path)) {                 // Recurse into subdirectories
                    traverse(current_path, depth + 1);
                // Process source files
                } else if (fs::is_regular_file(current_path) && is_source_file(current_path)) {  
                    file_queue.push(current_path);                    // Add file to processing queue
                    total_files++;                                    // Increment atomic counter
                }
            }
        };

        traverse(dir_path, 0);                                       // Start traversal from root directory
    }

} // namespace FileUtils
