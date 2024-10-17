# KrysalisNative CMakeLists.txt
project(KrysalisNative CXX)

# Add the C++ source files
file(GLOB_RECURSE NATIVE_SOURCES "*.cpp" "*.h")

# Create the shared library (DLL)
add_library(KrysalisNative SHARED ${NATIVE_SOURCES})

# Specify where to put the final DLL
set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
