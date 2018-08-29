# File: Core/CMakeLists.txt
# Purpose: Produces libCleanCore library. 

project(CleanCoreProject)

# Creates our library from all files inside Core/src. 
file(GLOB CoreSources "src/Clean/*.h" "src/Clean/*.cpp")
add_library(CleanCore SHARED ${CoreSources})

# Sets C++11 features enabled.
target_compile_features(CleanCore PRIVATE cxx_std_11)

# Sets output directory.
set_target_properties(CleanCore 
    PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY ${CLEAN_OUTPUT})
)

