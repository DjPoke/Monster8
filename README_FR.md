# Monster8 Editor - Guide Français

## 🎮 Qu'est-ce que Monster8 ?

Monster8 est une console fantasy. Ce dépôt contient l'éditeur/assembleur pour créer des jeux Monster8.

## 🚀 Démarrage Rapide

### Linux

```bash
# 1. Installer les dépendances (Ubuntu/Debian)
sudo apt-get install build-essential pkg-config libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev

# 2. Compiler
./build.sh

# 3. Exécuter
./Monster8Editor
```

### Windows

```bash
# 1. Installer MSYS2 depuis https://www.msys2.org/

# 2. Ouvrir "MSYS2 MinGW 64-bit" et installer les dépendances
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config make
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz

# 3. Compiler
./build.sh

# 4. Exécuter
./Monster8Editor.exe
```

## 📚 Documentation

| Document | Description |
|----------|-------------|
| **[QUICKSTART.md](QUICKSTART.md)** | Démarrage ultra-rapide |
| **[BUILD.md](BUILD.md)** | Guide complet de compilation |
| **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** | Guide spécifique Windows |
| **[SUMMARY.md](SUMMARY.md)** | Résumé des modifications |

## 🔨 Compilation

### Avec le script (recommandé)
```bash
./build.sh
```

### Avec Make directement
```bash
make              # Compiler
make clean        # Nettoyer
make help         # Aide
make info         # Configuration
```

### Dans VS Code
- Appuyez sur `Ctrl+Shift+B` pour compiler
- Ou utilisez le menu Terminal → Run Build Task

## 📋 Prérequis

### Tous les systèmes
- Compilateur C++ avec support C++17 (GCC 7+)
- Make
- pkg-config
- GTK 3.0
- GtkSourceView 3.0
- HarfBuzz

### Linux
Installez via votre gestionnaire de paquets (apt, dnf, pacman)

### Windows
Installez via MSYS2 (voir [WINDOWS_SETUP.md](WINDOWS_SETUP.md))

## 🎯 Fonctionnalités de l'Éditeur

- Éditeur de code avec coloration syntaxique
- Compilation de code assembleur Monster8
- Gestion de projets
- Import de sprites, tilesets, sons et musiques
- Export de ROM Monster8
- Intégration avec l'émulateur Monster8

## 🗂️ Structure du Projet

```
Monster8/
├── main.cpp              # Point d'entrée principal
├── code.cpp/hpp          # Gestion du code assembleur
├── functions.cpp/hpp     # Fonctions utilitaires
├── gtk.cpp/hpp           # Interface GTK
├── Makefile              # Build multiplateforme
├── build.sh              # Script de compilation
├── textview.css          # Style de l'éditeur
├── Settings.cfg          # Configuration
├── Emulator/             # Émulateur Monster8
├── Games/                # Jeux exemples
└── Palette/              # Palette de couleurs
```

## 🐛 Problèmes Courants

### Linux : "Package gtk+-3.0 was not found"
```bash
sudo apt-get install libgtk-3-dev
```

### Windows : "make: command not found"
Vous devez utiliser le terminal **MSYS2 MinGW 64-bit**, pas PowerShell ou CMD.

### Windows : L'exécutable ne démarre pas
Ajoutez `C:\msys64\mingw64\bin` à votre PATH système.

## 💡 Conseils

- **Linux** : Utilisez `make -j$(nproc)` pour compiler plus rapidement
- **Windows** : Créez un raccourci vers "MSYS2 MinGW 64-bit" pour un accès rapide
- **VS Code** : Les tâches de build sont préconfigurées (Ctrl+Shift+B)
- **Développement** : Utilisez `make clean` avant de recompiler complètement

## 🔗 Liens Utiles

- [Documentation GTK](https://docs.gtk.org/gtk3/)
- [MSYS2](https://www.msys2.org/)
- [GNU Make](https://www.gnu.org/software/make/)

## 📄 Licence

Voir le fichier [LICENSE](LICENSE)

## 🤝 Contribution

Les contributions sont les bienvenues ! N'hésitez pas à :
1. Fork le projet
2. Créer une branche pour votre fonctionnalité
3. Commiter vos changements
4. Pousser vers la branche
5. Ouvrir une Pull Request

## 📞 Support

Pour toute question ou problème :
1. Consultez la documentation dans le dossier
2. Vérifiez les problèmes courants ci-dessus
3. Ouvrez une issue sur GitHub

---

**Bon développement avec Monster8 ! 🎮✨**