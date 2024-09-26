cmake_minimum_required(VERSION 3.12)
project(codefetch VERSION 0.9.7 LANGUAGES NONE)
add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Check if Clang++ is available
find_program(CLANGXX_COMPILER clang++)

if(CLANGXX_COMPILER)
		# set(CMAKE_CXX_COMPILER ${CLANGXX_COMPILER})
    set(COMPILER_INFO "Using Clang++ compiler: ${CLANGXX_COMPILER}")
else()
    set(COMPILER_INFO "Clang++ not found, using default system compiler")
endif()

message("\n======== Custom Build Information =========")
message(STATUS "${COMPILER_INFO}")
message(STATUS "Using CMake generator: ${CMAKE_GENERATOR}")
message("===========================================\n")

# Enable C++ support
enable_language(CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set optimization flags for maximum performance
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native -ffast-math -DNDEBUG")

# Additional performance optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -flto")

# Enable Link Time Optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${error}")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Use pkg-config to find LibGit2
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 REQUIRED libgit2)

# Add CLI11
include(FetchContent)
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11)

# Add source files
set(SOURCES
    src/main.cpp
    src/file_utils.cpp
    src/line_count_util.cpp
    src/stats_printer.cpp
    src/thread_safe_queue.cpp
    src/signal_handler.cpp
    src/output_formatter.cpp
    modules/line_counter_module.cpp
    modules/language_stats.cpp
    modules/language_stats_module.cpp
    modules/metabuild_system_module.cpp
    modules/license_module.cpp
    modules/git_module.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${LIBGIT2_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Threads::Threads
    ${LIBGIT2_LIBRARIES}
    CLI11::CLI11
)

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

# Add compiler warnings (but not as errors to avoid potential build issues)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
)

# Add a custom target to print compiler information during build
add_custom_target(print_compiler_info
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling with: $<$<BOOL:${CMAKE_CXX_COMPILER}>:${CMAKE_CXX_COMPILER}>$<$<NOT:$<BOOL:${CMAKE_CXX_COMPILER}>>:default compiler>"
    COMMENT "Compiler information:"
)

# Make sure the custom target runs before the main build
add_dependencies(${PROJECT_NAME} print_compiler_info)

# Installation
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
