# Monster8 Editor - Démarrage Rapide

## 🚀 Compilation Rapide

### Linux

```bash
# Installer les dépendances (Ubuntu/Debian)
sudo apt-get install build-essential pkg-config libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev

# Compiler
./build.sh

# Ou directement avec make
make

# Exécuter
./Monster8Editor
```

### Windows (MSYS2)

```bash
# 1. Installer MSYS2 depuis https://www.msys2.org/

# 2. Ouvrir "MSYS2 MinGW 64-bit" et installer les dépendances
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config make
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz

# 3. Compiler
./build.sh

# Ou directement avec make
make

# 4. Exécuter
./Monster8Editor.exe
```

## 📚 Documentation Complète

Pour plus de détails, consultez [BUILD.md](BUILD.md)

## ❓ Problèmes Courants

**Linux : "Package gtk+-3.0 was not found"**
```bash
sudo apt-get install libgtk-3-dev
```

**Windows : "make: command not found"**
```bash
pacman -S make
```

**Windows : Utiliser le bon terminal**
- ✅ Utilisez "MSYS2 MinGW 64-bit"
- ❌ N'utilisez PAS "MSYS2 MSYS" ou "MSYS2 UCRT64"