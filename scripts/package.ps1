# AutoKey Packaging Script
# This script packages the AutoKey application for distribution

param(
    [string]$BuildType = "Release",
    [string]$QtPath = "",
    [string]$OutputDir = "dist",
    [switch]$CreateInstaller = $false
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectRoot "build"
$BinDir = Join-Path $BuildDir "bin"

Write-Host "AutoKey Packaging Script" -ForegroundColor Green
Write-Host "========================" -ForegroundColor Green
Write-Host "Project Root: $ProjectRoot"
Write-Host "Build Directory: $BuildDir"
Write-Host "Output Directory: $OutputDir"
Write-Host ""

# Check if build exists
if (-not (Test-Path $BinDir)) {
    Write-Error "Build directory not found. Please build the project first."
    exit 1
}

# Check if executable exists
$ExePath = Join-Path $BinDir "AutoKey.exe"
if (-not (Test-Path $ExePath)) {
    Write-Error "AutoKey.exe not found in build directory."
    exit 1
}

# Create output directory
$DistDir = Join-Path $ProjectRoot $OutputDir
if (Test-Path $DistDir) {
    Write-Host "Cleaning existing distribution directory..."
    Remove-Item $DistDir -Recurse -Force
}
New-Item -ItemType Directory -Path $DistDir | Out-Null

# Copy main executable
Write-Host "Copying executable..."
Copy-Item $ExePath $DistDir

# Copy configuration file
$ConfigPath = Join-Path $ProjectRoot "config.json"
if (Test-Path $ConfigPath) {
    Copy-Item $ConfigPath $DistDir
}

# Find Qt installation
$QtDirs = @()
if ($QtPath -ne "") {
    $QtDirs += $QtPath
}

# Check vcpkg first
if ($env:VCPKG_ROOT) {
    $VcpkgQt = Join-Path $env:VCPKG_ROOT "installed\x64-windows\tools\qt5"
    if (Test-Path $VcpkgQt) {
        $QtDirs += $VcpkgQt
        Write-Host "Found vcpkg Qt tools at: $VcpkgQt"
    }

    # Also check for Qt binaries in vcpkg
    $VcpkgBin = Join-Path $env:VCPKG_ROOT "installed\x64-windows\bin"
    if (Test-Path $VcpkgBin) {
        $QtDirs += $VcpkgBin
        Write-Host "Found vcpkg Qt binaries at: $VcpkgBin"
    }
}

# Fallback to standard Qt installations
$QtDirs += "C:\Qt\5.15.2\msvc2019_64"
$QtDirs += "C:\Qt5\5.15.2\msvc2019_64"
$QtDirs += "D:\Qt\5.15.2\msvc2019_64"
$QtDirs += "D:\Qt5\5.15.2\msvc2019_64"

$QtFound = $false
foreach ($QtDir in $QtDirs) {
    if (Test-Path $QtDir) {
        $QtBinDir = Join-Path $QtDir "bin"
        $WinDeployQt = Join-Path $QtBinDir "windeployqt.exe"
        
        if (Test-Path $WinDeployQt) {
            Write-Host "Found Qt at: $QtDir"
            $QtFound = $true
            
            # Run windeployqt
            Write-Host "Running windeployqt..."
            $WinDeployArgs = @(
                "--$($BuildType.ToLower())",
                "--no-translations",
                "--no-system-d3d-compiler",
                "--no-opengl-sw",
                "--dir", $DistDir,
                $ExePath
            )
            
            & $WinDeployQt @WinDeployArgs
            if ($LASTEXITCODE -ne 0) {
                Write-Error "windeployqt failed with exit code $LASTEXITCODE"
                exit 1
            }
            break
        }
    }
}

if (-not $QtFound) {
    Write-Warning "Qt installation not found. You may need to manually copy Qt DLLs."
    Write-Host "Searched in the following locations:"
    foreach ($QtDir in $QtDirs) {
        Write-Host "  - $QtDir"
    }
}

# Copy additional files
Write-Host "Copying additional files..."

# Copy README
$ReadmePath = Join-Path $ProjectRoot "README.md"
if (Test-Path $ReadmePath) {
    Copy-Item $ReadmePath $DistDir
}

# Create version info file
$VersionInfo = @"
AutoKey for Diablo 3
Version: 1.0.0
Build Date: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Build Type: $BuildType

System Requirements:
- Windows 10/11
- Visual C++ Redistributable 2019/2022

Installation:
1. Extract all files to a folder
2. Run AutoKey.exe
3. Configure your profiles in the application

For support and updates, visit:
https://github.com/your-username/autokey
"@

$VersionInfo | Out-File -FilePath (Join-Path $DistDir "VERSION.txt") -Encoding UTF8

# Create batch file for easy launching
$LaunchScript = @"
@echo off
cd /d "%~dp0"
start AutoKey.exe
"@

$LaunchScript | Out-File -FilePath (Join-Path $DistDir "Launch AutoKey.bat") -Encoding ASCII

# Get file size information
$Files = Get-ChildItem $DistDir -Recurse -File
$TotalSize = ($Files | Measure-Object -Property Length -Sum).Sum
$TotalSizeMB = [math]::Round($TotalSize / 1MB, 2)

Write-Host ""
Write-Host "Packaging completed successfully!" -ForegroundColor Green
Write-Host "Distribution directory: $DistDir"
Write-Host "Total files: $($Files.Count)"
Write-Host "Total size: $TotalSizeMB MB"
Write-Host ""

# List all files
Write-Host "Packaged files:"
$Files | ForEach-Object {
    $RelativePath = $_.FullName.Substring($DistDir.Length + 1)
    $SizeKB = [math]::Round($_.Length / 1KB, 1)
    Write-Host "  $RelativePath ($SizeKB KB)"
}

# Create ZIP archive
Write-Host ""
Write-Host "Creating ZIP archive..."
$ZipPath = Join-Path $ProjectRoot "AutoKey-v1.0.0-Windows.zip"
if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
}

