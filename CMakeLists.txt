cmake_minimum_required(VERSION 3.15)
project(Krysalis CXX)

set(CSHARP_PROJECT_DIR "${CMAKE_SOURCE_DIR}/src/Krysalis-Managed")
set(FILAMENT_DIR "${CMAKE_SOURCE_DIR}/lib/Native/filament")
set(NATIVE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Krysalis-Native")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

set(BIN_DIR "${BUILD_DIR}/bin")
set(LIB_DIR "${BUILD_DIR}/lib")

set(ResonitePath "C:/Program Files (x86)/Steam/steamapps/common/Resonite/")
if (NOT EXISTS "${ResonitePath}")
    set(ResonitePath "$ENV{HOME}/.steam/steam/steamapps/common/Resonite/")
endif()

add_custom_target(ManagedBuild ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DIR}
    COMMAND dotnet build ${CSHARP_PROJECT_DIR}/Krysalis-Managed.csproj -o ${LIB_DIR}
    WORKING_DIRECTORY ${CSHARP_PROJECT_DIR}
    COMMENT "Building C# project Krysalis-Managed"
)

add_subdirectory(${FILAMENT_DIR} ${LIB_DIR}/filament)

file(GLOB_RECURSE NATIVE_SOURCES "${NATIVE_SRC_DIR}/*.cpp" "${NATIVE_SRC_DIR}/*.h")
add_executable(KrysalisNative ${NATIVE_SOURCES})

target_link_libraries(KrysalisNative PUBLIC filament)

set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR}
)

add_dependencies(KrysalisNative ManagedBuild)

add_custom_command(
    TARGET ManagedBuild POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIB_DIR}/Krysalis.dll"
        "${ResonitePath}rml_mods/"
    COMMENT "Copying Krysalis.dll to Resonite mods folder"
)

if (WIN32)
    message(STATUS "Configuring for Windows")
elseif (APPLE)
    message(STATUS "Configuring for macOS")
elseif (UNIX)
    message(STATUS "Configuring for Linux")
endif()
