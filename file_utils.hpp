#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <filesystem>

namespace fs = std::filesystem;

bool is_source_file(const fs::path& path);

#endif // FILE_UTILS_HPP
