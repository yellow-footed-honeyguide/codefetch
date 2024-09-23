cmake_minimum_required(VERSION 3.12)
   project(codefetch)

   set(CMAKE_CXX_STANDARD 20)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)
   set(CMAKE_CXX_EXTENSIONS OFF)

   # Add indicators
   add_subdirectory(indicators)

   add_executable(codefetch
       codefetch.cpp
       file_utils.cpp
       language_stats.cpp
   )

   target_compile_options(codefetch PRIVATE -Wall -Wextra -Wpedantic -O3)

   find_package(Threads REQUIRED)
   target_link_libraries(codefetch PRIVATE Threads::Threads indicators::indicators)

   if(NOT WIN32)
       target_link_libraries(codefetch PRIVATE stdc++fs)
   endif()

   # Installation
   include(GNUInstallDirs)
   install(TARGETS codefetch
           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
