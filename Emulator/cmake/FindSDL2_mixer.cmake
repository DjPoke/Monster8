# FindSDL2_mixer.cmake - Module pour trouver SDL2_mixer

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_SDL2_MIXER QUIET SDL2_mixer)
endif()

find_path(SDL2_MIXER_INCLUDE_DIR
    NAMES SDL_mixer.h
    HINTS
        ${PC_SDL2_MIXER_INCLUDEDIR}
        ${PC_SDL2_MIXER_INCLUDE_DIRS}
    PATH_SUFFIXES SDL2
    PATHS
        /usr/include
        /usr/local/include
        ${SDL2_MIXER_PATH}/include
)

find_library(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
    HINTS
        ${PC_SDL2_MIXER_LIBDIR}
        ${PC_SDL2_MIXER_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        ${SDL2_MIXER_PATH}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_mixer
    REQUIRED_VARS SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR
    VERSION_VAR PC_SDL2_MIXER_VERSION
)

if(SDL2_MIXER_FOUND)
    set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})
    set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
    
    if(NOT TARGET SDL2_mixer::SDL2_mixer)
        add_library(SDL2_mixer::SDL2_mixer UNKNOWN IMPORTED)
        set_target_properties(SDL2_mixer::SDL2_mixer PROPERTIES
            IMPORTED_LOCATION "${SDL2_MIXER_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(SDL2_MIXER_INCLUDE_DIR SDL2_MIXER_LIBRARY)