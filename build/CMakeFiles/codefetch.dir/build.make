# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/honeyguide/Desktop/MyOpenSource/codefetch

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/honeyguide/Desktop/MyOpenSource/codefetch/build

# Include any dependencies generated for this target.
include CMakeFiles/codefetch.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/codefetch.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/codefetch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/codefetch.dir/flags.make

CMakeFiles/codefetch.dir/codefetch.cpp.o: CMakeFiles/codefetch.dir/flags.make
CMakeFiles/codefetch.dir/codefetch.cpp.o: /home/honeyguide/Desktop/MyOpenSource/codefetch/codefetch.cpp
CMakeFiles/codefetch.dir/codefetch.cpp.o: CMakeFiles/codefetch.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/honeyguide/Desktop/MyOpenSource/codefetch/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/codefetch.dir/codefetch.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/codefetch.dir/codefetch.cpp.o -MF CMakeFiles/codefetch.dir/codefetch.cpp.o.d -o CMakeFiles/codefetch.dir/codefetch.cpp.o -c /home/honeyguide/Desktop/MyOpenSource/codefetch/codefetch.cpp

CMakeFiles/codefetch.dir/codefetch.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/codefetch.dir/codefetch.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/honeyguide/Desktop/MyOpenSource/codefetch/codefetch.cpp > CMakeFiles/codefetch.dir/codefetch.cpp.i

CMakeFiles/codefetch.dir/codefetch.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/codefetch.dir/codefetch.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/honeyguide/Desktop/MyOpenSource/codefetch/codefetch.cpp -o CMakeFiles/codefetch.dir/codefetch.cpp.s

CMakeFiles/codefetch.dir/file_utils.cpp.o: CMakeFiles/codefetch.dir/flags.make
CMakeFiles/codefetch.dir/file_utils.cpp.o: /home/honeyguide/Desktop/MyOpenSource/codefetch/file_utils.cpp
CMakeFiles/codefetch.dir/file_utils.cpp.o: CMakeFiles/codefetch.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/honeyguide/Desktop/MyOpenSource/codefetch/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/codefetch.dir/file_utils.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/codefetch.dir/file_utils.cpp.o -MF CMakeFiles/codefetch.dir/file_utils.cpp.o.d -o CMakeFiles/codefetch.dir/file_utils.cpp.o -c /home/honeyguide/Desktop/MyOpenSource/codefetch/file_utils.cpp

CMakeFiles/codefetch.dir/file_utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/codefetch.dir/file_utils.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/honeyguide/Desktop/MyOpenSource/codefetch/file_utils.cpp > CMakeFiles/codefetch.dir/file_utils.cpp.i

CMakeFiles/codefetch.dir/file_utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/codefetch.dir/file_utils.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/honeyguide/Desktop/MyOpenSource/codefetch/file_utils.cpp -o CMakeFiles/codefetch.dir/file_utils.cpp.s

# Object files for target codefetch
codefetch_OBJECTS = \
"CMakeFiles/codefetch.dir/codefetch.cpp.o" \
"CMakeFiles/codefetch.dir/file_utils.cpp.o"

# External object files for target codefetch
codefetch_EXTERNAL_OBJECTS =

codefetch: CMakeFiles/codefetch.dir/codefetch.cpp.o
codefetch: CMakeFiles/codefetch.dir/file_utils.cpp.o
codefetch: CMakeFiles/codefetch.dir/build.make
codefetch: CMakeFiles/codefetch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/honeyguide/Desktop/MyOpenSource/codefetch/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable codefetch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/codefetch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/codefetch.dir/build: codefetch
.PHONY : CMakeFiles/codefetch.dir/build

CMakeFiles/codefetch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/codefetch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/codefetch.dir/clean

CMakeFiles/codefetch.dir/depend:
	cd /home/honeyguide/Desktop/MyOpenSource/codefetch/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/honeyguide/Desktop/MyOpenSource/codefetch /home/honeyguide/Desktop/MyOpenSource/codefetch /home/honeyguide/Desktop/MyOpenSource/codefetch/build /home/honeyguide/Desktop/MyOpenSource/codefetch/build /home/honeyguide/Desktop/MyOpenSource/codefetch/build/CMakeFiles/codefetch.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/codefetch.dir/depend