try {
    Compress-Archive -Path "$DistDir\*" -DestinationPath $ZipPath -CompressionLevel Optimal
    $ZipSize = [math]::Round((Get-Item $ZipPath).Length / 1MB, 2)
    Write-Host "ZIP archive created: $ZipPath ($ZipSize MB)" -ForegroundColor Green
}
catch {
    Write-Warning "Failed to create ZIP archive: $($_.Exception.Message)"
}

# Create installer if requested
if ($CreateInstaller) {
    Write-Host ""
    Write-Host "Creating installer..."
    
    $InnoSetupScript = Join-Path $ScriptDir "autokey-installer.iss"
    if (Test-Path $InnoSetupScript) {
        # Look for Inno Setup
        $InnoSetupPaths = @(
            "C:\Program Files (x86)\Inno Setup 6\ISCC.exe",
            "C:\Program Files\Inno Setup 6\ISCC.exe"
        )
        
        $InnoSetupFound = $false
        foreach ($InnoPath in $InnoSetupPaths) {
            if (Test-Path $InnoPath) {
                Write-Host "Found Inno Setup at: $InnoPath"
                & $InnoPath $InnoSetupScript
                if ($LASTEXITCODE -eq 0) {
                    Write-Host "Installer created successfully!" -ForegroundColor Green
                }
                else {
                    Write-Warning "Installer creation failed with exit code $LASTEXITCODE"
                }
                $InnoSetupFound = $true
                break
            }
        }
        
        if (-not $InnoSetupFound) {
            Write-Warning "Inno Setup not found. Please install Inno Setup to create installer."
        }
    }
    else {
        Write-Warning "Installer script not found: $InnoSetupScript"
    }
}

Write-Host ""
Write-Host "Packaging process completed!" -ForegroundColor Green
