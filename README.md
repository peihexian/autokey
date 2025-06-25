# AutoKey for Diablo 3

An automated key-pressing application designed specifically for Diablo 3 gameplay. This Qt5 C++ application allows you to configure and automate key sequences for different character classes, reducing repetitive strain during long gaming sessions.

## Features

- **Multiple Character Profiles**: 5 pre-configured class profiles with customizable settings
- **Flexible Key/Mouse Configuration**: Support for keyboard keys (1-4) and mouse buttons (left/right click)
- **Configurable Intervals**: Set individual timing intervals for each action to match ability cooldowns
- **Global Hotkeys**: F5 to start, F6 to stop simulation (works even when game is in focus)
- **System Tray Integration**: Minimize to tray and control from system tray menu
- **JSON Configuration**: Save and load custom configurations
- **User-Friendly GUI**: Easy-to-use tabbed interface for configuration

## Pre-configured Profiles

1. **Barbarian - Basic**: Keys 2, 3, 4 with moderate intervals
2. **Wizard - Combo**: Keys 1, 2, 3, 4 + right mouse click with varied intervals
3. **Demon Hunter - Rapid**: Left click, keys 2, 3 + right click with fast intervals
4. **Monk - Balanced**: Keys 1, 2, 3, 4 with balanced timing
5. **Necromancer - Pet Build**: Keys 1, 2, 3 + right click with longer intervals for pet management

## System Requirements

- Windows 10/11
- Qt5 (5.15 or later recommended)
- CMake 3.16 or later
- Visual Studio 2019/2022 or MinGW compiler

## Building from Source

### Prerequisites

1. **Qt5 development libraries** (choose one):
   - **Option A (Recommended)**: vcpkg-managed Qt5
   - **Option B**: Qt5 installer from qt.io
2. **CMake** 3.16 or later
3. **C++ compiler** (Visual Studio 2019/2022 or MinGW)
4. **vcpkg** (if using vcpkg-managed Qt5)

### Build Steps

#### Option A: Using vcpkg (Recommended)

```bash
# Ensure VCPKG_ROOT environment variable is set
# Install Qt5 via vcpkg (if not already installed)
vcpkg install qt5-base:x64-windows qt5-widgets:x64-windows

# Clone or extract the source code
cd autokey

# Build using the provided script
.\scripts\build.ps1

# Or manually:
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

#### Option B: Using Qt Installer

```bash
# Clone or extract the source code
cd autokey

# Create build directory
mkdir build
cd build

# Configure with CMake (adjust Qt path as needed)
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"

# Build the project
cmake --build . --config Release

