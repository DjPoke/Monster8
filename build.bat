@echo off
REM Build script for Monster8 Editor (Windows)
REM Usage: build.bat [options]

setlocal enabledelayedexpansion

echo === Monster8 Editor Build Script ===
echo.

REM Default parameters
set BUILD_TYPE=Release
set CLEAN=0
set RUN_AFTER=0

REM Parse command line arguments
:parse_args
if "%1"=="" goto check_dependencies
if /i "%1"=="--debug" goto set_debug
if /i "%1"=="-d" goto set_debug
if /i "%1"=="--release" goto set_release
if /i "%1"=="-r" goto set_release
if /i "%1"=="--clean" goto set_clean
if /i "%1"=="-c" goto set_clean
if /i "%1"=="--run" goto set_run
if /i "%1"=="--help" goto show_help
if /i "%1"=="-h" goto show_help
if /i "%1"=="debug" goto set_debug
if /i "%1"=="release" goto set_release
if /i "%1"=="clean" goto set_clean
if /i "%1"=="run" goto set_run
if /i "%1"=="help" goto show_help

echo Unknown option: %1
echo Use --help for usage information
goto end_error

:set_debug
set BUILD_TYPE=Debug
shift
goto parse_args

:set_release
set BUILD_TYPE=Release
shift
goto parse_args

:set_clean
set CLEAN=1
shift
goto parse_args

:set_run
set RUN_AFTER=1
shift
goto parse_args

:show_help
echo Usage: build.bat [options]
echo.
echo Options:
echo   -d, --debug     Build in Debug mode
echo   -r, --release   Build in Release mode (default)
echo   -c, --clean     Clean build directory before building
echo   --run           Run the executable after successful build
echo   -h, --help      Show this help message
echo.
echo Examples:
echo   build.bat               Build in Release mode
echo   build.bat --debug       Build in Debug mode
echo   build.bat --clean       Clean and build in Release mode
echo   build.bat -d --run      Build in Debug mode and run
echo.
goto end

:check_dependencies
echo Platform: Windows
echo Build type: %BUILD_TYPE%
echo.

REM Check for CMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: 'cmake' not found!
    echo.
    echo Please install CMake from https://cmake.org/download/
    echo or install via MSYS2: pacman -S mingw-w64-x86_64-cmake
    goto end_error
)

REM Check for a build tool (Make or Ninja)
where ninja >nul 2>&1
if not errorlevel 1 (
    set GENERATOR=Ninja
    goto deps_ok
)

where mingw32-make >nul 2>&1
if not errorlevel 1 (
    set GENERATOR=MinGW Makefiles
    goto deps_ok
)

where make >nul 2>&1
if not errorlevel 1 (
    set GENERATOR=MinGW Makefiles
    goto deps_ok
)

echo ERROR: No build tool found!
echo.
echo Please install one of the following:
echo   - Ninja: pacman -S mingw-w64-x86_64-ninja (MSYS2)
echo   - Make:  pacman -S make (MSYS2)
goto end_error

:deps_ok
echo All build tools found!
echo Using generator: %GENERATOR%
echo.

REM Clean build directory if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist build rmdir /s /q build
    echo.
)

REM Create build directory
if not exist build mkdir build

REM Configure with CMake
echo Configuring with CMake...
echo.
cd build
cmake -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..

if errorlevel 1 (
    cd ..
    echo.
    echo ===================================
    echo X CMake configuration failed!
    echo ===================================
    goto end_error
)

echo.
echo Building Monster8 Editor...
echo.

REM Build with all available CPU cores
cmake --build . --config %BUILD_TYPE% -j %NUMBER_OF_PROCESSORS%

REM Check if build was successful
if errorlevel 1 (
    cd ..
    echo.
    echo ===================================
    echo X Build failed!
    echo ===================================
    echo.
    echo Please check the error messages above.
    goto end_error
)

cd ..
echo.
echo ===================================
echo √ Build successful!
echo ===================================
echo Build Type: %BUILD_TYPE%
echo Executable: build\Monster8Editor.exe
echo.

REM Run the executable if requested
if %RUN_AFTER%==1 (
    echo Running Monster8 Editor...
    echo.
    build\Monster8Editor.exe
)

echo To run the editor:
echo   build\Monster8Editor.exe
echo.
goto end

:end_error
exit /b 1

:end
endlocal