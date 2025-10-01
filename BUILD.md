# Monster8 Editor - Guide de Compilation

Ce guide explique comment compiler Monster8 Editor sur Linux et Windows en utilisant **Make**.

## 📋 Prérequis

### Linux (Ubuntu/Debian)

```bash
# Installer les dépendances
sudo apt-get update
sudo apt-get install build-essential pkg-config
sudo apt-get install libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev
```

### Linux (Fedora/RHEL)

```bash
# Installer les dépendances
sudo dnf install gcc-c++ make pkg-config
sudo dnf install gtk3-devel gtksourceview3-devel harfbuzz-devel
```

### Linux (Arch)

```bash
# Installer les dépendances
sudo pacman -S base-devel pkg-config
sudo pacman -S gtk3 gtksourceview3 harfbuzz
```

### Windows (MSYS2)

1. **Télécharger et installer MSYS2** depuis https://www.msys2.org/

2. **Ouvrir "MSYS2 MinGW 64-bit"** depuis le menu Démarrer

3. **Installer les dépendances** :
```bash
# Mettre à jour MSYS2
pacman -Syu

# Installer les outils de build
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-pkg-config
pacman -S make

# Installer GTK3 et dépendances
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-gtksourceview3
pacman -S mingw-w64-x86_64-harfbuzz
```

## 🔨 Compilation

### Méthode 1 : Utiliser le script de build (Recommandé)

Le script `build.sh` détecte automatiquement votre plateforme et compile le projet.

#### Linux
```bash
chmod +x build.sh
./build.sh
```

#### Windows (MSYS2)
```bash
# Dans le terminal MSYS2 MinGW 64-bit
chmod +x build.sh
./build.sh
```

### Méthode 2 : Utiliser Make directement

#### Linux et Windows (MSYS2)
```bash
# Compilation simple
make

# Compilation avec tous les cœurs CPU
make -j$(nproc)  # Linux
make -j$NUMBER_OF_PROCESSORS  # Windows MSYS2
```

### Commandes Make disponibles

```bash
make          # Compiler le projet
make clean    # Nettoyer les fichiers de build
make distclean# Nettoyage complet (inclut les backups)
make install  # Installer (Linux uniquement)
make uninstall# Désinstaller (Linux uniquement)
make info     # Afficher la configuration
make help     # Afficher l'aide
```

## 🚀 Exécution

### Linux
```bash
./Monster8Editor
```

### Windows (MSYS2)
```bash
./Monster8Editor.exe
```

**Note Windows** : Pour exécuter l'application en dehors de MSYS2, vous devez soit :
- Ajouter `C:\msys64\mingw64\bin` à votre PATH système
- Copier les DLLs GTK nécessaires dans le même dossier que l'exécutable

## 📦 Installation (Linux uniquement)

```bash
# Installation système
sudo make install

# Désinstallation
sudo make uninstall
```

Cela installera l'exécutable dans `/usr/local/bin/`.

### Windows
L'installation système n'est pas supportée. Copiez simplement `Monster8Editor.exe` où vous voulez.

## 🧹 Nettoyage

```bash
# Nettoyer les fichiers objets et l'exécutable
make clean

# Nettoyage complet (inclut les fichiers de backup)
make distclean
```

## 🐛 Résolution de problèmes

### Linux : "Package gtk+-3.0 was not found"
```bash
# Vérifier que pkg-config trouve GTK
pkg-config --modversion gtk+-3.0

# Si non trouvé, réinstaller
sudo apt-get install --reinstall libgtk-3-dev
```

### Windows : "pkg-config not found"
- Assurez-vous d'utiliser le terminal **MSYS2 MinGW 64-bit** (pas MSYS2 MSYS)
- Vérifiez que pkg-config est installé : `pacman -S mingw-w64-x86_64-pkg-config`

### Windows : "make: command not found"
```bash
# Installer make dans MSYS2
pacman -S make
```

### Windows : "Cannot find -lgtk-3"
- Réinstallez GTK3 : `pacman -S mingw-w64-x86_64-gtk3`
- Vérifiez que vous utilisez MinGW 64-bit (pas 32-bit)

### Erreurs de compilation C++17
- Vérifiez votre version de GCC : `g++ --version` (doit être >= 7.0)
- Sur Ubuntu ancien : `sudo apt-get install g++-9` puis utilisez `CXX=g++-9 make`

### Windows : L'exécutable ne démarre pas en dehors de MSYS2
Vous devez ajouter les DLLs GTK au PATH ou les copier :
```bash
# Option 1 : Ajouter au PATH (temporaire)
export PATH="/mingw64/bin:$PATH"

# Option 2 : Copier les DLLs nécessaires
ldd Monster8Editor.exe  # Liste les DLLs requises
```

## 📝 Notes

- **Makefile multiplateforme** : Le même Makefile fonctionne sur Linux et Windows (MSYS2)
- **Détection automatique** : Le Makefile détecte automatiquement votre plateforme
- **Compilation incrémentale** : Seuls les fichiers modifiés sont recompilés
- **Cross-compilation** : Non supportée actuellement

## 🔗 Liens utiles

- [GTK Documentation](https://docs.gtk.org/gtk3/)
- [MSYS2 Documentation](https://www.msys2.org/docs/what-is-msys2/)
- [GNU Make Manual](https://www.gnu.org/software/make/manual/)