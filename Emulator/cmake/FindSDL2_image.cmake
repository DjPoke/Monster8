# FindSDL2_image.cmake - Module pour trouver SDL2_image

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_SDL2_IMAGE QUIET SDL2_image)
endif()

find_path(SDL2_IMAGE_INCLUDE_DIR
    NAMES SDL_image.h
    HINTS
        ${PC_SDL2_IMAGE_INCLUDEDIR}
        ${PC_SDL2_IMAGE_INCLUDE_DIRS}
    PATH_SUFFIXES SDL2
    PATHS
        /usr/include
        /usr/local/include
        ${SDL2_IMAGE_PATH}/include
)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS
        ${PC_SDL2_IMAGE_LIBDIR}
        ${PC_SDL2_IMAGE_LIBRARY_DIRS}
    PATHS
        /usr/lib
        /usr/local/lib
        ${SDL2_IMAGE_PATH}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image
    REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR
    VERSION_VAR PC_SDL2_IMAGE_VERSION
)

if(SDL2_IMAGE_FOUND)
    set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})
    set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
    
    if(NOT TARGET SDL2_image::SDL2_image)
        add_library(SDL2_image::SDL2_image UNKNOWN IMPORTED)
        set_target_properties(SDL2_image::SDL2_image PROPERTIES
            IMPORTED_LOCATION "${SDL2_IMAGE_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL2_IMAGE_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)