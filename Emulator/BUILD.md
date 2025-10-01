# Monster8 Emulator - Build Instructions

This document explains how to build the Monster8 emulator on Windows and Linux.

## Prerequisites

### Windows

1. **MSYS2/MinGW-w64** (recommended)
   - Download and install from: https://www.msys2.org/
   - Open MSYS2 MinGW 64-bit terminal
   - Install required packages:
     ```bash
     pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image
     ```

2. **Add MinGW to PATH**
   - Add `C:\msys64\mingw64\bin` to your system PATH
   - Verify with: `g++ --version` and `mingw32-make --version`

### Linux

Install development tools and SDL2 libraries:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential g++ make libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
```

**Fedora:**
```bash
sudo dnf install gcc-c++ make SDL2-devel SDL2_mixer-devel SDL2_image-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel sdl2 sdl2_mixer sdl2_image
```

## Building

### Using Makefile

The Makefile automatically detects your operating system and uses the appropriate commands.

**Build release version (optimized):**
```bash
# Windows (PowerShell or CMD)
mingw32-make release

# Linux
make release
```

**Build debug version (with debug symbols):**
```bash
# Windows
mingw32-make debug

# Linux
make debug
```

**Clean build artifacts:**
```bash
# Windows
mingw32-make clean

# Linux
make clean
```

**Show help:**
```bash
# Windows
mingw32-make help

# Linux
make help
```

### Using VS Code

The project includes VS Code tasks for building:

1. Open the project folder in VS Code
2. Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on macOS)
3. Select one of the build tasks:
   - **Build Monster8 (Release)** - Default, optimized build
   - **Build Monster8 (Debug)** - Debug build with symbols
   - **Clean Build** - Remove build artifacts
   - **Build and Run (Release)** - Build and immediately run

### Manual Compilation

If you prefer to compile manually:

**Windows:**
```bash
g++ -std=c++17 -O2 -DNDEBUG -IC:/msys64/mingw64/include/SDL2 -Dmain=SDL_main main.cpp M8.cpp -o Monster8.exe -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -mwindows -s
```

**Linux:**
```bash
g++ -std=c++17 -O2 -DNDEBUG $(sdl2-config --cflags) main.cpp M8.cpp -o Monster8 $(sdl2-config --libs) -lSDL2_mixer -lSDL2_image -s
```

## Running

After building, run the emulator:

**Windows:**
```bash
.\Monster8.exe
```

**Linux:**
```bash
./Monster8
```

## Troubleshooting

### Windows Issues

**"SDL2.dll not found":**
- Make sure `C:\msys64\mingw64\bin` is in your PATH
- Or copy SDL2 DLLs to the same directory as Monster8.exe

**"mingw32-make: command not found":**
- Install mingw-w64-x86_64-make: `pacman -S mingw-w64-x86_64-make`
- Or use the full path: `C:\msys64\mingw64\bin\mingw32-make.exe`

**"SDL2/SDL.h: No such file or directory":**
- The project uses automatic SDL2 header detection
- Make sure SDL2 is properly installed via MSYS2
- The `sdl_config.h` header will automatically find the correct include path

### Linux Issues

**"SDL2/SDL.h: No such file or directory":**
- Install SDL2 development packages (see Prerequisites above)

**"sdl2-config: command not found":**
- Install libsdl2-dev package

### General Issues

**Compilation errors:**
- Ensure you have C++17 compatible compiler (GCC 7+ or Clang 5+)
- Check that all SDL2 libraries are properly installed

**Linking errors:**
- Verify SDL2, SDL2_mixer, and SDL2_image are installed
- On Windows, check that MinGW libraries are in the correct location

## Project Structure

```
Monster8/Emulator/
├── main.cpp              # Main application entry point
├── M8.cpp                # Monster8 emulator implementation
├── M8.hpp                # Monster8 emulator header
├── sdl_config.h          # Cross-platform SDL2 header configuration
├── Makefile              # Cross-platform build configuration
├── LoadScreen.png        # Splash screen image
├── Cartridges/           # Game cartridge files (.binm8)
└── .vscode/              # VS Code configuration
    ├── tasks.json        # Build tasks
    ├── launch.json       # Debug configurations
    └── c_cpp_properties.json
```

## Additional Notes

- The Makefile automatically detects Windows vs Linux
- The `sdl_config.h` header automatically detects the correct SDL2 include path
- Debug builds on Linux include AddressSanitizer for memory error detection
- Release builds are optimized and stripped of debug symbols
- The emulator expects cartridge files in the `Cartridges/` directory