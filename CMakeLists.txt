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

include_directories("${FILAMENT_DIR}/include")

link_directories("${FILAMENT_DIR}/lib")

file(GLOB_RECURSE NATIVE_SOURCES "${NATIVE_SRC_DIR}/*.cpp" "${NATIVE_SRC_DIR}/*.h")

add_library(KrysalisNative SHARED ${NATIVE_SOURCES})

set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}/lib
)

target_link_libraries(KrysalisNative filament math utils backend)

file(GLOB_RECURSE MANAGED_SOURCES "${MANAGED_SRC_DIR}/*.cs")

add_custom_target(ManagedBuild ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MANAGED_BUILD_DIR}
    COMMAND dotnet csc -out:${MANAGED_BUILD_DIR}/KrysalisManaged.dll ${MANAGED_SOURCES}
    COMMENT "Building C# project KrysalisManaged"
)

add_dependencies(ManagedBuild KrysalisNative)

add_custom_command(
    TARGET ManagedBuild POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MANAGED_BUILD_DIR}/KrysalisManaged.dll"
        "${ResonitePath}/rml_mods/"
    COMMENT "Copying KrysalisManaged.dll to Resonite mods folder"
)

if (WIN32)
    message(STATUS "Configuring for Windows")
elseif (APPLE)
    message(STATUS "Configuring for macOS")
elseif (UNIX)
    message(STATUS "Configuring for Linux")
endif()
