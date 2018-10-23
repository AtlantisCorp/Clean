# File: JSONMapperLoader/CMakeLists.txt
# Purpose: Provides CMake project generation. 
# 
# Uses this project's file to include JSONMapperLoader module to your
# Clean's Project. It will be delivered directly to bin/Modules. 
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Adds sources for all platform (this is a generic loader).
FILE(GLOB JSONMapperLoaderSources "Modules/JSONMapperLoader/All/*.h" "Modules/JSONMapperLoader/All/*.cpp")
ADD_LIBRARY(JSONMapperLoader SHARED ${JSONMapperLoaderSources})
TARGET_LINK_LIBRARIES(JSONMapperLoader PRIVATE CleanCore)

# Tries to find RapidJSON library. 
LIST(INSERT CMAKE_MODULE_PATH 0 "Modules/JSONMapperLoader/CMake")
FIND_PACKAGE(RapidJSON REQUIRED MODULE)
TARGET_LINK_LIBRARIES(JSONMapperLoader PUBLIC RapidJSON::RapidJSON)

# Set C++17 flag and output directory to 'bin/Modules'.
TARGET_COMPILE_FEATURES(JSONMapperLoader PRIVATE cxx_std_17)
SET_TARGET_PROPERTIES(JSONMapperLoader PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CLEAN_OUTPUT}/Debug/Modules
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CLEAN_OUTPUT}/Release/Modules)