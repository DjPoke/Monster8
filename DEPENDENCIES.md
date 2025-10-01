# Dépendances MSYS2 copiées

Ce document liste les dépendances qui ont été copiées depuis MSYS2 pour permettre l'exécution autonome de Monster8Editor.exe.

## DLL copiées depuis C:\msys64\mingw64\bin

### Runtime GCC
- libgcc_s_seh-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll

### GLib et dépendances
- libglib-2.0-0.dll
- libgobject-2.0-0.dll
- libgio-2.0-0.dll
- libgmodule-2.0-0.dll
- libgthread-2.0-0.dll

### GTK 3
- libgtk-3-0.dll
- libgdk-3-0.dll
- libgtksourceview-3.0-1.dll

### GDK-Pixbuf
- libgdk_pixbuf-2.0-0.dll

### Pango (rendu de texte)
- libpango-1.0-0.dll
- libpangocairo-1.0-0.dll
- libpangowin32-1.0-0.dll
- libpangoft2-1.0-0.dll

### Cairo (rendu graphique)
- libcairo-2.dll
- libcairo-gobject-2.dll

### ATK (accessibilité)
- libatk-1.0-0.dll

### Autres dépendances
- libintl-8.dll (internationalisation)
- libiconv-2.dll (conversion d'encodage)
- libffi-8.dll (foreign function interface)
- libpcre2-8-0.dll (expressions régulières)
- libharfbuzz-0.dll (rendu de texte complexe)
- libfreetype-6.dll (rendu de polices)
- libfontconfig-1.dll (configuration de polices)
- libexpat-1.dll (parsing XML)
- libpixman-1-0.dll (rendu pixel)
- libpng16-16.dll (images PNG)
- zlib1.dll (compression)
- libbz2-1.dll (compression)
- libbrotlidec.dll (compression Brotli)
- libbrotlicommon.dll (compression Brotli)
- libgraphite2.dll (rendu de polices)
- libxml2-2.dll (parsing XML)
- liblzma-5.dll (compression LZMA)

## Ressources copiées

### share/glib-2.0/schemas
Schémas GSettings pour la configuration GTK

### lib/gdk-pixbuf-2.0
Loaders pour différents formats d'images (PNG, JPEG, etc.)

### share/icons/hicolor
Icônes standard GTK (tailles: 16x16, 22x22, 24x24, 32x32, 48x48)

## Utilisation

Toutes ces dépendances permettent à Monster8Editor.exe de s'exécuter sans avoir besoin d'ajouter MSYS2 dans le PATH Windows. L'application peut maintenant être distribuée avec ces fichiers.

## Structure de distribution recommandée

```
Monster8/
├── Monster8Editor.exe
├── *.dll (toutes les DLL listées ci-dessus)
├── lib/
│   └── gdk-pixbuf-2.0/
└── share/
    ├── glib-2.0/
    └── icons/
```

## Scripts de copie

Deux scripts PowerShell ont été créés pour faciliter la copie des dépendances :
- `copy_deps.ps1` : Copie les DLL
- `copy_resources.ps1` : Copie les ressources (schemas, icons, loaders)

Pour mettre à jour les dépendances, exécutez :
```powershell
.\copy_deps.ps1
.\copy_resources.ps1
```