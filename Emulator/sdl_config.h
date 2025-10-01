// SDL2 Configuration Header
// This file handles cross-platform SDL2 include paths

#ifndef SDL_CONFIG_H
#define SDL_CONFIG_H

// Try to detect the correct SDL2 include path
#if defined(_WIN32) || defined(_WIN64)
    // Windows: Try SDL2/ subdirectory first (MSYS2/MinGW standard)
    #if __has_include(<SDL2/SDL.h>)
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_mixer.h>
        #include <SDL2/SDL_image.h>
    #elif __has_include(<SDL.h>)
        #include <SDL.h>
        #include <SDL_mixer.h>
        #include <SDL_image.h>
    #else
        #error "SDL2 headers not found. Please install SDL2 development libraries."
    #endif
#else
    // Linux/Unix: Standard include path
    #if __has_include(<SDL2/SDL.h>)
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_mixer.h>
        #include <SDL2/SDL_image.h>
    #elif __has_include(<SDL.h>)
        #include <SDL.h>
        #include <SDL_mixer.h>
        #include <SDL_image.h>
    #else
        #error "SDL2 headers not found. Please install SDL2 development libraries."
    #endif
#endif

#endif // SDL_CONFIG_H