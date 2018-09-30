# File: CMake/CleanMacros.cmake
# Purpose: Actually creates CLEAN_PLATFORM_*, CLEAN_LANG_* and CLEAN_WINDOW_* definitions
#       for every project that includes this file. Notes that this file should be included
#       once in the main CMakeFileLists.txt file, in the root directory. 

# CLEAN_PLATFORM_WIN32
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(CLEAN_PLATFORM_WIN32)
    add_compile_definitions(CLEAN_WINDOW_WIN32)
    set(CLEAN_WINDOW_WIN32 TRUE)
    
# CLEAN_PLATFORM_LINUX
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(CLEAN_PLATFORM_LINUX)
    
# CLEAN_PLATFORM_MACOS
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_definitions(CLEAN_PLATFORM_MACOS)
    add_compile_definitions(CLEAN_WINDOW_COCOA)
    set(CLEAN_WINDOW_COCOA TRUE)
    
endif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

# Current language for a file is determined by including Core/Language.h. It will set our
# CLEAN_LANG_* macro automatically, based on compiler macro. Basically we have only two choices: 
# either C++ or OBJ-C++. 

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    
    # Tries to find a valid window system. We have X11, Wayland or KMS/DRM (experimental, i actually
    # want to draw something with it but it might be complicated).
    
    find_package(X11)
    
    if(X11_FOUND)
        # Defines X11 as a compile definition. 
        add_compile_definitions(CLEAN_WINDOW_X11)
        set(CLEAN_WINDOW_X11 TRUE)
    endif(X11_FOUND)
    
    find_package(Wayland)
    
    if (WAYLAND_FOUND)
        # Defines Wayland as a compile definition.
        add_compile_definitions(CLEAN_WINDOW_WAYLAND)
        set(CLEAN_WINDOW_WAYLAND TRUE)
    endif(WAYLAND_FOUND)
    
    find_package(Libdrm)
    
    if (Libdrm_FOUND)
        # Defines Libdrm as a compile definition.
        add_compile_definitions(CLEAN_WINDOW_LIBDRM)
        set(CLEAN_WINDOW_LIBDRM TRUE)
    endif(Libdrm_FOUND)
    
endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")