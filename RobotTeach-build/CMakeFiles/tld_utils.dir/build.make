# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jes/Desktop/project/RobotTeach/RobotTeach

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jes/Desktop/project/RobotTeach/RobotTeach-build

# Include any dependencies generated for this target.
include CMakeFiles/tld_utils.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tld_utils.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tld_utils.dir/flags.make

CMakeFiles/tld_utils.dir/tld_utils.o: CMakeFiles/tld_utils.dir/flags.make
CMakeFiles/tld_utils.dir/tld_utils.o: /home/jes/Desktop/project/RobotTeach/RobotTeach/tld_utils.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jes/Desktop/project/RobotTeach/RobotTeach-build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/tld_utils.dir/tld_utils.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/tld_utils.dir/tld_utils.o -c /home/jes/Desktop/project/RobotTeach/RobotTeach/tld_utils.cpp

CMakeFiles/tld_utils.dir/tld_utils.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tld_utils.dir/tld_utils.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jes/Desktop/project/RobotTeach/RobotTeach/tld_utils.cpp > CMakeFiles/tld_utils.dir/tld_utils.i

CMakeFiles/tld_utils.dir/tld_utils.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tld_utils.dir/tld_utils.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jes/Desktop/project/RobotTeach/RobotTeach/tld_utils.cpp -o CMakeFiles/tld_utils.dir/tld_utils.s

CMakeFiles/tld_utils.dir/tld_utils.o.requires:
.PHONY : CMakeFiles/tld_utils.dir/tld_utils.o.requires

CMakeFiles/tld_utils.dir/tld_utils.o.provides: CMakeFiles/tld_utils.dir/tld_utils.o.requires
	$(MAKE) -f CMakeFiles/tld_utils.dir/build.make CMakeFiles/tld_utils.dir/tld_utils.o.provides.build
.PHONY : CMakeFiles/tld_utils.dir/tld_utils.o.provides

CMakeFiles/tld_utils.dir/tld_utils.o.provides.build: CMakeFiles/tld_utils.dir/tld_utils.o

# Object files for target tld_utils
tld_utils_OBJECTS = \
"CMakeFiles/tld_utils.dir/tld_utils.o"

# External object files for target tld_utils
tld_utils_EXTERNAL_OBJECTS =

/home/jes/Desktop/project/RobotTeach/lib/libtld_utils.a: CMakeFiles/tld_utils.dir/tld_utils.o
/home/jes/Desktop/project/RobotTeach/lib/libtld_utils.a: CMakeFiles/tld_utils.dir/build.make
/home/jes/Desktop/project/RobotTeach/lib/libtld_utils.a: CMakeFiles/tld_utils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library /home/jes/Desktop/project/RobotTeach/lib/libtld_utils.a"
	$(CMAKE_COMMAND) -P CMakeFiles/tld_utils.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tld_utils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tld_utils.dir/build: /home/jes/Desktop/project/RobotTeach/lib/libtld_utils.a
.PHONY : CMakeFiles/tld_utils.dir/build

CMakeFiles/tld_utils.dir/requires: CMakeFiles/tld_utils.dir/tld_utils.o.requires
.PHONY : CMakeFiles/tld_utils.dir/requires

CMakeFiles/tld_utils.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tld_utils.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tld_utils.dir/clean

CMakeFiles/tld_utils.dir/depend:
	cd /home/jes/Desktop/project/RobotTeach/RobotTeach-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jes/Desktop/project/RobotTeach/RobotTeach /home/jes/Desktop/project/RobotTeach/RobotTeach /home/jes/Desktop/project/RobotTeach/RobotTeach-build /home/jes/Desktop/project/RobotTeach/RobotTeach-build /home/jes/Desktop/project/RobotTeach/RobotTeach-build/CMakeFiles/tld_utils.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tld_utils.dir/depend

