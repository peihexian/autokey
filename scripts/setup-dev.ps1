# AutoKey Development Environment Setup Script
# This script helps set up the development environment

param(
    [switch]$InstallQt = $false,
    [switch]$InstallVSCode = $false,
    [switch]$InstallCMake = $false,
    [switch]$CheckOnly = $false
)

Write-Host "AutoKey Development Environment Setup" -ForegroundColor Green
Write-Host "====================================" -ForegroundColor Green
Write-Host ""

# Function to check if a command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

# Function to check if a path exists
function Test-QtInstallation {
    $QtPaths = @(
        "C:\Qt\5.15.2\msvc2019_64",
        "C:\Qt5\5.15.2\msvc2019_64",
        "D:\Qt\5.15.2\msvc2019_64",
        "D:\Qt5\5.15.2\msvc2019_64"
    )
    
    foreach ($QtPath in $QtPaths) {
        if (Test-Path $QtPath) {
            return $QtPath
        }
    }
    return $null
}

# Check current environment
Write-Host "Checking development environment..." -ForegroundColor Yellow
Write-Host ""

# Check CMake
$CMakeInstalled = Test-Command "cmake"
Write-Host "CMake: " -NoNewline
if ($CMakeInstalled) {
    $CMakeVersion = (cmake --version | Select-Object -First 1) -replace "cmake version ", ""
    Write-Host "✓ Installed ($CMakeVersion)" -ForegroundColor Green
}
else {
    Write-Host "✗ Not found" -ForegroundColor Red
}

# Check Visual Studio / MSBuild
$MSBuildInstalled = Test-Command "msbuild"
Write-Host "MSBuild: " -NoNewline
if ($MSBuildInstalled) {
    Write-Host "✓ Available" -ForegroundColor Green
}
else {
    Write-Host "✗ Not found" -ForegroundColor Red
}

# Check Git
$GitInstalled = Test-Command "git"
Write-Host "Git: " -NoNewline
if ($GitInstalled) {
    $GitVersion = (git --version) -replace "git version ", ""
    Write-Host "✓ Installed ($GitVersion)" -ForegroundColor Green
}
else {
    Write-Host "✗ Not found" -ForegroundColor Red
}

# Check vcpkg
Write-Host "vcpkg: " -NoNewline
if ($env:VCPKG_ROOT) {
    if (Test-Path $env:VCPKG_ROOT) {
        $VcpkgExe = Join-Path $env:VCPKG_ROOT "vcpkg.exe"
        if (Test-Path $VcpkgExe) {
            Write-Host "✓ Found at $env:VCPKG_ROOT" -ForegroundColor Green

            # Check if Qt5 is installed via vcpkg
            $VcpkgQt = Join-Path $env:VCPKG_ROOT "installed\x64-windows\include\QtCore"
            if (Test-Path $VcpkgQt) {
                Write-Host "  ✓ Qt5 installed via vcpkg" -ForegroundColor Green
            }
            else {
                Write-Host "  ⚠ Qt5 not found in vcpkg (run: vcpkg install qt5-base qt5-widgets)" -ForegroundColor Yellow
            }
        }
        else {
            Write-Host "⚠ VCPKG_ROOT set but vcpkg.exe not found" -ForegroundColor Yellow
        }
    }
    else {
        Write-Host "⚠ VCPKG_ROOT set but path doesn't exist" -ForegroundColor Yellow
    }
}
else {
    Write-Host "✗ VCPKG_ROOT not set" -ForegroundColor Red
}

# Check Qt (fallback if vcpkg not available)
$QtPath = Test-QtInstallation
Write-Host "Qt 5.15.2 (fallback): " -NoNewline
if ($QtPath) {
    Write-Host "✓ Found at $QtPath" -ForegroundColor Green
}
else {
    Write-Host "✗ Not found" -ForegroundColor Red
}

# Check Visual Studio Code
$VSCodeInstalled = Test-Command "code"
Write-Host "VS Code: " -NoNewline
if ($VSCodeInstalled) {
    Write-Host "✓ Installed" -ForegroundColor Green
}
else {
    Write-Host "✗ Not found" -ForegroundColor Red
}

