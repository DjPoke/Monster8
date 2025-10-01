# Changements pour la compilation cross-platform

## Fichiers créés

### 1. `Makefile`
- Détection automatique du système d'exploitation (Windows/Linux)
- Support de pkg-config et sdl2-config
- Cibles: `release`, `debug`, `clean`, `help`
- Configurations optimisées pour chaque plateforme

### 2. `sdl_config.h`
- Header de configuration pour SDL2
- Détection automatique du chemin d'inclusion correct
- Support de `<SDL2/SDL.h>` (Linux) et `<SDL.h>` (Windows)
- Utilise `__has_include` pour la détection

### 3. `BUILD.md`
- Instructions de compilation complètes
- Prérequis pour Windows (MSYS2/MinGW) et Linux
- Guide de dépannage
- Exemples de compilation manuelle

## Fichiers modifiés

### 1. `M8.hpp`
- Remplacé `#include <SDL2/SDL.h>` et `#include <SDL2/SDL_mixer.h>`
- Par `#include "sdl_config.h"`
- Assure la compatibilité cross-platform

### 2. `main.cpp`
- Remplacé les includes SDL2 directs
- Par `#include "sdl_config.h"`
- Assure la compatibilité cross-platform

### 3. `.vscode/tasks.json`
- Ajout de tâches spécifiques Windows et Linux
- Support de `mingw32-make` (Windows) et `make` (Linux)
- Tâches: Build Debug, Build Release, Clean, Build and Run

### 4. `.vscode/launch.json`
- Configurations de débogage séparées pour Windows et Linux
- Support de gdb sur les deux plateformes
- Chemins corrects pour les exécutables (.exe sur Windows)

## Comment compiler

### Windows (avec MSYS2/MinGW installé)
```bash
mingw32-make release
```

### Linux
```bash
make release
```

### VS Code
Appuyez sur `Ctrl+Shift+B` et sélectionnez "Build Monster8 (Release)"

## Avantages

1. **Portabilité**: Le même code compile sur Windows et Linux sans modification
2. **Automatique**: Détection automatique de l'OS et des chemins SDL2
3. **Flexible**: Support de plusieurs méthodes d'installation de SDL2
4. **Simple**: Une seule commande pour compiler
5. **IDE intégré**: Support complet dans VS Code

## Notes techniques

- Le Makefile utilise `$(OS)` pour détecter Windows
- Le Makefile essaie pkg-config en premier, puis les chemins par défaut
- `sdl_config.h` utilise `__has_include` (C++17) pour la détection
- Les sanitizers (AddressSanitizer) sont activés uniquement sur Linux en mode debug