# Monster8 - Guide de Build avec CMake

Ce projet utilise CMake pour la compilation multiplateforme (Linux et Windows).

## Prérequis

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake g++ libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
```

### Windows (MSYS2/MinGW)
```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image
```

## Compilation

### Build Release (recommandé)
```bash
# Créer un dossier de build
mkdir build
cd build

# Configurer le projet
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compiler
cmake --build .

# L'exécutable sera dans le dossier build/
```

### Build Debug (avec symboles de débogage)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Build Debug avec Sanitizers (Linux uniquement)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_DEBUG=ON
cmake --build .
```

## Nettoyage

Pour nettoyer les fichiers de build :
```bash
rm -rf build/
```

## Exécution

Après la compilation, l'exécutable se trouve dans le dossier `build/` :

### Linux
```bash
./build/Monster8
```

### Windows
```bash
./build/Monster8.exe
```

## Options CMake

- `CMAKE_BUILD_TYPE` : Type de build (`Debug` ou `Release`)
- `BUILD_DEBUG` : Active les sanitizers sur Linux (uniquement en mode Debug)

## Structure du Projet

```
Monster8/
├── CMakeLists.txt       # Configuration CMake
├── main.cpp             # Point d'entrée
├── M8.cpp               # Implémentation de l'émulateur
├── M8.hpp               # En-tête de l'émulateur
├── resource.rc          # Ressources Windows (icône)
├── sdl_config.h         # Configuration SDL
└── build/               # Dossier de build (créé par CMake)
```

## Intégration IDE

### Visual Studio Code
Le projet peut être ouvert directement dans VS Code avec l'extension CMake Tools.

### CLion
CLion détecte automatiquement le fichier CMakeLists.txt.

### Visual Studio (Windows)
Ouvrir le dossier contenant CMakeLists.txt avec "Ouvrir un dossier".