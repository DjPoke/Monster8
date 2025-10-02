cmake_minimum_required(VERSION 3.12)
project(Monster8Editor VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Platform detection
if(WIN32)
    set(PLATFORM "Windows")
    set(TARGET_NAME "Monster8Editor")
else()
    set(PLATFORM ${CMAKE_SYSTEM_NAME})
    set(TARGET_NAME "Monster8Editor")
endif()

message(STATUS "=================================")
message(STATUS "Monster8 Editor Build Configuration")
message(STATUS "=================================")
message(STATUS "Platform: ${PLATFORM}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "=================================")

# Find required packages using pkg-config
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GTKSOURCEVIEW3 REQUIRED gtksourceview-3.0)
pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)

# Source files
set(SOURCES
    main.cpp
    code.cpp
    functions.cpp
    gtk_p1.cpp
    gtk_p2.cpp
)

# Header files (for IDE integration)
set(HEADERS
    code.hpp
    functions.hpp
    gtk_p1.hpp
    gtk_p2.hpp
)

# Windows resource file
if(WIN32)
    set(RC_FILE app.rc)
    enable_language(RC)
endif()

# Create executable
if(WIN32)
    add_executable(${TARGET_NAME} WIN32 ${SOURCES} ${HEADERS} ${RC_FILE})
else()
    add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
endif()

# Compiler flags
target_compile_options(${TARGET_NAME} PRIVATE
    -Wall
    $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# Include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${GTK3_INCLUDE_DIRS}
    ${GTKSOURCEVIEW3_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
)

# Link directories
target_link_directories(${TARGET_NAME} PRIVATE
    ${GTK3_LIBRARY_DIRS}
    ${GTKSOURCEVIEW3_LIBRARY_DIRS}
    ${HARFBUZZ_LIBRARY_DIRS}
)

# Link libraries
target_link_libraries(${TARGET_NAME} PRIVATE
    ${GTK3_LIBRARIES}
    ${GTKSOURCEVIEW3_LIBRARIES}
    ${HARFBUZZ_LIBRARIES}
)

# Add compile definitions for libraries
target_compile_options(${TARGET_NAME} PRIVATE
    ${GTK3_CFLAGS_OTHER}
    ${GTKSOURCEVIEW3_CFLAGS_OTHER}
    ${HARFBUZZ_CFLAGS_OTHER}
)

# Copy executable to project root after build
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${CMAKE_SOURCE_DIR}
    COMMENT "Copying ${TARGET_NAME} to project root directory"
)

# Installation rules (Linux only)
if(UNIX AND NOT APPLE)
    install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION bin
    )
    
    message(STATUS "Installation will copy ${TARGET_NAME} to /usr/local/bin (or custom prefix)")
endif()

# Print library information
message(STATUS "=================================")
message(STATUS "GTK3 Include Dirs: ${GTK3_INCLUDE_DIRS}")
message(STATUS "GTK3 Libraries: ${GTK3_LIBRARIES}")
message(STATUS "GtkSourceView3 Include Dirs: ${GTKSOURCEVIEW3_INCLUDE_DIRS}")
message(STATUS "GtkSourceView3 Libraries: ${GTKSOURCEVIEW3_LIBRARIES}")
message(STATUS "HarfBuzz Include Dirs: ${HARFBUZZ_INCLUDE_DIRS}")
message(STATUS "HarfBuzz Libraries: ${HARFBUZZ_LIBRARIES}")
message(STATUS "=================================")

# Build information summary
add_custom_target(info
    COMMAND ${CMAKE_COMMAND} -E echo "==================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Monster8 Editor Build Information"
    COMMAND ${CMAKE_COMMAND} -E echo "==================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Platform: ${PLATFORM}"
    COMMAND ${CMAKE_COMMAND} -E echo "Build Type: ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Build Dir: ${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Compiler: ${CMAKE_CXX_COMPILER}"
    COMMAND ${CMAKE_COMMAND} -E echo "C++ Standard: C++${CMAKE_CXX_STANDARD}"
    COMMAND ${CMAKE_COMMAND} -E echo "Target: ${TARGET_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "==================================="
    VERBATIM
)
