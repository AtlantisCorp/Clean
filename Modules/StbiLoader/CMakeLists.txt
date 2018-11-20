# File: Modules/StbiLoader/CMakeLists.txt
# Purpose: Produces StbiLoader module to load any images supported by STB Image.
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Adds sources for all platform (this is a generic loader).
FILE(GLOB STBILoaderSources "Modules/StbiLoader/All/*.h" "Modules/StbiLoader/All/*.cpp")
ADD_LIBRARY(STBILoader SHARED ${STBILoaderSources})
TARGET_LINK_LIBRARIES(STBILoader PRIVATE CleanCore)

# Set C++17 flag and output directory to 'bin/Modules'.
TARGET_COMPILE_FEATURES(STBILoader PRIVATE cxx_std_17)
SET_TARGET_PROPERTIES(STBILoader PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CLEAN_OUTPUT}/Debug/Modules
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CLEAN_OUTPUT}/Release/Modules)