# Check PowerShell version
Write-Host "PowerShell: " -NoNewline
$PSVersion = $PSVersionTable.PSVersion
if ($PSVersion.Major -ge 5) {
    Write-Host "✓ Version $PSVersion" -ForegroundColor Green
}
else {
    Write-Host "⚠ Version $PSVersion (5.0+ recommended)" -ForegroundColor Yellow
}

Write-Host ""

if ($CheckOnly) {
    Write-Host "Environment check completed." -ForegroundColor Green
    exit 0
}

# Installation section
$NeedsInstallation = $false

if (-not $CMakeInstalled -and $InstallCMake) {
    Write-Host "Installing CMake..." -ForegroundColor Yellow
    try {
        if (Test-Command "winget") {
            winget install Kitware.CMake
        }
        elseif (Test-Command "choco") {
            choco install cmake -y
        }
        else {
            Write-Host "Please install CMake manually from https://cmake.org/download/" -ForegroundColor Red
            $NeedsInstallation = $true
        }
    }
    catch {
        Write-Host "Failed to install CMake automatically. Please install manually." -ForegroundColor Red
        $NeedsInstallation = $true
    }
}

if (-not $QtPath -and $InstallQt) {
    Write-Host "Qt installation required..." -ForegroundColor Yellow
    Write-Host "Please install Qt 5.15.2 with MSVC 2019 64-bit from:" -ForegroundColor Yellow
    Write-Host "https://www.qt.io/download-qt-installer" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Recommended installation path: C:\Qt\5.15.2\msvc2019_64" -ForegroundColor Yellow
    $NeedsInstallation = $true
}

if (-not $VSCodeInstalled -and $InstallVSCode) {
    Write-Host "Installing Visual Studio Code..." -ForegroundColor Yellow
    try {
        if (Test-Command "winget") {
            winget install Microsoft.VisualStudioCode
        }
        elseif (Test-Command "choco") {
            choco install vscode -y
        }
        else {
            Write-Host "Please install VS Code manually from https://code.visualstudio.com/" -ForegroundColor Red
            $NeedsInstallation = $true
        }
    }
    catch {
        Write-Host "Failed to install VS Code automatically. Please install manually." -ForegroundColor Red
        $NeedsInstallation = $true
    }
}

if (-not $MSBuildInstalled) {
    Write-Host "Visual Studio Build Tools required..." -ForegroundColor Yellow
    Write-Host "Please install Visual Studio 2019/2022 or Build Tools from:" -ForegroundColor Yellow
    Write-Host "https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Required components:" -ForegroundColor Yellow
    Write-Host "  - MSVC v143 - VS 2022 C++ x64/x86 build tools" -ForegroundColor Yellow
    Write-Host "  - Windows 10/11 SDK" -ForegroundColor Yellow
    Write-Host "  - CMake tools for Visual Studio" -ForegroundColor Yellow
    $NeedsInstallation = $true
}

# Setup VS Code extensions
if ($VSCodeInstalled -or $InstallVSCode) {
    Write-Host ""
    Write-Host "Setting up VS Code extensions..." -ForegroundColor Yellow
    
    $Extensions = @(
        "ms-vscode.cpptools",
        "ms-vscode.cpptools-extension-pack",
        "ms-vscode.cmake-tools",
        "twxs.cmake"
    )
    
    foreach ($Extension in $Extensions) {
        try {
            code --install-extension $Extension --force
            Write-Host "  ✓ Installed $Extension" -ForegroundColor Green
        }
        catch {
            Write-Host "  ✗ Failed to install $Extension" -ForegroundColor Red
        }
    }
}

Write-Host ""

if ($NeedsInstallation) {
    Write-Host "Some components need manual installation." -ForegroundColor Yellow
    Write-Host "Please install the missing components and run this script again." -ForegroundColor Yellow
}
else {
    Write-Host "Development environment setup completed!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Yellow
    Write-Host "  1. Open the project in VS Code: code ." -ForegroundColor White
    Write-Host "  2. Configure CMake (Ctrl+Shift+P -> CMake: Configure)" -ForegroundColor White
    Write-Host "  3. Build the project (Ctrl+Shift+P -> CMake: Build)" -ForegroundColor White
    Write-Host "  4. Run the application (F5 or Ctrl+F5)" -ForegroundColor White
}

Write-Host ""
Write-Host "For more information, see README.md" -ForegroundColor Cyan
