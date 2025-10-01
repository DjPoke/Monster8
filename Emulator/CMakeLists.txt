cmake_minimum_required(VERSION 3.15)
project(Monster8 VERSION 1.0.0 LANGUAGES CXX)

# Ajouter le dossier cmake aux modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Configuration C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de build
option(BUILD_DEBUG "Build with debug symbols and sanitizers" OFF)

# Fichiers sources
set(SOURCES
    main.cpp
    M8.cpp
)

set(HEADERS
    M8.hpp
    sdl_config.h
)

# Créer l'exécutable
if(WIN32)
    # Sur Windows, ajouter le fichier de ressources et créer une application Windows
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} resource.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

# Trouver SDL2 et ses extensions
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Inclure les répertoires SDL2
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

# Lier les bibliothèques SDL2
if(WIN32)
    # Configuration spécifique Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE
        mingw32
        SDL2::SDL2main
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        SDL2_image::SDL2_image
    )
    # Définir SDL_main pour Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_HANDLED)
    # Flags de linkage Windows
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
else()
    # Configuration Linux/Unix
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        SDL2_image::SDL2_image
    )
endif()

# Flags de compilation communs
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g -O1>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

# Flags de linkage pour Release (strip symbols)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        target_link_options(${PROJECT_NAME} PRIVATE -s)
    else()
        target_link_options(${PROJECT_NAME} PRIVATE -s)
    endif()
endif()

# Sanitizers pour Debug sur Linux
if(BUILD_DEBUG AND UNIX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
    )
endif()

# Définir le nom de sortie selon la plateforme
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "Monster8"
        SUFFIX ".exe"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "Monster8"
    )
endif()

# Afficher les informations de configuration
message(STATUS "Monster8 Emulator Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if(BUILD_DEBUG)
    message(STATUS "  Debug Mode: Enabled (with sanitizers on Linux)")
endif()

# Copier automatiquement l'exécutable vers le répertoire racine après la compilation
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_BINARY_DIR}/../$<TARGET_FILE_NAME:${PROJECT_NAME}>
    COMMENT "Copie de l'exécutable vers le répertoire racine..."
)