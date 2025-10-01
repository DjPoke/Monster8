#!/bin/bash
# Script de build rapide pour Monster8

set -e  # Arrêter en cas d'erreur

# Couleurs pour l'affichage
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Fonction d'aide
show_help() {
    echo "Usage: ./build.sh [option]"
    echo ""
    echo "Options:"
    echo "  release    - Build en mode Release (par défaut)"
    echo "  debug      - Build en mode Debug"
    echo "  clean      - Nettoyer le dossier build"
    echo "  run        - Compiler et exécuter (Release)"
    echo "  help       - Afficher cette aide"
    echo ""
}

# Déterminer le mode de build
BUILD_TYPE="Release"
RUN_AFTER=false
CLEAN=false

case "${1:-release}" in
    release)
        BUILD_TYPE="Release"
        ;;
    debug)
        BUILD_TYPE="Debug"
        ;;
    clean)
        CLEAN=true
        ;;
    run)
        BUILD_TYPE="Release"
        RUN_AFTER=true
        ;;
    help|--help|-h)
        show_help
        exit 0
        ;;
    *)
        echo -e "${RED}Option invalide: $1${NC}"
        show_help
        exit 1
        ;;
esac

# Nettoyage si demandé
if [ "$CLEAN" = true ]; then
    echo -e "${BLUE}Nettoyage du dossier build...${NC}"
    rm -rf build/
    echo -e "${GREEN}✓ Nettoyage terminé${NC}"
    exit 0
fi

# Créer le dossier build s'il n'existe pas
if [ ! -d "build" ]; then
    echo -e "${BLUE}Création du dossier build...${NC}"
    mkdir build
fi

# Configuration CMake
echo -e "${BLUE}Configuration CMake ($BUILD_TYPE)...${NC}"
cd build
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Compilation
echo -e "${BLUE}Compilation...${NC}"
cmake --build . -j$(nproc)

echo -e "${GREEN}✓ Compilation réussie !${NC}"
echo -e "Exécutable: ${GREEN}build/Monster8${NC}"

# Exécution si demandé
if [ "$RUN_AFTER" = true ]; then
    echo -e "${BLUE}Lancement de Monster8...${NC}"
    ./Monster8
fi