# Détection automatique de la plateforme
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    TARGET := Monster8Editor.exe
    RM := del /Q
    RMDIR := rmdir /S /Q
else
    PLATFORM := $(shell uname -s)
    TARGET := Monster8Editor
    RM := rm -f
    RMDIR := rm -rf
endif

# Compilateur et flags
CXX := g++
CXXFLAGS := -std=c++17 -g -Wall

# Utilisation de pkg-config pour obtenir les flags GTK
PKG_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0 harfbuzz)
PKG_LIBS := $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0 harfbuzz)

# Fichiers sources
SOURCES := main.cpp code.cpp functions.cpp gtk.cpp
HEADERS := code.hpp functions.hpp gtk.hpp

# Fichiers objets
OBJECTS := $(SOURCES:.cpp=.o)

# Règle par défaut
all: $(TARGET)
	@echo "==================================="
	@echo "Build successful for $(PLATFORM)!"
	@echo "Executable: $(TARGET)"
	@echo "==================================="

# Compilation de l'exécutable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $^ -o $@ $(PKG_LIBS)

# Compilation des fichiers objets
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# Nettoyage
clean:
	@echo "Cleaning build files..."
ifeq ($(OS),Windows_NT)
	-$(RM) *.o 2>nul
	-$(RM) $(TARGET) 2>nul
	-$(RM) *.gch 2>nul
else
	$(RM) $(OBJECTS) $(TARGET) *.gch
endif
	@echo "Clean complete!"

# Nettoyage complet (inclut les fichiers de backup)
distclean: clean
	@echo "Deep cleaning..."
ifeq ($(OS),Windows_NT)
	-$(RM) *~ 2>nul
	-$(RM) *.bak 2>nul
else
	$(RM) *~ *.bak
endif

# Installation (Linux uniquement)
install: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo "Installation not supported on Windows"
	@echo "Copy $(TARGET) manually to your desired location"
else
	@echo "Installing $(TARGET) to /usr/local/bin..."
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installation complete!"
endif

# Désinstallation (Linux uniquement)
uninstall:
ifeq ($(OS),Windows_NT)
	@echo "Uninstall not applicable on Windows"
else
	@echo "Removing $(TARGET) from /usr/local/bin..."
	rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstall complete!"
endif

# Afficher les informations de configuration
info:
	@echo "==================================="
	@echo "Monster8 Editor Build Configuration"
	@echo "==================================="
	@echo "Platform: $(PLATFORM)"
	@echo "Compiler: $(CXX)"
	@echo "C++ Flags: $(CXXFLAGS)"
	@echo "Target: $(TARGET)"
	@echo "Sources: $(SOURCES)"
	@echo "==================================="
	@echo "GTK Flags: $(PKG_CFLAGS)"
	@echo "GTK Libs: $(PKG_LIBS)"
	@echo "==================================="

# Aide
help:
	@echo "Monster8 Editor - Makefile Commands"
	@echo "===================================="
	@echo "make          - Build the project"
	@echo "make all      - Same as 'make'"
	@echo "make clean    - Remove build files"
	@echo "make distclean- Deep clean (includes backups)"
	@echo "make install  - Install to system (Linux only)"
	@echo "make uninstall- Remove from system (Linux only)"
	@echo "make info     - Show build configuration"
	@echo "make help     - Show this help message"
	@echo "===================================="

.PHONY: all clean distclean install uninstall info help