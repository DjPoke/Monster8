# Script pour copier les dépendances GTK 3 depuis MSYS2
$msys2_bin = "C:\msys64\mingw64\bin"
$target_dir = "c:\Users\bruno\Documents\GitHub\Monster8"

# Liste des DLL essentielles pour GTK 3
$dlls = @(
    # Runtime GCC
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
    
    # GLib et dépendances
    "libglib-2.0-0.dll",
    "libgobject-2.0-0.dll",
    "libgio-2.0-0.dll",
    "libgmodule-2.0-0.dll",
    "libgthread-2.0-0.dll",
    
    # GTK 3
    "libgtk-3-0.dll",
    "libgdk-3-0.dll",
    
    # GDK-Pixbuf
    "libgdk_pixbuf-2.0-0.dll",
    
    # Pango
    "libpango-1.0-0.dll",
    "libpangocairo-1.0-0.dll",
    "libpangowin32-1.0-0.dll",
    "libpangoft2-1.0-0.dll",
    
    # Cairo
    "libcairo-2.dll",
    "libcairo-gobject-2.dll",
    
    # ATK
    "libatk-1.0-0.dll",
    
    # Autres dépendances
    "libintl-8.dll",
    "libiconv-2.dll",
    "libffi-8.dll",
    "libpcre2-8-0.dll",
    "libharfbuzz-0.dll",
    "libfreetype-6.dll",
    "libfontconfig-1.dll",
    "libexpat-1.dll",
    "libpixman-1-0.dll",
    "libpng16-16.dll",
    "zlib1.dll",
    "libbz2-1.dll",
    "libbrotlidec.dll",
    "libbrotlicommon.dll",
    "libgraphite2.dll",
    
    # GtkSourceView
    "libgtksourceview-3.0-1.dll",
    
    # XML
    "libxml2-2.dll",
    "liblzma-5.dll"
)

Write-Host "Copie des DLL depuis MSYS2..." -ForegroundColor Green
$copied = 0
$missing = 0

foreach ($dll in $dlls) {
    $source = Join-Path $msys2_bin $dll
    $dest = Join-Path $target_dir $dll
    
    if (Test-Path $source) {
        Copy-Item -Path $source -Destination $dest -Force
        Write-Host "  ✓ $dll" -ForegroundColor Gray
        $copied++
    } else {
        Write-Host "  ✗ $dll (non trouvé)" -ForegroundColor Yellow
        $missing++
    }
}

Write-Host "`nRésumé:" -ForegroundColor Cyan
Write-Host "  Copiés: $copied" -ForegroundColor Green
Write-Host "  Manquants: $missing" -ForegroundColor Yellow

# Copier aussi les dossiers de ressources GTK
Write-Host "`nCopie des ressources GTK..." -ForegroundColor Green

$resources = @(
    @{src="C:\msys64\mingw64\share\glib-2.0"; dest="share\glib-2.0"},
    @{src="C:\msys64\mingw64\share\icons"; dest="share\icons"},
    @{src="C:\msys64\mingw64\lib\gdk-pixbuf-2.0"; dest="lib\gdk-pixbuf-2.0"}
)

foreach ($res in $resources) {
    $src = $res.src
    $dst = Join-Path $target_dir $res.dest
    
    if (Test-Path $src) {
        if (!(Test-Path $dst)) {
            New-Item -ItemType Directory -Path $dst -Force | Out-Null
        }
        Copy-Item -Path "$src\*" -Destination $dst -Recurse -Force
        Write-Host "  ✓ $($res.dest)" -ForegroundColor Gray
    }
}

Write-Host "`nTerminé!" -ForegroundColor Green