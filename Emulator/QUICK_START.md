# Monster8 - Quick Start Guide

## 🚀 Compilation Ultra-Rapide

### Linux
```bash
./build.sh
```

### Windows
```cmd
build.bat
```

C'est tout ! L'exécutable sera dans `build/Monster8` (Linux) ou `build\Monster8.exe` (Windows).

---

## 🎮 Exécution

### Linux
```bash
./build/Monster8
```

### Windows
```cmd
build\Monster8.exe
```

---

## 🔧 Commandes Utiles

| Commande | Linux | Windows |
|----------|-------|---------|
| **Build Release** | `./build.sh` | `build.bat` |
| **Build Debug** | `./build.sh debug` | `build.bat debug` |
| **Build & Run** | `./build.sh run` | `build.bat run` |
| **Clean** | `./build.sh clean` | `build.bat clean` |

---

## 📦 Installation des Dépendances

### Linux (Ubuntu/Debian)
```bash
sudo apt install cmake g++ libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
```

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image
```

---

## 🆘 Problème ?

1. **Nettoyer et recompiler**
   ```bash
   ./build.sh clean && ./build.sh
   ```

2. **Vérifier les dépendances**
   ```bash
   cmake --version  # Doit être >= 3.15
   g++ --version    # Doit être installé
   ```

3. **Consulter la documentation complète**
   - `README_BUILD.md` - Guide complet
   - `BUILD_CMAKE.md` - Détails CMake
   - `MIGRATION_CMAKE.md` - Infos techniques

---

## 💡 Astuces

### VS Code
- `Ctrl+Shift+B` : Compiler
- `F5` : Déboguer

### Compilation parallèle
Le script utilise automatiquement tous les cœurs CPU disponibles pour une compilation rapide.

### Build isolé
Tous les fichiers de compilation sont dans `build/`, le projet reste propre !

---

**Bon développement ! 🎉**