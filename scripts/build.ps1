# AutoKey Build Script
# This script builds the AutoKey application

param(
    [string]$BuildType = "Release",
    [string]$QtPath = "",
    [string]$Generator = "",
    [switch]$Clean = $false,
    [switch]$Verbose = $false
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "AutoKey Build Script" -ForegroundColor Green
Write-Host "===================" -ForegroundColor Green
Write-Host "Project Root: $ProjectRoot"
Write-Host "Build Type: $BuildType"
Write-Host "Build Directory: $BuildDir"
Write-Host ""

# Clean build directory if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning build directory..."
    Remove-Item $BuildDir -Recurse -Force
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Set up CMake arguments
$CMakeArgs = @(
    "-B", $BuildDir,
    "-S", $ProjectRoot,
    "-DCMAKE_BUILD_TYPE=$BuildType"
)

# Add vcpkg toolchain if VCPKG_ROOT is set
if ($env:VCPKG_ROOT) {
    $VcpkgToolchain = Join-Path $env:VCPKG_ROOT "scripts\buildsystems\vcpkg.cmake"
    if (Test-Path $VcpkgToolchain) {
        $CMakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$VcpkgToolchain"
        Write-Host "Using vcpkg toolchain: $VcpkgToolchain"
    }
    else {
        Write-Warning "VCPKG_ROOT is set but toolchain file not found: $VcpkgToolchain"
    }
}
else {
    Write-Host "VCPKG_ROOT not set, trying to find Qt manually..."
}

# Add Qt path if specified (only if not using vcpkg)
if ($QtPath -ne "") {
    $CMakeArgs += "-DCMAKE_PREFIX_PATH=$QtPath"
    Write-Host "Using Qt path: $QtPath"
}
elseif (-not $env:VCPKG_ROOT) {
    # Try to find Qt automatically only if vcpkg is not available
    $QtPaths = @(
        "C:\Qt\5.15.2\msvc2019_64",
        "C:\Qt5\5.15.2\msvc2019_64",
        "D:\Qt\5.15.2\msvc2019_64",
        "D:\Qt5\5.15.2\msvc2019_64"
    )

    foreach ($QtDir in $QtPaths) {
        if (Test-Path $QtDir) {
            $CMakeArgs += "-DCMAKE_PREFIX_PATH=$QtDir"
            Write-Host "Found Qt at: $QtDir"
            break
        }
    }
}

# Add generator if specified
if ($Generator -ne "") {
    $CMakeArgs += "-G", $Generator
    Write-Host "Using generator: $Generator"
}

# Configure with CMake
Write-Host "Configuring with CMake..."
if ($Verbose) {
    Write-Host "CMake command: cmake $($CMakeArgs -join ' ')"
}

try {
    & cmake @CMakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE"
    }
}
catch {
    Write-Error "CMake configuration failed: $($_.Exception.Message)"
    exit 1
}

# Build with CMake
Write-Host ""
Write-Host "Building with CMake..."

$BuildArgs = @(
    "--build", $BuildDir,
    "--config", $BuildType
)

if ($Verbose) {
    $BuildArgs += "--verbose"
    Write-Host "Build command: cmake $($BuildArgs -join ' ')"
}

try {
    & cmake @BuildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
}
catch {
    Write-Error "Build failed: $($_.Exception.Message)"
    exit 1
}

# Check if executable was created
$ExePath = Join-Path $BuildDir "bin\AutoKey.exe"
if (Test-Path $ExePath) {
    Write-Host ""
    Write-Host "Build completed successfully!" -ForegroundColor Green
    Write-Host "Executable: $ExePath"
    
    # Get file information
    $FileInfo = Get-Item $ExePath
    $FileSizeMB = [math]::Round($FileInfo.Length / 1MB, 2)
    Write-Host "File size: $FileSizeMB MB"
    Write-Host "Created: $($FileInfo.CreationTime)"
    
    # Try to get version info
    try {
        $VersionInfo = $FileInfo.VersionInfo
        if ($VersionInfo.FileVersion) {
            Write-Host "Version: $($VersionInfo.FileVersion)"
        }
    }
    catch {
        # Version info not available
    }
    
}
else {
    Write-Error "Build completed but executable not found at: $ExePath"
    exit 1
}

# List build artifacts
Write-Host ""
Write-Host "Build artifacts:"
$BinDir = Join-Path $BuildDir "bin"
if (Test-Path $BinDir) {
    Get-ChildItem $BinDir | ForEach-Object {
        $size = if ($_.PSIsContainer) { "DIR" } else { "$([math]::Round($_.Length / 1KB, 1)) KB" }
        Write-Host "  $($_.Name) ($size)"
    }
}

Write-Host ""
Write-Host "Build process completed!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:"
Write-Host "  - Run the application: $ExePath"
Write-Host "  - Package for distribution: .\scripts\package.ps1"
Write-Host "  - Create installer: .\scripts\package.ps1 -CreateInstaller"
