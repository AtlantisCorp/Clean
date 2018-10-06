# File: Modules/OBJMeshLoader/CMakeLists.txt
# Purpose: Produces libOBJMeshLoader module. 
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Adds sources for all platform (this is a generic loader).
FILE(GLOB OBJLoaderSources "Modules/OBJMeshLoader/All/*.h" "Modules/OBJMeshLoader/All/*.cpp")
ADD_LIBRARY(OBJMeshLoader SHARED ${OBJLoaderSources})
TARGET_LINK_LIBRARIES(OBJMeshLoader PRIVATE CleanCore)

# Set C++17 flag and output directory to 'bin/Modules'.
TARGET_COMPILE_FEATURES(OBJMeshLoader PRIVATE cxx_std_17)
SET_TARGET_PROPERTIES(OBJMeshLoader PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CLEAN_OUTPUT}/Debug/Modules
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CLEAN_OUTPUT}/Release/Modules)