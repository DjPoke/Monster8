# Migration vers CMake - Résumé des changements

## Changements effectués

### 1. Nouveau système de build
- ✅ **Ajout de CMakeLists.txt** : Configuration CMake multiplateforme (Linux/Windows)
- ✅ **Suppression de Makefile** : Ancien système de build remplacé
- ✅ **Modules CMake personnalisés** : Ajout de FindSDL2.cmake, FindSDL2_mixer.cmake, FindSDL2_image.cmake dans le dossier `cmake/`

### 2. Configuration VS Code mise à jour
- ✅ **c_cpp_properties.json** : Ajout de la configuration Linux + support CMake Tools
- ✅ **tasks.json** : Nouvelles tâches CMake (Configure, Build Debug/Release, Clean, Build and Run)
- ✅ **launch.json** : Configurations de débogage mises à jour pour utiliser le dossier `build/`

### 3. Documentation
- ✅ **BUILD_CMAKE.md** : Guide complet d'utilisation de CMake
- ✅ **.gitignore** : Ajout pour ignorer les fichiers de build

## Avantages de CMake

### Multiplateforme
- Détection automatique de la plateforme (Linux/Windows)
- Configuration automatique des bibliothèques SDL2
- Gestion des différences de compilation entre OS

### Moderne et standard
- CMake est le standard de l'industrie pour C++
- Meilleure intégration avec les IDEs (VS Code, CLion, Visual Studio)
- Support natif de CMake Tools dans VS Code

### Flexibilité
- Builds séparés (dossier `build/` isolé)
- Configurations Debug/Release faciles à gérer
- Support des sanitizers sur Linux
- Compilation parallèle optimisée

## Comment utiliser

### Compilation rapide
```bash
# Configuration + Build en une commande
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)

# Exécution
./Monster8
```

### Dans VS Code
1. Installer l'extension "CMake Tools" (recommandé)
2. Utiliser `Ctrl+Shift+B` pour compiler (tâche par défaut : Release)
3. Utiliser `F5` pour déboguer

### Nettoyage
```bash
rm -rf build/
```

## Structure des fichiers

```
Monster8/
├── CMakeLists.txt              # Configuration principale CMake
├── cmake/                      # Modules CMake personnalisés
│   ├── FindSDL2.cmake
│   ├── FindSDL2_mixer.cmake
│   └── FindSDL2_image.cmake
├── build/                      # Dossier de build (généré, ignoré par git)
│   └── Monster8                # Exécutable compilé
├── .vscode/                    # Configuration VS Code
│   ├── c_cpp_properties.json   # IntelliSense C++
│   ├── tasks.json              # Tâches de build
│   └── launch.json             # Configurations de débogage
├── BUILD_CMAKE.md              # Guide d'utilisation CMake
└── .gitignore                  # Fichiers à ignorer

```

## Compatibilité

### Linux (testé)
- ✅ Ubuntu 24.04 LTS
- ✅ GCC 13.3.0
- ✅ SDL2 2.30.0
- ✅ SDL2_mixer 2.8.0
- ✅ SDL2_image 2.8.2

### Windows (à tester)
- Configuration prête pour MSYS2/MinGW64
- Chemins configurés pour C:/msys64/mingw64/
- Support du fichier resource.rc pour l'icône

## Notes importantes

1. **Dossier build/** : Tous les fichiers de compilation sont maintenant dans `build/`, ce qui garde le projet propre
2. **Configuration automatique** : CMake détecte automatiquement SDL2 via pkg-config
3. **Tâches VS Code** : Les tâches configurent automatiquement avant de compiler
4. **IntelliSense** : Utilise maintenant CMake Tools pour une meilleure détection des symboles

## Prochaines étapes

- [ ] Tester la compilation sur Windows
- [ ] Vérifier que toutes les dépendances sont correctement détectées
- [ ] Mettre à jour la documentation principale si nécessaire