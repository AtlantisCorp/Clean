# File: Main/CMakeLists.txt
# Purpose: Defines the main executable target. 
project(MainProject)

file(GLOB MainSources "Main/src/*.h" "Main/src/*.cpp")
add_executable(Main ${MainSources})

# Links our executable with libCleanCore. 
target_link_libraries(Main CleanCore)
target_compile_features(Main PRIVATE cxx_std_17)

# Sets output directory. 
SET_TARGET_PROPERTIES(Main 
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CLEAN_OUTPUT}/Debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CLEAN_OUTPUT}/Release
)