# Makefile for Monster8 Emulator
# Cross-platform build for Windows and Linux

# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

# Compiler
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

# Source files
SOURCES := main.cpp M8.cpp
HEADERS := M8.hpp

# Output executable
ifeq ($(DETECTED_OS),Windows)
    TARGET := Monster8.exe
    RM := del /Q
    RM_DIR := rmdir /S /Q
else
    TARGET := Monster8
    RM := rm -f
    RM_DIR := rm -rf
endif

# SDL2 configuration
ifeq ($(DETECTED_OS),Windows)
    # Windows: Try pkg-config first, fallback to manual paths
    # Check if pkg-config is available
    PKG_CONFIG := $(shell where pkg-config 2>nul)
    ifneq ($(PKG_CONFIG),)
        # Use pkg-config if available
        SDL2_CFLAGS := $(shell pkg-config --cflags sdl2) -Dmain=SDL_main
        SDL2_LIBS := -lmingw32 $(shell pkg-config --libs sdl2) -lSDL2_mixer -lSDL2_image -mwindows
    else
        # Fallback to manual paths (MSYS2/MinGW default location)
        # Adjust these paths if SDL2 is installed elsewhere
        SDL2_CFLAGS := -IC:/msys64/mingw64/include/SDL2 -Dmain=SDL_main
        SDL2_LIBS := -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -mwindows
    endif
else
    # Linux/Unix: Use sdl2-config or pkg-config
    SDL2_CONFIG := $(shell which sdl2-config 2>/dev/null)
    ifneq ($(SDL2_CONFIG),)
        SDL2_CFLAGS := $(shell sdl2-config --cflags)
        SDL2_LIBS := $(shell sdl2-config --libs) -lSDL2_mixer -lSDL2_image
    else
        SDL2_CFLAGS := $(shell pkg-config --cflags sdl2)
        SDL2_LIBS := $(shell pkg-config --libs sdl2) -lSDL2_mixer -lSDL2_image
    endif
endif

# Build configurations
.PHONY: all debug release clean help

all: release

# Debug build with sanitizers (Linux only for sanitizers)
debug: CXXFLAGS += -O1 -g -DDEBUG
ifeq ($(DETECTED_OS),Linux)
    debug: CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
    debug: LDFLAGS += -fsanitize=address
endif
debug: $(TARGET)

# Release build
release: CXXFLAGS += -O2 -DNDEBUG
ifeq ($(DETECTED_OS),Windows)
    release: LDFLAGS += -s
else
    release: LDFLAGS += -s
endif
release: $(TARGET)

# Build target
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) $(SOURCES) -o $(TARGET) $(SDL2_LIBS) $(LDFLAGS)

# Clean build artifacts
clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist $(TARGET) $(RM) $(TARGET)
	@if exist *.o $(RM) *.o
else
	$(RM) $(TARGET) *.o
endif

# Help
help:
	@echo "Monster8 Emulator - Makefile"
	@echo "Detected OS: $(DETECTED_OS)"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build release version (default)"
	@echo "  make release  - Build optimized release version"
	@echo "  make debug    - Build debug version with symbols"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "SDL2 Configuration:"
	@echo "  CFLAGS: $(SDL2_CFLAGS)"
	@echo "  LIBS: $(SDL2_LIBS)"