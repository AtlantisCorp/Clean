# File: CMakeLists.txt
# Purpose: Includes other CMake files to generate Main, Core and all Modules. 

project(MainProject)
include(CMake/CleanMacros.cmake)

set(CLEAN_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Includes Core/src directory to compiler directories when looking for file. 
# This way the Main target will see <Clean/Core.h> file form. 
include_directories("Core/src")

# Includes Core and Main CMake files. 
include(Core/CMakeLists.txt)
include(Main/CMakeLists.txt)

# Adds here every CMake files for modules. 
# include(Modules/GlDriver/CMakeLists.txt)