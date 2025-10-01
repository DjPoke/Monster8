# Guide de Build - Monster8 Editor

## Vue d'ensemble

Le projet Monster8 Editor utilise **CMake** comme système de build et supporte deux configurations :
- **Debug** : Version avec symboles de débogage, sans optimisation (-g -O0 -DDEBUG)
- **Release** : Version optimisée pour la production (-O3 -DNDEBUG)

## Prérequis

### Tous les systèmes
- **CMake** 3.12 ou supérieur
- **Compilateur C++17** (GCC 7+, Clang, ou MSVC)
- **pkg-config**

### Bibliothèques requises
- GTK+ 3.0
- GtkSourceView 3.0
- HarfBuzz
- GLib 2.0
- Cairo
- Pango

## Installation des dépendances

### Windows (MSYS2)
```bash
# Installer les outils de build
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc make

# Installer les bibliothèques
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz
```

### Linux (Ubuntu/Debian)
```bash
# Installer les outils de build
sudo apt-get install cmake build-essential pkg-config

# Installer les bibliothèques
sudo apt-get install libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev
```

### Linux (Fedora)
```bash
# Installer les outils de build
sudo dnf install cmake gcc-c++ make pkg-config

# Installer les bibliothèques
sudo dnf install gtk3-devel gtksourceview3-devel harfbuzz-devel
```

### Linux (Arch)
```bash
# Installer les outils de build
sudo pacman -S cmake base-devel pkg-config

# Installer les bibliothèques
sudo pacman -S gtk3 gtksourceview3 harfbuzz
```

## Copie automatique de l'exécutable

Après chaque compilation réussie, l'exécutable est **automatiquement copié** du dossier `build/` vers la racine du projet. Cela permet de :
- Exécuter directement `./Monster8Editor` sans avoir à naviguer dans le dossier `build/`
- Faciliter le développement et les tests rapides
- Maintenir une compatibilité avec les scripts et workflows existants

Cette copie est effectuée automatiquement par CMake après chaque build.

## Compilation rapide avec build.sh

Le script `build.sh` est la méthode la plus simple pour compiler le projet :

```bash
# Build Release (par défaut)
./build.sh

# Build Debug
./build.sh --debug
# ou
./build.sh -d

# Build Release explicite
./build.sh --release
# ou
./build.sh -r

# Clean build (supprime le dossier build avant de compiler)
./build.sh --clean
# ou
./build.sh -c

# Combiner les options
./build.sh --debug --clean

# Afficher l'aide
./build.sh --help
```

## Compilation manuelle avec CMake

### Méthode standard (build dans un dossier séparé)

```bash
# Créer le dossier de build
mkdir build
cd build

# Configurer le projet (Release par défaut)
cmake ..

# Ou spécifier le type de build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compiler
cmake --build .

# Ou avec make directement (plus rapide avec plusieurs cœurs)
make -j$(nproc)
```

### Build avec type spécifique

```bash
# Debug
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j$(nproc)

# Release
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### Nettoyage

```bash
# Nettoyer les fichiers compilés (garde la configuration CMake)
cmake --build . --target clean

# Nettoyage complet (supprimer tout le dossier build)
cd ..
rm -rf build
```

### Installation (Linux uniquement)

```bash
# Installer dans /usr/local/bin (nécessite sudo)
cd build
sudo cmake --install .

