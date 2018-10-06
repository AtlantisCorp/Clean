# File: Core/CMakeLists.txt
# Purpose: Produces libCleanCore library. 
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

# Creates our library from all files inside Core/src. 
FILE(GLOB CoreSources "Core/src/Clean/*.h" "Core/src/Clean/*.cpp")
ADD_LIBRARY(CleanCore SHARED ${CoreSources})

# Sets C++17 features enabled.
TARGET_COMPILE_FEATURES(CleanCore PRIVATE cxx_std_17)

# Sets output directory.
SET_TARGET_PROPERTIES(CleanCore 
    PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CLEAN_OUTPUT}/Debug
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CLEAN_OUTPUT}/Release
)

