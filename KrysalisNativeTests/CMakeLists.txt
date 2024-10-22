cmake_minimum_required(VERSION 3.18)

# Define the project name and version
project(KrysalisNativeTests VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add the test executable to the tests
add_test(NAME KrysalisNativeTests COMMAND KrysalisNativeTests)

# Paths to pre-built libraries
if (WIN32)
    set(FILAMENT_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-windows.tgz")
    set(GLFW_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip")
elseif(APPLE)
    set(FILAMENT_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-v1.55.0-mac.tgz")
    set(GLFW_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.MACOS.zip")
elseif(UNIX)
    set(FILAMENT_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-v1.55.0-linux.tgz")
    find_package(GLFW 3.4 REQUIRED) # Use system package manager for GLFW on Linux
endif()

# Store binaries in the build/external directory
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)

# Ensure the external build directory exists
file(MAKE_DIRECTORY ${EXTERNAL_DIR})

# Download and extract Filament if not present
if (NOT EXISTS ${EXTERNAL_DIR}/filament)
    message(STATUS "Downloading Filament...")
    set(FILAMENT_ARCHIVE ${EXTERNAL_DIR}/filament.tgz)
    file(DOWNLOAD ${FILAMENT_URL} ${FILAMENT_ARCHIVE} SHOW_PROGRESS)

    message(STATUS "Extracting Filament...")
    file(ARCHIVE_EXTRACT
        INPUT ${FILAMENT_ARCHIVE}
        DESTINATION ${EXTERNAL_DIR}/filament
    )
    message(STATUS "Filament extracted successfully.")
endif()

# Download and extract GLFW if not present
if (NOT EXISTS ${EXTERNAL_DIR}/glfw)
    message(STATUS "Downloading GLFW...")
    set(GLFW_ARCHIVE ${EXTERNAL_DIR}/glfw.zip)
    file(DOWNLOAD ${GLFW_URL} ${GLFW_ARCHIVE} SHOW_PROGRESS)

    message(STATUS "Extracting GLFW...")
    file(ARCHIVE_EXTRACT
        INPUT ${GLFW_ARCHIVE}
        DESTINATION ${EXTERNAL_DIR}
    )
    message(STATUS "GLFW extracted successfully.")

    message(STATUS "Renaming GLFW directory...")
    file(RENAME ${EXTERNAL_DIR}/glfw-3.4.bin.WIN64 ${EXTERNAL_DIR}/glfw)
endif()

set(FILAMENT_DIR ${EXTERNAL_DIR}/filament)
set(GLFW_DIR ${EXTERNAL_DIR}/glfw)
set(MaterialsDirectory "${CMAKE_SOURCE_DIR}/../assets/materials")
set(BinDirectory "${CMAKE_SOURCE_DIR}/../assets/bin")
set(ResonitePath "${CMAKE_BIN_DIR}/Resonite")

add_subdirectory(${CMAKE_SOURCE_DIR}/../KrysalisNative KrysalisNative_build)

# Add include directories for the pre-built libraries
include_directories(
    ${FILAMENT_DIR}/include           # Filament's include directory (already adjusted)
    ${GLFW_DIR}/include       # GLFW's include directory
    ${CMAKE_SOURCE_DIR}/../external/include/
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/
)

# Link libraries for Filament and GLFW
link_directories(
    ${FILAMENT_DIR}/lib/x86_64/md       # Filament's lib directory
    ${GLFW_DIR}/lib-vc2022           # GLFW's lib directory
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/build/Release
)

add_library(KrysalisNativeTests SHARED
    Tests.cpp
)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET KrysalisNativeTests PROPERTY CXX_STANDARD 20)
endif()

# Set target properties for the output
set_target_properties(KrysalisNativeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(KrysalisNativeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_OUTPUT_DIRECTORY}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_OUTPUT_DIRECTORY}/Release
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_DIRECTORY}/Debug
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_DIRECTORY}/Release
)

# Link the libraries (Filament, GLFW, etc.)
target_link_libraries(KrysalisNativeTests PRIVATE
    opengl32
    Shlwapi
    kernel32
    ${FILAMENT_DIR}/lib/x86_64/md/filament.lib
    ${FILAMENT_DIR}/lib/x86_64/md/filameshio.lib
    ${FILAMENT_DIR}/lib/x86_64/md/utils.lib
    ${FILAMENT_DIR}/lib/x86_64/md/backend.lib
    ${FILAMENT_DIR}/lib/x86_64/md/bluegl.lib
    ${FILAMENT_DIR}/lib/x86_64/md/bluevk.lib
    ${FILAMENT_DIR}/lib/x86_64/md/filaflat.lib
    ${FILAMENT_DIR}/lib/x86_64/md/filamat.lib
    ${FILAMENT_DIR}/lib/x86_64/md/ibl.lib
    ${FILAMENT_DIR}/lib/x86_64/md/meshoptimizer.lib
    ${FILAMENT_DIR}/lib/x86_64/md/ktxreader.lib
    ${FILAMENT_DIR}/lib/x86_64/md/basis_transcoder.lib
    ${FILAMENT_DIR}/lib/x86_64/md/dracodec.lib
    ${FILAMENT_DIR}/lib/x86_64/md/camutils.lib
    ${FILAMENT_DIR}/lib/x86_64/md/stb.lib
    ${FILAMENT_DIR}/lib/x86_64/md/filament-iblprefilter.lib
    ${FILAMENT_DIR}/lib/x86_64/md/gltfio.lib
    ${FILAMENT_DIR}/lib/x86_64/md/gltfio_core.lib
    ${FILAMENT_DIR}/lib/x86_64/md/filabridge.lib
    ${FILAMENT_DIR}/lib/x86_64/md/mikktspace.lib
    ${FILAMENT_DIR}/lib/x86_64/md/shaders.lib
    ${FILAMENT_DIR}/lib/x86_64/md/viewer.lib
    ${FILAMENT_DIR}/lib/x86_64/md/geometry.lib         # Added geometry.lib
    ${FILAMENT_DIR}/lib/x86_64/md/ibl-lite.lib         # Added ibl-lite.lib
    ${FILAMENT_DIR}/lib/x86_64/md/image.lib            # Added image.lib
    ${FILAMENT_DIR}/lib/x86_64/md/matdbg.lib           # Added matdbg.lib
    ${FILAMENT_DIR}/lib/x86_64/md/uberarchive.lib      # Added uberarchive.lib
    ${FILAMENT_DIR}/lib/x86_64/md/uberzlib.lib         # Added uberzlib.lib
    ${FILAMENT_DIR}/lib/x86_64/md/vkshaders.lib        # Added vkshaders.lib
    ${FILAMENT_DIR}/lib/x86_64/md/zstd.lib             # Added zstd.lib
    ${FILAMENT_DIR}/lib/x86_64/md/smol-v.lib           # Added filament-utils.lib
    ${GLFW_DIR}/lib-vc2022/glfw3.lib
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/build/Release/KrysalisNative.lib
)

target_compile_definitions(KrysalisNativeTests PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)


# Define build configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNativeTests PRIVATE _DEBUG)
    target_compile_options(KrysalisNativeTests PRIVATE /Zi)  # Debug symbols for MSVC
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNativeTests PRIVATE NDEBUG)
    target_compile_options(KrysalisNativeTests PRIVATE /O2)  # Optimization for MSVC
endif()

add_dependencies(KrysalisNativeTests KrysalisNative)