# The executable will be in build/bin/AutoKey.exe
```

### Alternative: Using Qt Creator

1. Open `CMakeLists.txt` in Qt Creator
2. Configure the project with your Qt kit
3. Build and run

### Using Visual Studio Code (Recommended)

1. **Setup Development Environment**:

   ```bash
   # Run the setup script to check/install dependencies
   .\scripts\setup-dev.ps1 -CheckOnly

   # Or install missing components automatically
   .\scripts\setup-dev.ps1 -InstallVSCode -InstallCMake
   ```

2. **Open in VS Code**:

   ```bash
   # Open the workspace file
   code AutoKey.code-workspace

   # Or open the folder directly
   code .
   ```

3. **Build and Run**:

   - Press `Ctrl+Shift+P` and run "CMake: Configure"
   - Press `Ctrl+Shift+B` to build (or use "CMake: Build")
   - Press `F5` to debug or `Ctrl+F5` to run without debugging

4. **Available VS Code Tasks**:
   - **Build AutoKey**: Builds the project in Release mode
   - **Package AutoKey**: Creates distribution package
   - **Clean Build**: Cleans and rebuilds the project
   - **Setup Development Environment**: Checks development dependencies

## Usage

### Basic Usage

1. **Launch the Application**: Run `AutoKey.exe`
2. **Select a Profile**: Choose from the dropdown in the "Profiles" tab
3. **Configure Actions**: Switch to "Actions" tab to view/modify key sequences
4. **Start Simulation**: Click "Start (F5)" or press F5 globally
5. **Stop Simulation**: Click "Stop (F6)" or press F6 globally

### Creating Custom Profiles

1. Go to the "Profiles" tab
2. Click "Add" to create a new profile
3. Enter a name for your profile
4. Switch to "Actions" tab
5. Click "Add Action" to add key/mouse actions
6. Configure the type (Keyboard/Mouse), key, and interval for each action
7. Save your configuration

### Global Hotkeys

- **F5**: Start the current profile's simulation
- **F6**: Stop any running simulation

These hotkeys work globally, even when Diablo 3 is in focus.

### System Tray

- The application can minimize to the system tray
- Right-click the tray icon for quick access to start/stop functions
- Double-click to show/hide the main window

## Configuration

### Settings Tab

- **Hotkeys**: View current hotkey assignments (F5/F6 are fixed)
- **Application Settings**:
  - Minimize to system tray
  - Start minimized
- **Configuration Management**: Save/load custom configurations

### JSON Configuration

The application saves settings in `config.json`. You can:

- Backup configurations by copying this file
- Share configurations with other users
- Manually edit for advanced customization

## Development and CI/CD

### GitHub Actions Workflows

The project includes automated CI/CD pipelines:

1. **Build Workflow** (`.github/workflows/build.yml`):

   - Triggers on push to main/develop branches and pull requests
   - Builds the application with Qt5 and MSVC
   - Runs basic code analysis and tests
   - Uploads build artifacts

2. **Release Workflow** (`.github/workflows/release.yml`):

   - Triggers on version tags (e.g., `v1.0.0`)
   - Builds and packages the application
   - Creates GitHub releases with ZIP archives
   - Includes windeployqt for Qt dependency bundling

3. **CodeQL Security Analysis** (`.github/workflows/codeql.yml`):
   - Runs security and quality analysis
   - Scans for vulnerabilities and code issues
   - Scheduled weekly scans

### Creating a Release

To create a new release:

1. **Tag the version**:

   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```

2. **GitHub Actions will automatically**:

   - Build the application
   - Package with Qt dependencies
   - Create a GitHub release
   - Upload the distribution ZIP file

3. **Manual release** (if needed):
   ```bash
   # Build and package locally
   .\scripts\build.ps1 -BuildType Release
   .\scripts\package.ps1
   ```

### Development Scripts

- `scripts/setup-dev.ps1`: Development environment setup
- `scripts/build.ps1`: Build automation script
- `scripts/package.ps1`: Packaging and distribution script

## Technical Implementation

### Key Simulation

The application uses Windows API functions for global key/mouse simulation:

- `SendInput()` for keyboard and mouse events
- `RegisterHotKey()` for global hotkey detection
- Works with any DirectX/OpenGL application including Diablo 3

### Architecture

- **Qt5 Framework**: Modern C++ GUI framework
- **JSON Configuration**: Human-readable settings storage
- **Timer-based Execution**: Precise interval control using QTimer
- **Windows Integration**: Native Windows API for input simulation

## Important Notes

### Game Compatibility

- Designed specifically for Diablo 3
- Should work with most versions of Diablo 3
- May work with other similar games

### Terms of Service

- This tool is designed for legitimate gameplay enhancement
- Users are responsible for ensuring compliance with game terms of service
- The tool simulates normal keyboard/mouse input and does not modify game files

### Performance

- Minimal CPU usage when running
- No impact on game performance
- Precise timing control for optimal ability rotation

## Troubleshooting

### Global Hotkeys Not Working

- Try running the application as Administrator
- Check if other applications are using F5/F6 hotkeys
- Ensure Windows is not blocking the application

### Key Simulation Not Working in Game

- Run the application as Administrator
- Ensure Diablo 3 is running in windowed or borderless windowed mode
- Check Windows UAC settings

### Configuration Not Saving

- Ensure the application has write permissions to its directory
- Check if antivirus software is blocking file access

## License

This project is provided as-is for educational and personal use. Users are responsible for compliance with applicable terms of service and local laws.

## Support

For issues, suggestions, or contributions, please refer to the project documentation or contact the development team.

---

**Disclaimer**: This application is an independent project and is not affiliated with Blizzard Entertainment or Diablo 3. Use at your own discretion and ensure compliance with game terms of service.
