# File: Main/CMakeLists.txt
# Purpose: Defines the main executable target. 
project(MainProject)

file(GLOB MainSources "src/*.h" "src/*.cpp")
add_executable(Main ${MainSources})

# Links our executable with libCleanCore. 
target_link_libraries(Main CleanCore)

# Sets output directory. 
set_target_properties(Main 
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CLEAN_OUTPUT}
)