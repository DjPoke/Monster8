# Changements - Support Multiplateforme

## 🎯 Objectif

Permettre la compilation de Monster8 Editor sur **Linux** et **Windows** en utilisant Make.

## ✅ Modifications Effectuées

### 1. Makefile Multiplateforme
- **Fichier** : `Makefile`
- **Changements** :
  - Détection automatique de la plateforme (Linux/Windows)
  - Adaptation des commandes selon l'OS (rm vs del, etc.)
  - Nom d'exécutable adapté (.exe sur Windows)
  - Compilation incrémentale avec fichiers objets
  - Nouvelles cibles : `info`, `help`, `distclean`
  - Installation système (Linux uniquement)

### 2. Script de Build Amélioré
- **Fichier** : `build.sh`
- **Changements** :
  - Détection automatique de la plateforme
  - Vérification des dépendances (make, pkg-config, GTK)
  - Messages d'erreur détaillés avec instructions d'installation
  - Support Linux et Windows (MSYS2)
  - Utilisation optimale des cœurs CPU

### 3. Documentation
- **Nouveau** : `BUILD.md` - Guide complet de compilation
  - Instructions détaillées pour Linux (Ubuntu, Fedora, Arch)
  - Instructions détaillées pour Windows (MSYS2)
  - Résolution de problèmes
  - Commandes Make disponibles

- **Nouveau** : `QUICKSTART.md` - Démarrage rapide
  - Instructions minimales pour compiler rapidement
  - Problèmes courants et solutions

- **Mis à jour** : `README.md`
  - Ajout de la section Build
  - Référence vers BUILD.md
  - Liste des prérequis

### 4. Configuration Git
- **Mis à jour** : `.gitignore`
  - Ignore les exécutables (Monster8Editor, Monster8Editor.exe)
  - Ignore les fichiers objets (*.o)
  - Ignore les headers précompilés (*.gch)
  - Conserve .vscode pour les paramètres du projet

### 5. Fichiers Supprimés
- ❌ `CMakeLists.txt` - Remplacé par Makefile multiplateforme
- ❌ `build.bat` - Remplacé par build.sh qui fonctionne partout

## 🔧 Utilisation

### Compilation Simple
```bash
make
```

### Avec le Script
```bash
./build.sh
```

### Nettoyage
```bash
make clean
```

### Aide
```bash
make help
```

## 📋 Prérequis

### Linux
- build-essential (gcc, g++, make)
- pkg-config
- libgtk-3-dev
- libgtksourceview-3.0-dev
- libharfbuzz-dev

### Windows
- MSYS2 MinGW 64-bit
- mingw-w64-x86_64-gcc
- mingw-w64-x86_64-make
- mingw-w64-x86_64-pkg-config
- mingw-w64-x86_64-gtk3
- mingw-w64-x86_64-gtksourceview3
- mingw-w64-x86_64-harfbuzz

## ✨ Avantages

1. **Un seul Makefile** pour Linux et Windows
2. **Détection automatique** de la plateforme
3. **Compilation incrémentale** (plus rapide)
4. **Messages clairs** en cas d'erreur
5. **Documentation complète** en français
6. **Pas de dépendance à CMake**

## 🎓 Pour les Développeurs

Le Makefile utilise :
- `$(OS)` pour détecter Windows
- `$(shell uname -s)` pour détecter Linux
- Conditions `ifeq` pour adapter les commandes
- `pkg-config` pour trouver GTK automatiquement
- Compilation séparée (*.cpp → *.o → exécutable)

## 📝 Notes

- Le même code source fonctionne sur les deux plateformes
- Aucune modification du code C++ n'a été nécessaire
- Les fichiers .vscode sont conservés pour VS Code
- L'installation système n'est supportée que sur Linux