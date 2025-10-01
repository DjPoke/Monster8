---
description: Repository Information Overview
alwaysApply: true
---

# Monster8 Information

## Summary
Monster8 appears to be a C++ emulator project, likely for an 8-bit system or fantasy console. The code suggests it implements a virtual machine with memory, screen buffer, audio, and input capabilities. The project is structured as a simple C++ application without external build systems.

## Structure
The repository has a minimal structure with just the core emulator implementation files:
- `M8.cpp` - Main implementation file containing the emulator logic
- `M8.hpp` - Header file defining the Monster8 class and its components
- `Monster8.code-workspace` - VS Code workspace configuration

## Language & Runtime
**Language**: C++
**Standard**: Modern C++ (using std::array, suggesting C++11 or later)
**Build System**: None specified (likely manual compilation)

## Dependencies
**Standard Library Components**:
- `<iostream>` - For input/output operations
- `<array>` - For fixed-size array containers
- `<fstream>` - For file operations
- `<vector>` - For dynamic array containers
- `<stdint.h>` - For fixed-width integer types

## Build & Installation
No explicit build system is defined. The project likely requires manual compilation with a C++ compiler:

```bash
g++ -std=c++11 M8.cpp -o Monster8
```

## Main Components
**Emulator Core**:
- `Monster8` class - Main emulator implementation
- Memory management (65536 bytes of addressable memory)
- Screen buffer (65536 bytes for display output)
- Audio buffer (65536 bytes for sound output)
- Input handling via joystick array
- CPU emulation with registers (A and D registers)
- Stack implementation (4096 words)
- Timer system (delay and sound timers)

**Key Features**:
- ROM loading capability
- Built-in 8x8 VGA font set (ASCII 0x00-0x7F)
- Cycle-based emulation

## Development Environment
**IDE**: Visual Studio Code
**File Associations**:
- `.cpp` files associated with C++ language
- Specific associations for `iostream`, `array`, and `iosfwd` headers