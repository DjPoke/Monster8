---
description: Repository Information Overview
alwaysApply: true
---

# Monster8 Editor Information

## Summary
Monster8 Editor is a simple rich text editor application built with C++ and GTK 3. It provides basic text editing capabilities with formatting options like bold, italic, and underline.

## Structure
- **main.cpp**: Main application source file
- **CMakeLists.txt**: CMake build configuration
- **.vscode/**: VS Code configuration files
  - **c_cpp_properties.json**: C/C++ extension configuration
  - **tasks.json**: Build tasks configuration
- **README.md**: Project documentation

## Language & Runtime
**Language**: C++
**Version**: C++17
**Build System**: CMake
**Dependencies**: GTK 3.0

## Dependencies
**Main Dependencies**:
- GTK 3.0 (libgtk-3-dev)
- GLib 2.0
- Pango
- Cairo
- GDK-Pixbuf

## Build & Installation
```bash
# Direct compilation
g++ -o Monster8Editor main.cpp `pkg-config --cflags --libs gtk+-3.0`

# Or using CMake
mkdir build && cd build
cmake ..
make
```

## Usage
The application provides a simple text editor with a toolbar containing formatting buttons:
- Bold: Applies bold formatting to selected text
- Italic: Applies italic formatting to selected text
- Underline: Applies underline formatting to selected text