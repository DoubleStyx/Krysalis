cmake_minimum_required(VERSION 3.14)
project(KrysalisNativeTests LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths for KrysalisNative and other dependencies
set(KrysalisNative_DIR ${CMAKE_SOURCE_DIR}/../KrysalisNative)
set(Filament_DIR ${CMAKE_SOURCE_DIR}/../external/filament)  # Adjust this if needed

# Include the KrysalisNative headers
include_directories(
    ${KrysalisNative_DIR}  # Path to KrysalisNative headers
    ${Filament_DIR}/include
)

# Add the source files for tests
add_executable(KrysalisNativeTests
    Tests.cpp  # Add any additional test files here
)

# Link KrysalisNative and its dependencies
target_link_libraries(KrysalisNativeTests PRIVATE
    KrysalisNative  # Link the KrysalisNative library
    ${Filament_DIR}/lib/x86_64/md/filament.lib
    ${Filament_DIR}/lib/x86_64/md/utils.lib
    ${Filament_DIR}/lib/x86_64/md/backend.lib
    ${Filament_DIR}/lib/x86_64/md/bluegl.lib
    ${Filament_DIR}/lib/x86_64/md/bluevk.lib
    # Add other dependencies as necessary
)

# Set build directories for debug and release
set_target_properties(KrysalisNativeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# Ensure KrysalisNative is built before KrysalisNativeTests
add_dependencies(KrysalisNativeTests KrysalisNative)
