cmake_minimum_required(VERSION 3.14)
project(KrysalisNativeTests LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the KrysalisNative directory as a subdirectory
add_subdirectory(${CMAKE_SOURCE_DIR}/../KrysalisNative KrysalisNative_build)

# Include directories for KrysalisNative and other dependencies
include_directories(
    ${CMAKE_SOURCE_DIR}/../KrysalisNative  # Path to KrysalisNative headers
    ${CMAKE_SOURCE_DIR}/../external/filament/include  # Filament includes
)

# Add the source files for tests
add_executable(KrysalisNativeTests
    Tests.cpp  # Add any additional test files here
)

# Link KrysalisNative and its dependencies
target_link_libraries(KrysalisNativeTests PRIVATE
    KrysalisNative  # Link the KrysalisNative library
    ${CMAKE_SOURCE_DIR}/../external/filament/lib/x86_64/md/filament.lib
    ${CMAKE_SOURCE_DIR}/../external/filament/lib/x86_64/md/utils.lib
    ${CMAKE_SOURCE_DIR}/../external/filament/lib/x86_64/md/backend.lib
    ${CMAKE_SOURCE_DIR}/../external/filament/lib/x86_64/md/bluegl.lib
    ${CMAKE_SOURCE_DIR}/../external/filament/lib/x86_64/md/bluevk.lib
)

# Define the deprecation suppression after the target is created
target_compile_definitions(KrysalisNativeTests PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

# Set build directories for debug and release
set_target_properties(KrysalisNativeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# Ensure KrysalisNative is built before KrysalisNativeTests
add_dependencies(KrysalisNativeTests KrysalisNative)
