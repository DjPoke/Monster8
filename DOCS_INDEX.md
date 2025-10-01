# 📚 Index de la Documentation Monster8 Editor

## 🎯 Par Objectif

### Je veux compiler rapidement
1. **[QUICKSTART.md](QUICKSTART.md)** - Instructions minimales
2. Exécutez `./build.sh`

### Je suis sur Windows et je débute
1. **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** - Guide complet Windows
2. **[QUICKSTART.md](QUICKSTART.md)** - Commandes de base

### Je veux comprendre le système de build
1. **[BUILD.md](BUILD.md)** - Guide complet de compilation
2. **[CHANGES.md](CHANGES.md)** - Détails des modifications
3. **[SUMMARY.md](SUMMARY.md)** - Vue d'ensemble

### J'ai un problème de compilation
1. **[BUILD.md](BUILD.md)** - Section "Résolution de problèmes"
2. **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** - Section "Problèmes Courants" (Windows)
3. **[README_FR.md](README_FR.md)** - Section "Problèmes Courants"

### Je veux contribuer au projet
1. **[SUMMARY.md](SUMMARY.md)** - Comprendre l'architecture
2. **[CHANGES.md](CHANGES.md)** - Notes pour développeurs
3. **[README_FR.md](README_FR.md)** - Section "Contribution"

## 📄 Par Document

### Documentation Utilisateur

| Document | Niveau | Plateforme | Description |
|----------|--------|------------|-------------|
| **[README.md](README.md)** | Débutant | Toutes | Introduction générale (EN) |
| **[README_FR.md](README_FR.md)** | Débutant | Toutes | Introduction générale (FR) |
| **[QUICKSTART.md](QUICKSTART.md)** | Débutant | Toutes | Démarrage rapide |
| **[BUILD.md](BUILD.md)** | Intermédiaire | Toutes | Guide complet de compilation |
| **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** | Débutant | Windows | Guide spécifique Windows |

### Documentation Technique

| Document | Niveau | Public | Description |
|----------|--------|--------|-------------|
| **[SUMMARY.md](SUMMARY.md)** | Intermédiaire | Tous | Résumé des modifications |
| **[CHANGES.md](CHANGES.md)** | Avancé | Développeurs | Détails techniques |
| **[DOCS_INDEX.md](DOCS_INDEX.md)** | Débutant | Tous | Ce fichier |

## 🗺️ Parcours Recommandés

### 🐧 Utilisateur Linux Débutant
```
1. README_FR.md (vue d'ensemble)
   ↓
2. QUICKSTART.md (compilation rapide)
   ↓
3. BUILD.md (si problèmes)
```

### 🪟 Utilisateur Windows Débutant
```
1. README_FR.md (vue d'ensemble)
   ↓
2. WINDOWS_SETUP.md (installation MSYS2)
   ↓
3. QUICKSTART.md (compilation)
   ↓
4. BUILD.md (si problèmes)
```

### 👨‍💻 Développeur Expérimenté
```
1. SUMMARY.md (vue d'ensemble technique)
   ↓
2. CHANGES.md (détails des modifications)
   ↓
3. Makefile (code source)
```

### 🔧 Mainteneur / Contributeur
```
1. SUMMARY.md (architecture)
   ↓
2. CHANGES.md (historique)
   ↓
3. BUILD.md (processus de build)
   ↓
4. Code source (main.cpp, Makefile, etc.)
```

## 📊 Matrice de Contenu

|  | Installation | Compilation | Problèmes | Technique | Windows |
|--|--------------|-------------|-----------|-----------|---------|
| **README.md** | ⭐ | ⭐ | - | - | - |
| **README_FR.md** | ⭐⭐ | ⭐⭐ | ⭐ | - | ⭐ |
| **QUICKSTART.md** | ⭐ | ⭐⭐⭐ | ⭐ | - | ⭐ |
| **BUILD.md** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐ | ⭐⭐ |
| **WINDOWS_SETUP.md** | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | - | ⭐⭐⭐ |
| **SUMMARY.md** | - | ⭐ | - | ⭐⭐⭐ | - |
| **CHANGES.md** | - | - | - | ⭐⭐⭐ | - |

Légende : ⭐ = Peu d'info, ⭐⭐ = Info moyenne, ⭐⭐⭐ = Info complète

## 🔍 Recherche Rapide

### Commandes Make
- **[BUILD.md](BUILD.md)** - Section "Commandes Make disponibles"
- **[QUICKSTART.md](QUICKSTART.md)** - Commandes de base
- `make help` dans le terminal

### Installation MSYS2
- **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** - Guide complet
- **[BUILD.md](BUILD.md)** - Section "Windows (MSYS2)"

### Dépendances
- **[BUILD.md](BUILD.md)** - Section "Prérequis"
- **[README_FR.md](README_FR.md)** - Section "Prérequis"

### Résolution de problèmes
- **[BUILD.md](BUILD.md)** - Section "🐛 Résolution de problèmes"
- **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** - Section "⚠️ Problèmes Courants"
- **[README_FR.md](README_FR.md)** - Section "🐛 Problèmes Courants"

### Intégration VS Code
- **[WINDOWS_SETUP.md](WINDOWS_SETUP.md)** - Section "Intégration VS Code"
- `.vscode/tasks.json` - Configuration des tâches

## 📱 Aide Rapide par Erreur

| Erreur | Document | Section |
|--------|----------|---------|
| "make: command not found" | WINDOWS_SETUP.md | Problèmes Courants |
| "pkg-config not found" | BUILD.md | Résolution de problèmes |
| "Package gtk+-3.0 was not found" | BUILD.md | Résolution de problèmes |
| "Cannot find -lgtk-3" | BUILD.md | Résolution de problèmes |
| Exécutable ne démarre pas (Windows) | WINDOWS_SETUP.md | L'exécutable ne démarre pas |
| Erreurs C++17 | BUILD.md | Résolution de problèmes |

## 🎓 Glossaire

- **MSYS2** : Environnement Unix-like pour Windows
- **MinGW** : Compilateur GCC pour Windows
- **pkg-config** : Outil pour trouver les bibliothèques
- **GTK** : Bibliothèque d'interface graphique
- **Make** : Outil de build automatique
- **Makefile** : Fichier de configuration pour Make

## 🔗 Liens Externes

- [MSYS2 Official](https://www.msys2.org/)
- [GTK Documentation](https://docs.gtk.org/gtk3/)
- [GNU Make Manual](https://www.gnu.org/software/make/manual/)

## 💡 Conseils de Navigation

1. **Commencez toujours par README_FR.md** pour une vue d'ensemble
2. **Utilisez QUICKSTART.md** si vous voulez compiler rapidement
3. **Consultez BUILD.md** pour des informations détaillées
4. **Gardez WINDOWS_SETUP.md** ouvert si vous êtes sur Windows
5. **Utilisez Ctrl+F** pour rechercher dans les documents

## 📞 Besoin d'Aide ?

1. Cherchez votre erreur dans ce document (section "Aide Rapide par Erreur")
2. Consultez le document recommandé
3. Essayez `make help` dans le terminal
4. Vérifiez les issues GitHub
5. Créez une nouvelle issue si nécessaire

---

**Navigation rapide** : [README](README.md) | [Démarrage Rapide](QUICKSTART.md) | [Build Complet](BUILD.md) | [Windows](WINDOWS_SETUP.md)