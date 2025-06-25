cmake_minimum_required(VERSION 3.16)
project(AutoKey VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure vcpkg integration
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Set vcpkg target triplet if not specified
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
        else()
            set(VCPKG_TARGET_TRIPLET "x86-windows" CACHE STRING "")
        endif()
    endif()
    message(STATUS "Using vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")
endif()

# Find Qt5 components
# First try to find Qt5 as a whole, then individual components
find_package(Qt5 QUIET COMPONENTS Core Widgets)
if(NOT Qt5_FOUND)
    # Fallback to individual components (vcpkg style)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    set(Qt5_FOUND TRUE)
endif()

# Set Qt5 properties
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    main.cpp
    mainwindow.cpp
    configmanager.cpp
    keysimulator.cpp
    classprofile.cpp
    actioneditdialog.cpp
)

# Header files
set(HEADERS
    mainwindow.h
    configmanager.h
    keysimulator.h
    classprofile.h
    actioneditdialog.h
)

# Resource files
set(RESOURCE_FILES
    resources.qrc
)

# Windows resource file for icon and version info
if(WIN32)
    list(APPEND RESOURCE_FILES autokey.rc)
endif()

# UI files (none - we create UI programmatically)
set(UI_FILES
)

# Create executable
add_executable(AutoKey ${SOURCES} ${HEADERS} ${UI_FILES} ${RESOURCE_FILES})

# Link Qt5 libraries
target_link_libraries(AutoKey Qt5::Core Qt5::Widgets)

# Link Windows libraries for keyboard/mouse simulation
if(WIN32)
    target_link_libraries(AutoKey user32)
endif()

# Set output directory
set_target_properties(AutoKey PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# For Debug builds, enable console window to see debug output
if(WIN32)
    set_target_properties(AutoKey PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # For Debug configuration, add console subsystem
    set_target_properties(AutoKey PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
    )
endif()

# Copy config file to output directory
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/bin/Release/config.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/bin/Debug/config.json COPYONLY)

# Print Qt5 information for debugging
message(STATUS "Qt5Core found: ${Qt5Core_FOUND}")
message(STATUS "Qt5Widgets found: ${Qt5Widgets_FOUND}")
message(STATUS "Qt5Core version: ${Qt5Core_VERSION}")
message(STATUS "Qt5Widgets version: ${Qt5Widgets_VERSION}")
message(STATUS "Qt5Core library: ${Qt5Core_LIBRARIES}")
message(STATUS "Qt5Widgets library: ${Qt5Widgets_LIBRARIES}")
if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "VCPKG_ROOT: $ENV{VCPKG_ROOT}")
    message(STATUS "Using vcpkg for Qt5 dependencies")
endif()