# Ou spécifier un préfixe personnalisé
cmake -DCMAKE_INSTALL_PREFIX=/opt/monster8 ..
sudo cmake --install .
```

## Structure des répertoires

```
Monster8/
├── build/                      # Dossier de build (ignoré par Git)
│   ├── CMakeCache.txt
│   ├── CMakeFiles/
│   ├── Monster8Editor          # Exécutable (Linux)
│   └── Monster8Editor.exe      # Exécutable (Windows)
├── Monster8Editor              # Copie automatique de l'exécutable (Linux)
├── Monster8Editor.exe          # Copie automatique de l'exécutable (Windows)
├── CMakeLists.txt              # Configuration CMake
├── build.sh                    # Script de build automatique
├── main.cpp                    # Sources
├── code.cpp
├── functions.cpp
├── gtk.cpp
└── app.rc                      # Ressources Windows
```

**Note**: L'exécutable est automatiquement copié du dossier `build/` vers la racine du projet après chaque compilation.

## Compilation dans VS Code

### Tâches disponibles

Le fichier `.vscode/tasks.json` devrait être mis à jour pour utiliser CMake. Vous pouvez utiliser l'extension **CMake Tools** pour VS Code :

1. Installer l'extension "CMake Tools" depuis le marketplace VS Code
2. Ouvrir la palette de commandes (`Ctrl+Shift+P`)
3. Taper "CMake: Configure"
4. Sélectionner le type de build (Debug/Release)
5. Compiler avec `F7` ou via la palette "CMake: Build"

### Configuration manuelle des tâches

Si vous préférez configurer manuellement, ajoutez ces tâches dans `.vscode/tasks.json` :

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Configure (Debug)",
            "type": "shell",
            "command": "cmake",
            "args": ["-DCMAKE_BUILD_TYPE=Debug", "-S", ".", "-B", "build"],
            "group": "build"
        },
        {
            "label": "CMake Build",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build", "-j", "4"],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

## Débogage dans VS Code

### Configuration de lancement

Mettez à jour `.vscode/launch.json` pour pointer vers l'exécutable dans le dossier `build/` :

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Monster8Editor",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Monster8Editor",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "preLaunchTask": "CMake Build"
        }
    ]
}
```

## Différences entre Debug et Release

### Version Debug
- **Optimisation** : Aucune (-O0)
- **Symboles de débogage** : Activés (-g)
- **Définitions** : `DEBUG` défini
- **Taille** : Plus grande
- **Performance** : Plus lente
- **Usage** : Développement et débogage

### Version Release
- **Optimisation** : Maximum (-O3)
- **Symboles de débogage** : Désactivés
- **Définitions** : `NDEBUG` défini
- **Taille** : Plus petite
- **Performance** : Optimale
- **Usage** : Distribution et production

## Plateformes supportées

### Windows (MSYS2/MinGW)
- Compilateur : GCC (MinGW-w64)
- Exécutable : `build/Monster8Editor.exe`
- Ressources : `app.rc` compilé automatiquement par CMake

### Linux
- Compilateur : GCC ou Clang
- Exécutable : `build/Monster8Editor`
- Installation : `sudo cmake --install .` dans le dossier `build/`

## Commandes utiles CMake

```bash
# Afficher les informations de configuration
cmake -LAH build/

# Compiler en mode verbose (voir toutes les commandes)
cmake --build build/ --verbose

# Nettoyer et recompiler
rm -rf build/ && mkdir build && cd build && cmake .. && make -j$(nproc)

# Exécuter une cible personnalisée
cmake --build build/ --target info
```

## Résolution de problèmes

### CMake non trouvé
Assurez-vous que CMake est installé et dans votre PATH :
```bash
# Vérifier l'installation
cmake --version

# Linux : installer CMake
sudo apt-get install cmake    # Ubuntu/Debian
sudo dnf install cmake         # Fedora
sudo pacman -S cmake           # Arch

# Windows : installer via MSYS2
pacman -S mingw-w64-x86_64-cmake
```

### Erreurs pkg-config
Si CMake ne trouve pas GTK :
```bash
# Vérifier que pkg-config fonctionne
pkg-config --modversion gtk+-3.0

# Si non trouvé, vérifier que les bibliothèques sont installées
# Voir la section "Installation des dépendances" ci-dessus
```

### Erreurs de compilation
1. Supprimer le dossier `build/` complètement
2. Reconfigurer et recompiler :
   ```bash
   rm -rf build
   ./build.sh --clean
   ```

### Erreurs de lancement dans VS Code
1. Vérifiez que le build a réussi
2. Vérifiez que le chemin vers l'exécutable est correct dans `launch.json`
3. Pour Windows, assurez-vous d'utiliser MSYS2 terminal dans VS Code

## Avantages de CMake vs Make

- ✅ Génération automatique des Makefiles
- ✅ Meilleure gestion des dépendances
- ✅ Support natif multi-plateforme
- ✅ Intégration IDE améliorée (VS Code, CLion, Visual Studio)
- ✅ Configuration plus lisible et maintenable
- ✅ Support des générateurs multiples (Make, Ninja, Visual Studio, etc.)

## Notes importantes

- Le dossier `build/` est ignoré par Git (voir `.gitignore`)
- Les fichiers CMake temporaires sont également ignorés
- CMake génère automatiquement les dépendances
- Vous pouvez avoir plusieurs dossiers de build (par ex. `build-debug/`, `build-release/`)
- Les ressources Windows (app.rc) sont gérées automatiquement par CMake