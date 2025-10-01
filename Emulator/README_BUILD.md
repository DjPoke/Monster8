# Monster8 Emulator - Guide de Build

## 🚀 Build Rapide

### Linux
```bash
./build.sh          # Build Release (par défaut)
./build.sh debug    # Build Debug
./build.sh run      # Build et exécuter
./build.sh clean    # Nettoyer
```

### Windows (MSYS2/MinGW)
```cmd
build.bat           REM Build Release (par défaut)
build.bat debug     REM Build Debug
build.bat run       REM Build et exécuter
build.bat clean     REM Nettoyer
```

## 📋 Prérequis

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake g++ libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
```

### Windows (MSYS2/MinGW64)
```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer \
          mingw-w64-x86_64-SDL2_image
```

## 🔧 Build Manuel avec CMake

### Configuration
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Compilation
```bash
cmake --build . -j$(nproc)    # Linux
cmake --build . -j 8          # Windows
```

### Exécution
```bash
./build/Monster8              # Linux
build\Monster8.exe            # Windows
```

## 🎯 Options de Build

- **Release** : Optimisé, sans symboles de débogage (par défaut)
- **Debug** : Avec symboles de débogage, optimisation minimale
- **Debug + Sanitizers** (Linux uniquement) :
  ```bash
  cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_DEBUG=ON
  ```

## 📁 Structure du Projet

```
Monster8/
├── CMakeLists.txt          # Configuration CMake principale
├── cmake/                  # Modules CMake pour trouver SDL2
├── build/                  # Dossier de build (généré, ignoré par git)
│   └── Monster8            # Exécutable compilé
├── build.sh                # Script de build Linux
├── build.bat               # Script de build Windows
├── main.cpp                # Point d'entrée
├── M8.cpp / M8.hpp         # Implémentation de l'émulateur
└── .vscode/                # Configuration VS Code
```

## 🔨 Intégration IDE

### Visual Studio Code
1. Installer l'extension **CMake Tools**
2. Ouvrir le dossier du projet
3. Utiliser `Ctrl+Shift+B` pour compiler
4. Utiliser `F5` pour déboguer

### CLion
- Ouvre automatiquement le projet via CMakeLists.txt

### Visual Studio (Windows)
- Ouvrir le dossier contenant CMakeLists.txt

## 🧹 Nettoyage

```bash
./build.sh clean    # Linux
build.bat clean     # Windows
# ou manuellement :
rm -rf build/       # Linux
rmdir /s /q build   # Windows
```

## 📚 Documentation Complète

- **BUILD_CMAKE.md** : Guide détaillé d'utilisation de CMake
- **MIGRATION_CMAKE.md** : Détails de la migration depuis Makefile

## ✅ Système Testé

- ✅ Linux Ubuntu 24.04 LTS
- ✅ GCC 13.3.0
- ✅ CMake 3.28+
- ✅ SDL2 2.30.0, SDL2_mixer 2.8.0, SDL2_image 2.8.2

## 🆘 Problèmes Courants

### SDL2 non trouvé
```bash
# Linux
sudo apt install libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image
```

### CMake trop ancien
```bash
# Vérifier la version
cmake --version

# Minimum requis : 3.15
```

### Erreur de compilation
```bash
# Nettoyer et recompiler
./build.sh clean
./build.sh release
```