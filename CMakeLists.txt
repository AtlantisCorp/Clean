# File: CMakeLists.txt
# Purpose: Includes other CMake files to generate Main, Core and all Modules. 
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(MainProject)

INCLUDE(CMake/CleanMacros.cmake)

SET(CLEAN_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Includes GLM directory. If not found, we can't compile any Clean project.
FIND_PACKAGE(GLM REQUIRED)
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})
MESSAGE(STATUS "GLM Dir: ${GLM_INCLUDE_DIRS}")

# Includes Core/src directory to compiler directories when looking for file. 
# This way the Main target will see <Clean/Core.h> file form. 
include_directories("Core/src")

# Includes Core and Main CMake files. 
include(Core/CMakeLists.txt)
include(Main/CMakeLists.txt)

# Adds here every CMake files for modules. 
include(Modules/GlDriver/CMakeLists.txt)
include(Modules/OBJMeshLoader/CMakeLists.txt)
include(Modules/JSONMapperLoader/CMakeLists.txt)
