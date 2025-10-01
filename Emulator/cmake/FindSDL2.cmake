# FindSDL2.cmake - Module pour trouver SDL2
# Ce module définit:
#  SDL2_FOUND - Si SDL2 a été trouvé
#  SDL2_INCLUDE_DIRS - Les répertoires d'include de SDL2
#  SDL2_LIBRARIES - Les bibliothèques à lier
#  SDL2::SDL2 - Cible importée

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_SDL2 QUIET sdl2)
endif()

find_path(SDL2_INCLUDE_DIR
    NAMES SDL.h
    HINTS
        ${PC_SDL2_INCLUDEDIR}
        ${PC_SDL2_INCLUDE_DIRS}
    PATH_SUFFIXES SDL2
    PATHS
        /usr/include
        /usr/local/include
        ${SDL2_PATH}/include
)

find_library(SDL2_LIBRARY
    NAMES SDL2
    HINTS
        ${PC_SDL2_LIBDIR}
        ${PC_SDL2_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        ${SDL2_PATH}/lib
)

if(WIN32)
    find_library(SDL2_MAIN_LIBRARY
        NAMES SDL2main
        HINTS
            ${PC_SDL2_LIBDIR}
            ${PC_SDL2_LIBRARY_DIRS}
        PATHS
            ${SDL2_PATH}/lib
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2
    REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR
    VERSION_VAR PC_SDL2_VERSION
)

if(SDL2_FOUND)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIR})
    set(SDL2_LIBRARIES ${SDL2_LIBRARY})
    
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 UNKNOWN IMPORTED)
        set_target_properties(SDL2::SDL2 PROPERTIES
            IMPORTED_LOCATION "${SDL2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
        )
    endif()
    
    if(WIN32 AND SDL2_MAIN_LIBRARY)
        if(NOT TARGET SDL2::SDL2main)
            add_library(SDL2::SDL2main UNKNOWN IMPORTED)
            set_target_properties(SDL2::SDL2main PROPERTIES
                IMPORTED_LOCATION "${SDL2_MAIN_LIBRARY}"
            )
        endif()
    endif()
endif()

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY SDL2_MAIN_LIBRARY)