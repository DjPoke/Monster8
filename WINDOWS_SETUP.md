# Configuration Windows pour Monster8 Editor

## 🪟 Installation de MSYS2

### Étape 1 : Télécharger MSYS2

1. Allez sur https://www.msys2.org/
2. Téléchargez l'installateur (msys2-x86_64-*.exe)
3. Exécutez l'installateur
4. Installez dans `C:\msys64` (chemin par défaut recommandé)

### Étape 2 : Mettre à jour MSYS2

1. Ouvrez **"MSYS2 MinGW 64-bit"** depuis le menu Démarrer
2. Exécutez :
```bash
pacman -Syu
```
3. Si demandé, fermez et rouvrez le terminal, puis exécutez à nouveau :
```bash
pacman -Syu
```

### Étape 3 : Installer les Outils de Développement

Dans le terminal **MSYS2 MinGW 64-bit** :

```bash
# Outils de compilation
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-pkg-config
pacman -S make

# Bibliothèques GTK
pacman -S mingw-w64-x86_64-gtk3
pacman -S mingw-w64-x86_64-gtksourceview3
pacman -S mingw-w64-x86_64-harfbuzz
```

Ou en une seule commande :
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config make mingw-w64-x86_64-gtk3 mingw-w64-x86_64-gtksourceview3 mingw-w64-x86_64-harfbuzz
```

## 🔨 Compilation

### Naviguer vers le Projet

Dans le terminal **MSYS2 MinGW 64-bit** :

```bash
# Exemple si votre projet est dans Documents
cd /c/Users/bruno/Documents/GitHub/Monster8
```

**Note** : Dans MSYS2, `C:\` devient `/c/`, `D:\` devient `/d/`, etc.

### Compiler

```bash
# Méthode 1 : Avec le script
chmod +x build.sh
./build.sh

# Méthode 2 : Directement avec make
make

# Méthode 3 : Compilation parallèle (plus rapide)
make -j$(nproc)
```

### Exécuter

```bash
./Monster8Editor.exe
```

## 🎯 Vérification de l'Installation

Vérifiez que tout est installé correctement :

```bash
# Vérifier GCC
gcc --version

# Vérifier Make
make --version

# Vérifier pkg-config
pkg-config --version

# Vérifier GTK
pkg-config --modversion gtk+-3.0
```

Toutes ces commandes doivent afficher une version sans erreur.

## ⚠️ Problèmes Courants

### "make: command not found"
Vous n'avez pas installé make :
```bash
pacman -S make
```

### "pkg-config: command not found"
```bash
pacman -S mingw-w64-x86_64-pkg-config
```

### "Package gtk+-3.0 was not found"
```bash
pacman -S mingw-w64-x86_64-gtk3
```

### Mauvais Terminal
❌ **N'utilisez PAS** :
- "MSYS2 MSYS" (terminal par défaut)
- "MSYS2 UCRT64"
- PowerShell Windows
- CMD Windows

✅ **Utilisez TOUJOURS** :
- "MSYS2 MinGW 64-bit"

### L'exécutable ne démarre pas en dehors de MSYS2

C'est normal ! L'exécutable a besoin des DLLs GTK. Deux solutions :

**Solution 1 : Ajouter MSYS2 au PATH Windows**
1. Ouvrez les Paramètres Système → Variables d'environnement
2. Ajoutez `C:\msys64\mingw64\bin` au PATH
3. Redémarrez votre terminal

**Solution 2 : Copier les DLLs**
Dans MSYS2, listez les DLLs nécessaires :
```bash
ldd Monster8Editor.exe
```
Copiez toutes les DLLs listées depuis `C:\msys64\mingw64\bin` vers le dossier de l'exécutable.

## 📁 Structure des Chemins

| Windows | MSYS2 |
|---------|-------|
| `C:\Users\bruno` | `/c/Users/bruno` |
| `D:\Projects` | `/d/Projects` |
| `C:\msys64` | `/` |

## 🚀 Workflow Recommandé

1. **Développement** : Utilisez VS Code ou votre IDE préféré sous Windows
2. **Compilation** : Ouvrez MSYS2 MinGW 64-bit et compilez avec `make`
3. **Test** : Exécutez depuis MSYS2 avec `./Monster8Editor.exe`

## 💡 Astuces

### Créer un Raccourci de Compilation

Créez un fichier `compile.bat` dans votre projet :
```batch
@echo off
C:\msys64\msys2_shell.cmd -mingw64 -defterm -no-start -here -c "make"
pause
```

Double-cliquez dessus pour compiler sans ouvrir MSYS2 manuellement.

### Intégration VS Code

Ajoutez dans `.vscode/tasks.json` :
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build with Make (MSYS2)",
            "type": "shell",
            "command": "C:\\msys64\\msys2_shell.cmd",
            "args": [
                "-mingw64",
                "-defterm",
                "-no-start",
                "-here",
                "-c",
                "make"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

Puis utilisez `Ctrl+Shift+B` pour compiler depuis VS Code.

## 📚 Ressources

- [MSYS2 Documentation](https://www.msys2.org/docs/what-is-msys2/)
- [GTK Windows Guide](https://www.gtk.org/docs/installations/windows/)
- [Monster8 BUILD.md](BUILD.md)