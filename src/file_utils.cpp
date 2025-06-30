#include <string>     
#include <unordered_set> 
#include <mutex>          // For std::once_flag
#include <functional>     // For std::function and std::bind
#include <algorithm>      // For std::transform

#include "file_utils.hpp"
#include "../modules/file_extension_to_language_map.hpp" // Extension to language mapping

namespace FileUtils {
    // Static cache for file extensions to avoid repeated processing
    static std::unordered_set<std::string> extension_cache;
    // Flag to ensure thread-safe one-time initialization of the cache
    static std::once_flag cache_flag;
    
    // Initializes the extension cache with values from the mapping
    void init_cache() {
        // Iterate through all extensions in the global mapping
        for (const auto& [ext, _] : extension_to_language) {
            extension_cache.insert(ext);  // Add each extension to cache
        }
    }
    
    // Checks if a given file path points to a source code file
    bool is_source_file(const fs::path& path) {
        // Initialize cache exactly once (thread-safe)
        std::call_once(cache_flag, init_cache);
        
        // Set of special filenames that should be treated as source files
        static const std::unordered_set<std::string> special_files = {
            "WORKSPACE", "BUILD.bazel", "meson.build", "CMakeLists.txt", 
            "configure.ac", "Makefile", "LICENSE", "LICENSE.txt", "LICENSE.md",
            "COPYING", "README.md", "README"
        };

        // Extract filename from path for special files check
        std::string filename = path.filename().string();
        // Check if this is one of our special files
        if (special_files.find(filename) != special_files.end()) {
            return true;  // Early return for special files
        }

        // Get file extension and convert to lowercase for case-insensitive comparison
        std::string ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Check if extension exists in our pre-loaded cache
        return extension_cache.find(ext) != extension_cache.end();
    }

    // Recursively traverses directory and adds source files to queue
    void traverse_directory(const fs::path& dir_path, ThreadSafeQueue& file_queue, 
        std::atomic<size_t>& total_files, int max_depth) {
        
        // Use error_code to avoid exception handling for filesystem operations
        std::error_code ec;
        
        // Define recursive lambda for directory traversal
        std::function<void(const fs::path&, int)> traverse = [&](const fs::path& path, int depth) {
            // Stop recursion if we've reached maximum depth
            if (depth > max_depth) return;

            // Iterate through directory entries
            for (const auto& entry : fs::directory_iterator(path, ec)) {
                if (ec) continue;  // Skip entries that caused errors
                
                const auto& current_path = entry.path();

                // Check if entry is a directory (using faster entry methods)
                if (entry.is_directory(ec) && !ec) {
                    traverse(current_path, depth + 1);  // Recurse into subdirectory
                } 
                // Check if entry is a regular file and a source file
                else if (entry.is_regular_file(ec) && !ec && is_source_file(current_path)) {
                    file_queue.push(current_path);  // Add file to processing queue
                    total_files.fetch_add(1, std::memory_order_relaxed);  // Increment counter
                }
            }
        };

        // Start recursive traversal from root directory at depth 0
        traverse(dir_path, 0);
    }
}
