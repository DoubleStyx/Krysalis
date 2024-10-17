cmake_minimum_required(VERSION 3.15)
project(Krysalis CXX)

set(MANAGED_SRC_DIR "${CMAKE_SOURCE_DIR}/KrysalisManaged")
set(NATIVE_SRC_DIR "${CMAKE_SOURCE_DIR}/KrysalisNative")

set(MANAGED_BUILD_DIR "${CMAKE_BINARY_DIR}/Managed")
set(NATIVE_BUILD_DIR "${CMAKE_BINARY_DIR}/Native")

set(FILAMENT_DIR "C:/Libraries/filament-windows")

if (WIN32)
    set(ResonitePath "C:/Program Files (x86)/Steam/steamapps/common/Resonite/")
elseif (UNIX)
    set(ResonitePath "$ENV{HOME}/.steam/steam/steamapps/common/Resonite/")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

project ("Krysalis")

# Include sub-projects.
add_subdirectory ("KrysalisNative")
add_subdirectory ("KrysalisManaged")
