cmake_minimum_required(VERSION 3.12)  # Minimum CMake version required

# Project definition
project(codefetch VERSION 0.9.10)

# Set C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the project version as a preprocessor macro
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Find required packages
find_package(Threads REQUIRED)  # For multithreading support

# Find LibGit2 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 REQUIRED libgit2)

# Define source files
set(SOURCES
        src/main.cpp
        src/file_utils.cpp
        src/line_count_util.cpp
        src/thread_safe_queue.cpp
        src/output_formatter.cpp
        modules/total_lines.cpp
        modules/language_stats_lib.cpp
        modules/language_stats.cpp
        modules/metabuild_system.cpp
        modules/license_detect.cpp
        modules/git_statistics.cpp
)

# Create executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/modules
        ${LIBGIT2_INCLUDE_DIRS}
)

# Link required libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        Threads::Threads
        ${LIBGIT2_LIBRARIES}
)

# Installation setup
install(TARGETS ${PROJECT_NAME} DESTINATION bin)