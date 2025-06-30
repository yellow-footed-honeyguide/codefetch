#include <fstream>       
#include <string>  
#include <vector>        
#include <algorithm>     // For std::count
#include <sys/mman.h>    // For memory-mapped file operations
#include <sys/stat.h>    // For file status information
#include <fcntl.h>       // For file control options
#include <unistd.h>      // For POSIX API (close, read, etc.)

#include "line_count_util.hpp"  // Header for LineCounter class

// Counts lines in a file using memory-mapped files or fallback to standard I/O
void LineCounter::count_lines(const fs::path& file_path) {
    // Open file for memory mapping (O_RDONLY = read-only mode)
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == -1) {
        // Fallback to standard file reading if memory mapping fails
        std::ifstream file(file_path, std::ios::binary);  // Open in binary mode for consistent behavior
        if (!file) return;  // Return if file cannot be opened
        
        // Buffer size optimized for performance (64KB)
        constexpr size_t BUFFER_SIZE = 64 * 1024;
        std::vector<char> buffer(BUFFER_SIZE);  // Allocate read buffer
        
        LineCount local_count;              // Temporary counter for this file
        bool in_multiline_comment = false;  // Not used in current implementation
        
        // Read file in chunks until EOF
        while (file.read(buffer.data(), BUFFER_SIZE) || file.gcount() > 0) {
            size_t bytes_read = file.gcount();  // Get actual bytes read
            
            // Simple line counting - count newline characters
            for (size_t i = 0; i < bytes_read; ++i) {
                if (buffer[i] == '\n') {
                    local_count.code++;  // Increment line counter
                }
            }
        }
        
        total_count.code += local_count.code;  // Add to total count
        return;
    }
    
    // Get file statistics (size, permissions etc.)
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);  // Close file descriptor on error
        return;
    }
    
    size_t file_size = st.st_size;  // Get file size in bytes
    if (file_size == 0) {
        close(fd);  // Close immediately for empty files
        return;
    }
    
    // Memory-map the file for fast access
    void* mapped = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        close(fd);  // Cleanup if mapping fails
        return;
    }
    
    // Optimize for sequential access pattern
    madvise(mapped, file_size, MADV_SEQUENTIAL);
    
    const char* data = static_cast<const char*>(mapped);  // Cast to char pointer
    LineCount local_count;  // Temporary counter
    
    // Super-fast line counting - just count newlines
    for (size_t i = 0; i < file_size; ++i) {
        if (data[i] == '\n') {
            local_count.code++;  // Increment for each newline
        }
    }
    
    // Handle files not ending with newline (count as additional line)
    if (file_size > 0 && data[file_size - 1] != '\n') {
        local_count.code++;
    }
    
    munmap(mapped, file_size);  // Unmap memory
    close(fd);                  // Close file descriptor
    
    total_count.code += local_count.code;  // Update total count
}

// Alternative implementation that returns line count directly
size_t LineCounter::count_lines_in_file(const fs::path& file_path) {
    // Try memory-mapped approach first
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == -1) {
        // Fallback to standard file reading
        std::ifstream file(file_path, std::ios::binary);
        if (!file) return 0;  // Return 0 if file cannot be opened
        
        // Larger buffer (1MB) for better performance
        constexpr size_t BUFFER_SIZE = 1024 * 1024;
        std::vector<char> buffer(BUFFER_SIZE);
        
        size_t line_count = 0;
        
        // Read file in chunks and count newlines
        while (file.read(buffer.data(), BUFFER_SIZE) || file.gcount() > 0) {
            size_t bytes_read = file.gcount();
            // Use optimized std::count algorithm
            line_count += std::count(buffer.data(), buffer.data() + bytes_read, '\n');
        }
        
        // Check if file ends without newline
        file.clear();  // Clear error flags to allow seeking
        file.seekg(-1, std::ios::end);  // Move to last character
        char last_char;
        if (file.get(last_char) && last_char != '\n') {
            line_count++;  // Count last line if not terminated
        }
        
        return line_count;
    }
    
    // Get file statistics
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return 0;
    }
    
    size_t file_size = st.st_size;
    if (file_size == 0) {
        close(fd);
        return 0;  // Empty file has 0 lines
    }
    
    // Memory-map the file
    void* mapped = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        close(fd);
        return 0;
    }
    
    // Optimize for sequential reading
    madvise(mapped, file_size, MADV_SEQUENTIAL);
    
    const char* data = static_cast<const char*>(mapped);
    
    // Vectorized line counting using std::count
    size_t line_count = std::count(data, data + file_size, '\n');
    
    // Handle last line without newline
    if (file_size > 0 && data[file_size - 1] != '\n') {
        line_count++;
    }
    
    munmap(mapped, file_size);  // Cleanup memory mapping
    close(fd);  // Close file descriptor
    
    return line_count;
}

