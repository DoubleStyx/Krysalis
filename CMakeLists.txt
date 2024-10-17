cmake_minimum_required(VERSION 3.15)
project(Krysalis CXX)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(MANAGED_SRC_DIR "${SRC_DIR}/KrysalisManaged")
set(NATIVE_SRC_DIR "${SRC_DIR}/KrysalisNative")

set(MANAGED_BUILD_DIR "${CMAKE_BINARY_DIR}/Managed")

set(ResonitePath "C:/Program Files (x86)/Steam/steamapps/common/Resonite/")
if (NOT EXISTS "${ResonitePath}")
    set(ResonitePath "$ENV{HOME}/.steam/steam/steamapps/common/Resonite/")
endif()

file(GLOB_RECURSE NATIVE_SOURCES "${NATIVE_SRC_DIR}/*.cpp" "${NATIVE_SRC_DIR}/*.h")
add_executable(KrysalisNative ${NATIVE_SOURCES})

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
