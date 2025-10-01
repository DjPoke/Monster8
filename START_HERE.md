# 🚀 COMMENCEZ ICI !

## Vous voulez compiler Monster8 Editor ?

### 🐧 Sur Linux

```bash
# 1. Installez les dépendances
sudo apt-get install build-essential pkg-config libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev

# 2. Compilez
./build.sh

# 3. Lancez
./Monster8Editor
```

**C'est tout !** 🎉

---

### 🪟 Sur Windows

```bash
# 1. Installez MSYS2
# Téléchargez depuis : https://www.msys2.org/
# Installez-le, puis ouvrez "MSYS2 MinGW 64-bit"

# 2. Installez les dépendances (dans MSYS2)
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config make
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz

# 3. Naviguez vers le projet (dans MSYS2)
cd /c/Users/VOTRE_NOM/Documents/GitHub/Monster8

# 4. Compilez
./build.sh

# 5. Lancez
./Monster8Editor.exe
```

**C'est tout !** 🎉

---

## 📚 Besoin d'aide ?

- **Démarrage rapide** → [QUICKSTART.md](QUICKSTART.md)
- **Guide complet** → [BUILD.md](BUILD.md)
- **Guide Windows détaillé** → [WINDOWS_SETUP.md](WINDOWS_SETUP.md)
- **Tous les documents** → [DOCS_INDEX.md](DOCS_INDEX.md)

## ❓ Problème ?

### Linux : "Package gtk+-3.0 was not found"
```bash
sudo apt-get install libgtk-3-dev
```

### Windows : "make: command not found"
Vous devez utiliser **"MSYS2 MinGW 64-bit"**, pas PowerShell !

### Autre problème ?
Consultez [BUILD.md](BUILD.md) section "Résolution de problèmes"

---

**Bon développement ! 🎮**