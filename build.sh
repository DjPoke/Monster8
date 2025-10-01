#!/bin/bash
# Build script for Monster8 Editor
# Works on Linux and Windows (MSYS2/MinGW)

echo "=== Monster8 Editor Build Script ==="
echo ""

# Detect platform
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows (MSYS2)"
    EXECUTABLE="Monster8Editor.exe"
else
    PLATFORM="Linux"
    EXECUTABLE="Monster8Editor"
fi

echo "Platform detected: $PLATFORM"
echo ""

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "ERROR: 'make' command not found!"
    echo ""
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        echo "Please install make in MSYS2:"
        echo "  pacman -S make"
    else
        echo "Please install make:"
        echo "  sudo apt-get install build-essential  # Ubuntu/Debian"
        echo "  sudo dnf install make gcc-c++         # Fedora"
        echo "  sudo pacman -S base-devel             # Arch"
    fi
    exit 1
fi

# Check if pkg-config is available
if ! command -v pkg-config &> /dev/null; then
    echo "ERROR: 'pkg-config' not found!"
    echo ""
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        echo "Please install pkg-config in MSYS2:"
        echo "  pacman -S mingw-w64-x86_64-pkg-config"
    else
        echo "Please install pkg-config:"
        echo "  sudo apt-get install pkg-config"
    fi
    exit 1
fi

# Check if GTK is available
if ! pkg-config --exists gtk+-3.0; then
    echo "ERROR: GTK 3.0 not found!"
    echo ""
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        echo "Please install GTK3 in MSYS2:"
        echo "  pacman -S mingw-w64-x86_64-gtk3"
        echo "  pacman -S mingw-w64-x86_64-gtksourceview3"
        echo "  pacman -S mingw-w64-x86_64-harfbuzz"
    else
        echo "Please install GTK3 development files:"
        echo "  sudo apt-get install libgtk-3-dev libgtksourceview-3.0-dev  # Ubuntu/Debian"
        echo "  sudo dnf install gtk3-devel gtksourceview3-devel            # Fedora"
        echo "  sudo pacman -S gtk3 gtksourceview3                          # Arch"
    fi
    exit 1
fi

echo "All dependencies found!"
echo ""

# Clean previous build
echo "Cleaning previous build..."
make clean

# Build
echo ""
echo "Building Monster8 Editor..."
echo ""

# Use all available CPU cores
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    JOBS=$NUMBER_OF_PROCESSORS
else
    JOBS=$(nproc 2>/dev/null || echo 4)
fi

make -j$JOBS

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "==================================="
    echo "✓ Build successful!"
    echo "==================================="
    echo "Executable: ./$EXECUTABLE"
    echo ""
    echo "To run:"
    echo "  ./$EXECUTABLE"
    echo ""
    if [[ "$OSTYPE" != "msys" && "$OSTYPE" != "win32" ]]; then
        echo "To install system-wide:"
        echo "  sudo make install"
        echo ""
    fi
else
    echo ""
    echo "==================================="
    echo "✗ Build failed!"
    echo "==================================="
    echo ""
    echo "Please check the error messages above."
    exit 1
fi