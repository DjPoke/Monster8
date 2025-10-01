# Changelog - Migration vers CMake

## 📅 Date : Octobre 2024

## 🎯 Objectif
Migration du système de build de Makefile vers CMake pour une meilleure portabilité et intégration IDE.

---

## ✅ Fichiers Ajoutés

### Système de Build
- ✅ `CMakeLists.txt` - Configuration CMake principale
- ✅ `cmake/FindSDL2.cmake` - Module pour trouver SDL2
- ✅ `cmake/FindSDL2_mixer.cmake` - Module pour trouver SDL2_mixer
- ✅ `cmake/FindSDL2_image.cmake` - Module pour trouver SDL2_image

### Scripts de Build
- ✅ `build.sh` - Script de build rapide pour Linux
- ✅ `build.bat` - Script de build rapide pour Windows

### Documentation
- ✅ `BUILD_CMAKE.md` - Guide détaillé d'utilisation de CMake
- ✅ `MIGRATION_CMAKE.md` - Documentation de la migration
- ✅ `README_BUILD.md` - Guide de build rapide
- ✅ `CHANGELOG_CMAKE.md` - Ce fichier

### Configuration
- ✅ `.gitignore` - Ignore les fichiers de build

---

## 🗑️ Fichiers Supprimés

- ❌ `Makefile` - Remplacé par CMakeLists.txt

---

## 🔄 Fichiers Modifiés

### Configuration VS Code
- 🔧 `.vscode/c_cpp_properties.json`
  - Ajout de la configuration Linux
  - Configuration pour utiliser CMake Tools
  - Chemins d'include mis à jour

- 🔧 `.vscode/tasks.json`
  - Remplacement des tâches Make par des tâches CMake
  - Nouvelles tâches : Configure, Build Debug/Release, Clean, Build and Run
  - Support multiplateforme amélioré

- 🔧 `.vscode/launch.json`
  - Mise à jour des chemins d'exécutable vers `build/Monster8`
  - Mise à jour des preLaunchTask pour utiliser CMake

---

## 🚀 Nouvelles Fonctionnalités

### Build System
1. **Détection automatique de la plateforme**
   - Linux et Windows supportés nativement
   - Configuration SDL2 automatique via pkg-config

2. **Builds isolés**
   - Tous les fichiers de build dans le dossier `build/`
   - Projet source reste propre

3. **Compilation parallèle optimisée**
   - Utilisation de tous les cœurs CPU disponibles
   - Build plus rapide

4. **Support des sanitizers (Linux)**
   - Option `-DBUILD_DEBUG=ON` pour activer AddressSanitizer
   - Aide à détecter les bugs mémoire

### Scripts de Build
1. **build.sh (Linux)**
   - Commandes simples : `./build.sh release`, `./build.sh debug`, `./build.sh run`
   - Affichage coloré et informatif
   - Gestion automatique du dossier build

2. **build.bat (Windows)**
   - Équivalent Windows du script Linux
   - Syntaxe simple et cohérente

### Intégration IDE
1. **VS Code**
   - Support natif de CMake Tools
   - IntelliSense amélioré
   - Tâches de build intégrées

2. **CLion / Visual Studio**
   - Détection automatique du projet CMake
   - Configuration zéro

---

## 📊 Comparaison Makefile vs CMake

| Aspect | Makefile | CMake |
|--------|----------|-------|
| **Portabilité** | Manuelle | Automatique |
| **Détection SDL2** | Manuelle | Automatique |
| **IDE Support** | Limité | Excellent |
| **Build isolé** | Non | Oui (dossier build/) |
| **Compilation parallèle** | Manuelle (-j) | Automatique |
| **Configuration** | Hardcodée | Détectée |
| **Standard industrie** | Ancien | Moderne |

---

## 🔧 Configuration Technique

### CMake
- **Version minimale** : 3.15
- **Standard C++** : C++17
- **Générateur** : Unix Makefiles (Linux) / MinGW Makefiles (Windows)

### Compilateur
- **Linux** : GCC 13.3.0+
- **Windows** : MinGW-w64 GCC 15.2.0+

### Dépendances
- SDL2 2.30.0+
- SDL2_mixer 2.8.0+
- SDL2_image 2.8.2+

---

## 📝 Instructions de Migration

### Pour les développeurs

1. **Supprimer les anciens builds**
   ```bash
   rm -f Monster8 Monster8.exe *.o
   ```

2. **Utiliser le nouveau système**
   ```bash
   ./build.sh release
   ```

3. **Dans VS Code**
   - Installer l'extension "CMake Tools"
   - Utiliser `Ctrl+Shift+B` pour compiler

### Pour les utilisateurs

Aucun changement ! L'exécutable fonctionne de la même manière.

---

## ✅ Tests Effectués

- ✅ Compilation Release sur Linux Ubuntu 24.04
- ✅ Compilation Debug sur Linux Ubuntu 24.04
- ✅ Détection automatique de SDL2
- ✅ Scripts build.sh fonctionnels
- ✅ Tâches VS Code fonctionnelles
- ✅ Configuration de débogage VS Code
- ✅ IntelliSense C++ fonctionnel

---

## 🔮 Prochaines Étapes

- [ ] Tester la compilation sur Windows avec MSYS2
- [ ] Vérifier le fichier resource.rc sur Windows
- [ ] Tester le script build.bat
- [ ] Créer un package pour distribution

---

## 📞 Support

En cas de problème avec le nouveau système de build :
1. Consulter `BUILD_CMAKE.md` pour le guide détaillé
2. Consulter `MIGRATION_CMAKE.md` pour les détails techniques
3. Vérifier que toutes les dépendances sont installées

---

## 🎉 Résumé

La migration vers CMake est **complète et fonctionnelle** sur Linux. Le système est maintenant :
- ✅ Plus moderne
- ✅ Plus portable
- ✅ Plus facile à utiliser
- ✅ Mieux intégré aux IDEs
- ✅ Plus maintenable

**Le Makefile a été supprimé avec succès !**