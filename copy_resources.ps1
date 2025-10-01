# Script pour copier les ressources GTK depuis MSYS2
$target_dir = "c:\Users\bruno\Documents\GitHub\Monster8"

Write-Host "Copie des ressources GTK..." -ForegroundColor Green

# Copier les schemas GLib
$glib_schemas_src = "C:\msys64\mingw64\share\glib-2.0\schemas"
$glib_schemas_dest = "$target_dir\share\glib-2.0\schemas"

if (Test-Path $glib_schemas_src) {
    New-Item -ItemType Directory -Path $glib_schemas_dest -Force | Out-Null
    Copy-Item -Path "$glib_schemas_src\*" -Destination $glib_schemas_dest -Recurse -Force
    Write-Host "  Schemas GLib copies" -ForegroundColor Gray
}

# Copier les loaders GDK-Pixbuf
$pixbuf_src = "C:\msys64\mingw64\lib\gdk-pixbuf-2.0"
$pixbuf_dest = "$target_dir\lib\gdk-pixbuf-2.0"

if (Test-Path $pixbuf_src) {
    New-Item -ItemType Directory -Path $pixbuf_dest -Force | Out-Null
    Copy-Item -Path "$pixbuf_src\*" -Destination $pixbuf_dest -Recurse -Force
    Write-Host "  Loaders GDK-Pixbuf copies" -ForegroundColor Gray
}

# Copier quelques icones de base
$icons_src = "C:\msys64\mingw64\share\icons\hicolor"
$icons_dest = "$target_dir\share\icons\hicolor"

if (Test-Path $icons_src) {
    New-Item -ItemType Directory -Path $icons_dest -Force | Out-Null
    # Copier seulement index.theme et quelques tailles essentielles
    Copy-Item -Path "$icons_src\index.theme" -Destination $icons_dest -Force -ErrorAction SilentlyContinue
    
    $sizes = @("16x16", "22x22", "24x24", "32x32", "48x48")
    foreach ($size in $sizes) {
        $size_src = "$icons_src\$size"
        $size_dest = "$icons_dest\$size"
        if (Test-Path $size_src) {
            Copy-Item -Path $size_src -Destination $size_dest -Recurse -Force -ErrorAction SilentlyContinue
        }
    }
    Write-Host "  Icones copiees" -ForegroundColor Gray
}

Write-Host "`nTermine! Toutes les dependances sont pretes." -ForegroundColor Green
Write-Host "Vous pouvez maintenant lancer Monster8Editor.exe sans MSYS2 dans le PATH." -ForegroundColor Cyan