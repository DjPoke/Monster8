#!/bin/bash
# Build script for Monster8 Editor (CMake version)
# Works on Linux and Windows (MSYS2/MinGW)

echo "=== Monster8 Editor Build Script ==="
echo ""

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN_BUILD=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug|-d)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release|-r)
            BUILD_TYPE="Release"
            shift
            ;;
        --clean|-c)
            CLEAN_BUILD=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  -d, --debug     Build in Debug mode (default: Release)"
            echo "  -r, --release   Build in Release mode"
            echo "  -c, --clean     Clean build directory before building"
            echo "  -h, --help      Show this help message"
            echo ""
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Detect platform
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows (MSYS2)"
    EXECUTABLE="Monster8Editor.exe"
else
    PLATFORM="Linux"
    EXECUTABLE="Monster8Editor"
fi

echo "Platform detected: $PLATFORM"
echo "Build type: $BUILD_TYPE"
echo ""

# Check if cmake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: 'cmake' command not found!"
    echo ""
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        echo "Please install CMake in MSYS2:"
        echo "  pacman -S mingw-w64-x86_64-cmake"
    else
        echo "Please install CMake:"
        echo "  sudo apt-get install cmake         # Ubuntu/Debian"
        echo "  sudo dnf install cmake             # Fedora"
        echo "  sudo pacman -S cmake               # Arch"
    fi
    exit 1
fi

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

# Build directory
BUILD_DIR="build"

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo ""
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
echo ""
cd "$BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

if [ $? -ne 0 ]; then
    echo ""
    echo "==================================="
    echo "✗ CMake configuration failed!"
    echo "==================================="
    exit 1
fi

echo ""
echo "Building Monster8 Editor..."
echo ""

# Use all available CPU cores
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    JOBS=$NUMBER_OF_PROCESSORS
else
    JOBS=$(nproc 2>/dev/null || echo 4)
fi

cmake --build . --config $BUILD_TYPE -j $JOBS

# Check if build was successful
if [ $? -eq 0 ]; then
    cd ..
    echo ""
    echo "==================================="
    echo "✓ Build successful!"
    echo "==================================="
    echo "Build Type: $BUILD_TYPE"
    echo "Executable: ./$EXECUTABLE (copied to project root)"
    echo "Build directory: $BUILD_DIR/$EXECUTABLE"
    echo ""
    echo "To run:"
    echo "  ./$EXECUTABLE"
    echo ""
    if [[ "$OSTYPE" != "msys" && "$OSTYPE" != "win32" ]]; then
        echo "To install system-wide:"
        echo "  cd $BUILD_DIR && sudo cmake --install ."
        echo ""
    fi
else
    cd ..
    echo ""
    echo "==================================="
    echo "✗ Build failed!"
    echo "==================================="
    echo ""
    echo "Please check the error messages above."
    exit 1
fi