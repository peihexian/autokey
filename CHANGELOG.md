# Changelog

All notable changes to AutoKey for Diablo 3 will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed
- ✅ **Global Hotkeys (F5/F6) Now Working** - Fixed critical issue where F5/F6 hotkeys were not responding
- 🔧 Improved hotkey registration using direct Windows API calls
- 🎯 Fixed nativeEvent handling for proper Windows message processing
- 🖼️ Fixed system tray icon display issue

### Added
- 🎮 Action editor with keyboard capture functionality
- 📱 Enhanced system tray integration
- 🔧 Improved error handling and debugging
- 📝 Better user feedback for hotkey registration status

### Changed
- 🏗️ Simplified hotkey registration architecture
- 🎨 Updated UI with better icon support
- 📦 Improved build and packaging process

## [1.0.0] - Initial Release

### Added
- 🎮 Automated key pressing for Diablo 3
- 👥 5 pre-configured character class profiles:
  - Barbarian
  - Crusader
  - Demon Hunter
  - Monk
  - Wizard
- ⚙️ Customizable key sequences and intervals
- 🔥 Global hotkeys (F5 to start, F6 to stop)
- 📱 System tray integration
- 💾 JSON configuration storage
- 🎯 Profile management system
- 🔧 Configurable simulation settings

### Features
- **Global Hotkeys**: F5 to start, F6 to stop simulation
- **Profile System**: Pre-configured and customizable profiles
- **System Tray**: Minimize to tray for background operation
- **Configuration**: Persistent settings storage
- **Cross-Application**: Works while other applications are in focus

### System Requirements
- Windows 10/11
- Visual C++ Redistributable 2019/2022
- Qt 5.15.2 runtime libraries (included in release)

### Installation
1. Download the release ZIP file
2. Extract to a folder of your choice
3. Run AutoKey.exe
4. Configure your profiles and start gaming!

### Usage
- Configure your desired key sequences in the profile editor
- Set appropriate intervals between key presses
- Use F5 to start and F6 to stop the simulation
- The application can run in the background via system tray

### Notes
- This tool is designed for legitimate gameplay enhancement
- Ensure compliance with game terms of service
- Run as administrator if global hotkeys don't work initially
