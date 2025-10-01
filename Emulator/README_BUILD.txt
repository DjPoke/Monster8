================================================================================
  MONSTER8 EMULATOR - COMPILATION RAPIDE
================================================================================

WINDOWS:
--------
1. Installer MSYS2 depuis https://www.msys2.org/
2. Dans le terminal MSYS2 MinGW 64-bit:
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image

3. Ajouter C:\msys64\mingw64\bin au PATH système

4. Compiler:
   mingw32-make release

5. Exécuter:
   .\Monster8.exe


LINUX:
------
1. Installer les dépendances:
   Ubuntu/Debian:
     sudo apt install build-essential libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev
   
   Fedora:
     sudo dnf install gcc-c++ SDL2-devel SDL2_mixer-devel SDL2_image-devel
   
   Arch:
     sudo pacman -S base-devel sdl2 sdl2_mixer sdl2_image

2. Compiler:
   make release

3. Exécuter:
   ./Monster8


VS CODE:
--------
1. Ouvrir le dossier dans VS Code
2. Appuyer sur Ctrl+Shift+B
3. Sélectionner "Build Monster8 (Release)"
4. Pour déboguer: F5


COMMANDES MAKEFILE:
-------------------
mingw32-make release    (Windows) - Compilation optimisée
make release            (Linux)   - Compilation optimisée

mingw32-make debug      (Windows) - Compilation avec symboles de débogage
make debug              (Linux)   - Compilation avec symboles de débogage

mingw32-make clean      (Windows) - Nettoyer les fichiers compilés
make clean              (Linux)   - Nettoyer les fichiers compilés

mingw32-make help       (Windows) - Afficher l'aide
make help               (Linux)   - Afficher l'aide


FICHIERS IMPORTANTS:
--------------------
- Makefile          : Configuration de compilation cross-platform
- sdl_config.h      : Configuration automatique des headers SDL2
- BUILD.md          : Documentation complète de compilation
- main.cpp, M8.cpp  : Code source de l'émulateur
- Cartridges/       : Dossier pour les fichiers .binm8


DÉPANNAGE:
----------
Windows - "SDL2.dll not found":
  -> Ajouter C:\msys64\mingw64\bin au PATH

Windows - "mingw32-make: command not found":
  -> Installer: pacman -S mingw-w64-x86_64-make

Linux - "SDL2/SDL.h: No such file or directory":
  -> Installer les packages libsdl2-dev

Pour plus de détails, voir BUILD.md

================================================================================