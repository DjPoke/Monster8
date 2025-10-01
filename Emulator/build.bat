@echo off
REM Script de build rapide pour Monster8 (Windows)

setlocal enabledelayedexpansion

REM Déterminer le mode de build
set BUILD_TYPE=Release
set RUN_AFTER=0
set CLEAN=0

if "%1"=="" goto release
if /i "%1"=="release" goto release
if /i "%1"=="debug" goto debug
if /i "%1"=="clean" goto clean
if /i "%1"=="run" goto run
if /i "%1"=="help" goto help
if /i "%1"=="--help" goto help
if /i "%1"=="-h" goto help

echo Option invalide: %1
goto help

:release
set BUILD_TYPE=Release
goto build

:debug
set BUILD_TYPE=Debug
goto build

:run
set BUILD_TYPE=Release
set RUN_AFTER=1
goto build

:clean
echo Nettoyage du dossier build...
if exist build rmdir /s /q build
echo [OK] Nettoyage termine
goto end

:help
echo Usage: build.bat [option]
echo.
echo Options:
echo   release    - Build en mode Release (par defaut)
echo   debug      - Build en mode Debug
echo   clean      - Nettoyer le dossier build
echo   run        - Compiler et executer (Release)
echo   help       - Afficher cette aide
echo.
goto end

:build
REM Créer le dossier build s'il n'existe pas
if not exist build mkdir build

REM Configuration CMake
echo Configuration CMake (%BUILD_TYPE%)...
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo [ERREUR] Configuration CMake echouee
    cd ..
    goto end
)

REM Compilation
echo Compilation...
cmake --build . -j 8
if errorlevel 1 (
    echo [ERREUR] Compilation echouee
    cd ..
    goto end
)

cd ..
echo [OK] Compilation reussie !
echo Executable: build\Monster8.exe

REM Exécution si demandé
if %RUN_AFTER%==1 (
    echo Lancement de Monster8...
    build\Monster8.exe
)

:end
endlocal