cmake_minimum_required(VERSION 3.10)
project(Monster8 VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Source files
set(SOURCES
    main.cpp
    M8.cpp
)

set(HEADERS
    M8.hpp
    sdl_config.h
)

# Create executable
add_executable(Monster8 ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(Monster8 PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(Monster8 PRIVATE
    ${SDL2_LIBRARIES}
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(Monster8 PRIVATE SDL_MAIN_HANDLED)
    
    # Copy SDL2 DLLs to output directory on Windows
    if(MSVC)
        # For MSVC, copy DLLs after build
        add_custom_command(TARGET Monster8 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:Monster8>
        )
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    target_link_libraries(Monster8 PRIVATE pthread dl)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(Monster8 PRIVATE /W4)
else()
    target_compile_options(Monster8 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Monster8 PRIVATE DEBUG)
    if(NOT MSVC)
        # Enable AddressSanitizer on Linux/GCC for debug builds
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(Monster8 PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(Monster8 PRIVATE -fsanitize=address)
        endif()
    endif()
else()
    target_compile_definitions(Monster8 PRIVATE NDEBUG)
    if(NOT MSVC)
        target_link_options(Monster8 PRIVATE -s)  # Strip symbols
    endif()
endif()

# Installation
install(TARGETS Monster8 DESTINATION bin)
install(FILES LoadScreen.png DESTINATION bin)
install(DIRECTORY Cartridges DESTINATION bin)

# Print configuration
message(STATUS "Monster8 Emulator Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  SDL2 found: ${SDL2_FOUND}")
message(STATUS "  SDL2 include: ${SDL2_INCLUDE_DIRS}")
message(STATUS "  SDL2 libraries: ${SDL2_LIBRARIES}")