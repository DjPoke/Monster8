# Guide de Build - Monster8 Editor

## Vue d'ensemble

Le projet Monster8 Editor supporte maintenant deux configurations de build :
- **Debug** : Version avec symboles de débogage, sans optimisation (-g -O0 -DDEBUG)
- **Release** : Version optimisée pour la production (-O3 -DNDEBUG)

## Structure des répertoires

Les builds sont organisés dans des répertoires séparés :
```
build/
├── Debug/
│   ├── Monster8Editor.exe (Windows) ou Monster8Editor (Linux)
│   ├── *.o (fichiers objets)
│   └── app.res (ressources Windows)
└── Release/
    ├── Monster8Editor.exe (Windows) ou Monster8Editor (Linux)
    ├── *.o (fichiers objets)
    └── app.res (ressources Windows)
```

## Compilation avec Make

### Commandes de base

```bash
# Build Debug (par défaut)
make
make debug

# Build Release
make release

# Build avec type spécifique
make BUILD_TYPE=Debug
make BUILD_TYPE=Release
```

### Commandes de nettoyage

```bash
# Nettoyer le build actuel
make clean

# Nettoyer tous les builds (Debug + Release)
make clean-all

# Nettoyage complet (inclut les fichiers de backup)
make distclean
```

### Informations

```bash
# Afficher la configuration de build
make info

# Afficher l'aide
make help
```

## Compilation dans VS Code

### Tâches disponibles

Le fichier `.vscode/tasks.json` définit les tâches suivantes :

1. **Build Monster8Editor (Debug)** - Build Debug (tâche par défaut)
2. **Build Monster8Editor (Release)** - Build Release
3. **Build Monster8Editor (Debug - MSYS2)** - Build Debug via MSYS2 (Windows)
4. **Build Monster8Editor (Release - MSYS2)** - Build Release via MSYS2 (Windows)
5. **Clean Current Build** - Nettoyer le build actuel
6. **Clean All Builds** - Nettoyer tous les builds
7. **Build Info** - Afficher les informations de configuration

### Raccourcis clavier

- `Ctrl+Shift+B` : Exécuter la tâche de build par défaut (Debug)
- `Ctrl+Shift+P` puis "Tasks: Run Task" : Choisir une tâche spécifique

## Débogage dans VS Code

### Configurations de lancement

Le fichier `.vscode/launch.json` définit 4 configurations :

1. **Debug (Windows)** - Déboguer la version Debug sur Windows
2. **Release (Windows)** - Déboguer la version Release sur Windows
3. **Debug (Linux)** - Déboguer la version Debug sur Linux
4. **Release (Linux)** - Déboguer la version Release sur Linux

### Utilisation

1. Appuyez sur `F5` ou allez dans le menu "Run > Start Debugging"
2. Sélectionnez la configuration appropriée dans la liste déroulante
3. Le projet sera automatiquement compilé avant le lancement

## Différences entre Debug et Release

### Version Debug
- **Optimisation** : Aucune (-O0)
- **Symboles de débogage** : Activés (-g)
- **Définitions** : DEBUG défini
- **Taille** : Plus grande
- **Performance** : Plus lente
- **Usage** : Développement et débogage

### Version Release
- **Optimisation** : Maximum (-O3)
- **Symboles de débogage** : Désactivés
- **Définitions** : NDEBUG défini
- **Taille** : Plus petite
- **Performance** : Optimale
- **Usage** : Distribution et production

## Plateformes supportées

### Windows
- Compilateur : MinGW-w64 (via MSYS2)
- Exécutable : `build/Debug/Monster8Editor.exe` ou `build/Release/Monster8Editor.exe`
- Ressources : Fichier `app.rc` compilé en `app.res`

### Linux
- Compilateur : GCC
- Exécutable : `build/Debug/Monster8Editor` ou `build/Release/Monster8Editor`
- Installation : `sudo make install` (installe dans `/usr/local/bin`)

## Dépendances

Le projet nécessite les bibliothèques suivantes :
- GTK+ 3.0
- GtkSourceView 3.0
- HarfBuzz
- GLib 2.0
- Cairo
- Pango

### Installation des dépendances

**Windows (MSYS2)** :
```bash
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz
```

**Linux (Ubuntu/Debian)** :
```bash
sudo apt-get install libgtk-3-dev libgtksourceview-3.0-dev libharfbuzz-dev
```

**Linux (Fedora)** :
```bash
sudo dnf install gtk3-devel gtksourceview3-devel harfbuzz-devel
```

## Résolution de problèmes

### Make non trouvé (Windows)
Utilisez les tâches MSYS2 dans VS Code ou exécutez les commandes via MSYS2 :
```bash
C:\msys64\msys2_shell.cmd -mingw64 -defterm -no-start -here -c "make debug"
```

### Erreurs de compilation
1. Vérifiez que toutes les dépendances sont installées
2. Exécutez `make info` pour voir la configuration
3. Nettoyez et recompilez : `make clean-all && make debug`

### Erreurs de lancement dans VS Code
1. Vérifiez que le build a réussi
2. Vérifiez que le chemin de l'exécutable est correct dans `launch.json`
3. Vérifiez que GDB est installé et accessible

## Notes importantes

- Le répertoire `build/` est ignoré par Git (voir `.gitignore`)
- Les fichiers objets (*.o) sont maintenant dans les répertoires de build
- Chaque configuration (Debug/Release) a ses propres fichiers objets
- Les deux versions peuvent coexister sans conflit