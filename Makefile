# Détection automatique de la plateforme
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
    TARGET := Monster8Editor.exe
    RM := del /Q
    RMDIR := rmdir /S /Q
    WINDRES := windres
    RC_FILE := app.rc
    RC_OBJECT := app.res
    MKDIR := if not exist
    MKDIR_CMD := mkdir
else
    PLATFORM := $(shell uname -s)
    TARGET := Monster8Editor
    RM := rm -f
    RMDIR := rm -rf
    RC_OBJECT :=
    MKDIR := mkdir -p
    MKDIR_CMD :=
endif

# Configuration de build (Debug par défaut)
BUILD_TYPE ?= Debug

# Répertoires de build
BUILD_DIR := .

# Compilateur et flags de base
CXX := g++
CXXFLAGS_BASE := -std=c++17 -Wall

# Flags spécifiques selon le type de build
ifeq ($(BUILD_TYPE),Debug)
    CXXFLAGS := $(CXXFLAGS_BASE) -g -O0 -DDEBUG
else ifeq ($(BUILD_TYPE),Release)
    CXXFLAGS := $(CXXFLAGS_BASE) -O3 -DNDEBUG
else
    $(error BUILD_TYPE must be Debug or Release)
endif

# Utilisation de pkg-config pour obtenir les flags GTK
PKG_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0 harfbuzz)
PKG_LIBS := $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0 harfbuzz)

# Fichiers sources
SOURCES := main.cpp code.cpp functions.cpp gtk.cpp
HEADERS := code.hpp functions.hpp gtk.hpp

# Fichiers objets dans le répertoire de build
OBJECTS := $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

# Cible finale dans le répertoire de build
TARGET_PATH := $(BUILD_DIR)/$(TARGET)

# Objet de ressources dans le répertoire de build
ifeq ($(OS),Windows_NT)
    RC_OBJECT_PATH := $(BUILD_DIR)/$(RC_OBJECT)
else
    RC_OBJECT_PATH :=
endif

# Règle par défaut
all: $(TARGET_PATH)
	@echo "==================================="
	@echo "Build successful for $(PLATFORM)!"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Executable: $(TARGET_PATH)"
	@echo "==================================="

# Compilation de l'exécutable
$(TARGET_PATH): $(OBJECTS) $(RC_OBJECT_PATH)
	@echo "Linking $(TARGET_PATH)..."
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(RC_OBJECT_PATH) -o $@ $(PKG_LIBS)

# Compilation des fichiers objets
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	@echo "Compiling $< ($(BUILD_TYPE))..."
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) -c $< -o $@

# Compilation du fichier de ressources Windows
ifeq ($(OS),Windows_NT)
$(RC_OBJECT_PATH): $(RC_FILE)
	@echo "Compiling Windows resources..."
	$(WINDRES) $(RC_FILE) -O coff -o $(RC_OBJECT_PATH)
endif

# Cibles spécifiques pour Debug et Release
debug:
	@$(MAKE) BUILD_TYPE=Debug

release:
	@$(MAKE) BUILD_TYPE=Release

# Nettoyage du build actuel
clean:
	@echo "Cleaning build files..."
ifeq ($(OS),Windows_NT)
	-$(RM) "$(TARGET_PATH)" 2>nul
	-$(RM) "$(BUILD_DIR)/*.o" 2>nul
	-$(RM) "$(RC_OBJECT_PATH)" 2>nul
else
	$(RM) $(TARGET_PATH)
	$(RM) $(BUILD_DIR)/*.o
	$(RM) $(RC_OBJECT_PATH)
endif
	@echo "Clean complete!"

# Nettoyage de tous les builds
clean-all: clean
	@echo "All builds cleaned!"

# Nettoyage complet (inclut les fichiers de backup)
distclean: clean-all
	@echo "Deep cleaning..."
ifeq ($(OS),Windows_NT)
	-$(RM) *~ 2>nul
	-$(RM) *.bak 2>nul
else
	$(RM) *~ *.bak
endif

# Installation (Linux uniquement)
install: $(TARGET_PATH)
ifeq ($(OS),Windows_NT)
	@echo "Installation not supported on Windows"
	@echo "Copy $(TARGET_PATH) manually to your desired location"
else
	@echo "Installing $(TARGET) to /usr/local/bin..."
	install -m 755 $(TARGET_PATH) /usr/local/bin/
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
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Build Dir: $(BUILD_DIR)"
	@echo "Compiler: $(CXX)"
	@echo "C++ Flags: $(CXXFLAGS)"
	@echo "Target: $(TARGET_PATH)"
	@echo "Sources: $(SOURCES)"
	@echo "==================================="
	@echo "GTK Flags: $(PKG_CFLAGS)"
	@echo "GTK Libs: $(PKG_LIBS)"
	@echo "==================================="

# Aide
help:
	@echo "Monster8 Editor - Makefile Commands"
	@echo "===================================="
	@echo "make          - Build the project (Debug by default)"
	@echo "make all      - Same as 'make'"
	@echo "make debug    - Build Debug version"
	@echo "make release  - Build Release version"
	@echo "make clean    - Remove current build files"
	@echo "make clean-all- Remove all build files (Debug + Release)"
	@echo "make distclean- Deep clean (includes backups)"
	@echo "make install  - Install to system (Linux only)"
	@echo "make uninstall- Remove from system (Linux only)"
	@echo "make info     - Show build configuration"
	@echo "make help     - Show this help message"
	@echo "===================================="
	@echo "Build Type can be specified: make BUILD_TYPE=Release"
	@echo "===================================="

.PHONY: all debug release clean clean-all distclean install uninstall info help