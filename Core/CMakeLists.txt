# File: Core/CMakeLists.txt
# Purpose: Produces libCleanCore library. 

project(CleanCoreProject)

# Creates our library from all files inside Core/src. 
file(GLOB CoreSources "Core/src/Clean/*.h" "Core/src/Clean/*.cpp")
add_library(CleanCore SHARED ${CoreSources})

# Sets C++11 features enabled.
target_compile_features(CleanCore PRIVATE cxx_std_17)

# Sets output directory.
set_target_properties(CleanCore 
    PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY ${CLEAN_OUTPUT}
)

