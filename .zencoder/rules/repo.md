---
description: Repository Information Overview
alwaysApply: true
---

# Monster8 Editor Information

## Summary
Monster8 Editor is an assembler editor for the Monster8 fantasy console, built with C++ and GTK 3. It provides code editing with syntax highlighting, project management, and resource handling (sprites, tilesets, sounds, music).

## Structure
- **main.cpp**: Main application entry point
- **gtk.cpp/gtk.hpp**: GTK UI implementation and dialogs
- **code.cpp/code.hpp**: Code processing and assembly
- **functions.cpp/functions.hpp**: Utility functions
- **CMakeLists.txt**: CMake build configuration
- **build.sh**: Automated build script
- **.vscode/**: VS Code configuration files
  - **c_cpp_properties.json**: C/C++ extension configuration
  - **tasks.json**: Build tasks configuration
  - **launch.json**: Debug configurations
- **app.rc**: Windows resource file (icon, version info)
- **BUILD.md**: Detailed build instructions
- **README.md**: Project documentation

## Language & Runtime
**Language**: C++
**Version**: C++17
**Build System**: CMake 3.12+
**Dependencies**: GTK 3.0, GtkSourceView 3.0, HarfBuzz

## Dependencies
**Main Dependencies**:
- GTK 3.0 (libgtk-3-dev)
- GLib 2.0
- Pango
- Cairo
- GDK-Pixbuf

## Build & Installation
```bash
# Quick build with script (recommended)
./build.sh
./Monster8Editor  # Executable is auto-copied to project root

# Or manual CMake build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
cd .. && ./Monster8Editor

# Debug build
./build.sh --debug

# Installation (Linux only)
cd build && sudo cmake --install .
```

**Note**: The executable is automatically copied from `build/` to the project root after each successful compilation.

## Usage
The application provides a simple text editor with a toolbar containing formatting buttons:
- Bold: Applies bold formatting to selected text
- Italic: Applies italic formatting to selected text
- Underline: Applies underline formatting to selected text