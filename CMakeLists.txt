cmake_minimum_required(VERSION 3.15)
project(Krysalis CXX)

set(CSHARP_PROJECT_DIR "${CMAKE_SOURCE_DIR}/src/Krysalis-Managed")
set(FILAMENT_DIR "${CMAKE_SOURCE_DIR}/lib/Native/filament")
set(NATIVE_SRC_DIR "${CMAKE_SOURCE_DIR}/src/Krysalis-Native")
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

set(CSHARP_BUILD_DIR "${BUILD_DIR}/Managed")
set(FILAMENT_BUILD_DIR "${BUILD_DIR}/Native/filament")
set(NATIVE_BUILD_DIR "${BUILD_DIR}/Native")

add_custom_target(ManagedBuild ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CSHARP_BUILD_DIR}
    COMMAND dotnet build ${CSHARP_PROJECT_DIR}/Krysalis-Managed.csproj -o ${CSHARP_BUILD_DIR}
    WORKING_DIRECTORY ${CSHARP_PROJECT_DIR}
    COMMENT "Building C# project Krysalis-Managed"
)

add_subdirectory(${FILAMENT_DIR} ${FILAMENT_BUILD_DIR})

file(GLOB_RECURSE NATIVE_SOURCES "${NATIVE_SRC_DIR}/*.cpp" "${NATIVE_SRC_DIR}/*.h")
add_executable(KrysalisNative ${NATIVE_SOURCES})

target_link_libraries(KrysalisNative PUBLIC filament)

set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${NATIVE_BUILD_DIR}
)

add_dependencies(KrysalisNative ManagedBuild)

if (WIN32)
    message(STATUS "Configuring for Windows")
elseif (APPLE)
    message(STATUS "Configuring for macOS")
elseif (UNIX)
    message(STATUS "Configuring for Linux")
endif()
