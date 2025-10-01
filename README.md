# Monster8
Monster8 is a Fantasy Console.

There are two programs:
- The fantasy console itself (emulator)
- An assembler editor

## 🔨 Building

Monster8 Editor can be compiled on both **Linux** and **Windows** using **CMake**.

For detailed build instructions, see [BUILD.md](BUILD.md).

### Quick Start

**Linux:**
```bash
./build.sh
./Monster8Editor
```

**Windows (MSYS2):**
```bash
./build.sh
./Monster8Editor.exe
```

**Manual CMake build:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
cd ..
./Monster8Editor  # Executable is automatically copied to project root
```

### Requirements
- CMake 3.12+
- GTK 3.0
- GtkSourceView 3.0
- HarfBuzz
- C++17 compatible compiler (GCC 7+)
- pkg-config

See [BUILD.md](BUILD.md) for complete installation and compilation instructions.
