# File: Modules/GlDriver/CMakeLists.txt
# Purpose: Produces libGlDriver module. 
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(GlDriverProject)

# Adds sources depending on window platform.
FILE(GLOB GlDriverSources "Modules/GlDriver/All/*.h" "Modules/GlDriver/All/*.cpp")
ADD_LIBRARY(GlDriver SHARED ${GlDriverSources})
TARGET_LINK_LIBRARIES(GlDriver PRIVATE CleanCore)

IF(CLEAN_WINDOW_WIN32)
    FILE(GLOB GlDriverSourcesWin32 "Modules/GlDriver/Win32/*.h" "Modules/GlDriver/Win32/*.cpp")
    TARGET_SOURCES(GlDriver PUBLIC ${GlDriverSourcesWin32})

ELSEIF(CLEAN_WINDOW_COCOA)
    FILE(GLOB GlDriverSourcesCocoa "Modules/GlDriver/Cocoa/*.h" "Modules/GlDriver/Cocoa/*.mm" "Modules/GlDriver/Cocoa/*.cpp")
    TARGET_SOURCES(GlDriver PUBLIC ${GlDriverSourcesCocoa})
    
    # Adds on Cocoa and OpenGL Framework.
    INCLUDE_DIRECTORIES("/System/Library/Frameworks")
    
    FIND_PACKAGE(OpenGL REQUIRED)
    TARGET_LINK_LIBRARIES(GlDriver PRIVATE OpenGL::GL)
    INCLUDE_DIRECTORIES("${OPENGL_INCLUDE_DIR}/OpenGL")
    MESSAGE(STATUS "OPENGL Dir: ${OPENGL_INCLUDE_DIR}")
    
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    MARK_AS_ADVANCED(COCOA_LIBRARY)
    TARGET_LINK_LIBRARIES(GlDriver PRIVATE ${COCOA_LIBRARY})
    
ELSEIF(CLEAN_WINDOW_X11)
    FILE(GLOB GlDriverSourcesX11 "Modules/GlDriver/X11/*.h" "Modules/GlDriver/X11/*.cpp")
    TARGET_SOURCES(GlDriver PUBLIC ${GlDriverSourcesX11})
    
    FIND_PACKAGE(OpenGL REQUIRED)
    TARGET_LINK_LIBRARIES(GlDriver PRIVATE OpenGL::GL)
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR}/GL)
    MESSAGE(STATUS "OPENGL Dir: ${OPENGL_INCLUDE_DIR}")
    
ENDIF()

# Set C++17 flag and output directory to 'bin/Modules'.
TARGET_COMPILE_FEATURES(GlDriver PRIVATE cxx_std_17)
SET_TARGET_PROPERTIES(GlDriver PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CLEAN_OUTPUT}/Modules)