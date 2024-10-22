# Set the minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Define the project name and version
project(KrysalisNative VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Paths to pre-built libraries
set(FILAMENT_WINDOWS_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-windows.tgz")
set(FILAMENT_MAC_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-v1.55.0-mac.tgz")
set(FILAMENT_LINUX_URL "https://github.com/google/filament/releases/download/v1.55.0/filament-v1.55.0-linux.tgz")

set(GLFW_WINDOWS_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip")
set(GLFW_MAC_URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.MACOS.zip")

set(BUILD_DIR ${CMAKE_BINARY_DIR}/external) # Store binaries in the build/external directory

# Detect platform and download the appropriate binaries if not already present
if (WIN32)
    set(FILAMENT_TGZ ${BUILD_DIR}/filament-windows.tgz)
    set(GLFW_ZIP ${BUILD_DIR}/glfw-3.4.bin.WIN64.zip)
elseif(APPLE)
    set(FILAMENT_TGZ ${BUILD_DIR}/filament-v1.55.0-mac.tgz)
    set(GLFW_ZIP ${BUILD_DIR}/glfw-3.4.bin.MACOS.zip)
elseif(UNIX)
    set(FILAMENT_TGZ ${BUILD_DIR}/filament-v1.55.0-linux.tgz)
    find_package(GLFW 3.4 REQUIRED) # Use system package manager for GLFW on Linux
endif()

# Download and extract Filament if not present
if (NOT EXISTS ${BUILD_DIR}/filament)
    file(DOWNLOAD ${FILAMENT_TGZ} ${FILAMENT_TGZ})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${FILAMENT_TGZ} WORKING_DIRECTORY ${BUILD_DIR})
endif()

# Download and extract GLFW if not present (for Windows/macOS)
if ((WIN32 OR APPLE) AND NOT EXISTS ${BUILD_DIR}/glfw)
    file(DOWNLOAD ${GLFW_ZIP} ${GLFW_ZIP})
    if (WIN32)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${GLFW_ZIP} WORKING_DIRECTORY ${BUILD_DIR})
    elseif(APPLE)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${GLFW_ZIP} WORKING_DIRECTORY ${BUILD_DIR})
    endif()
endif()

# Paths to the extracted pre-built binaries
if (WIN32 OR APPLE)
    set(FILAMENT_DIR ${BUILD_DIR}/filament)
    set(GLFW_DIR ${BUILD_DIR}/glfw)
elseif(UNIX)
    set(FILAMENT_DIR ${BUILD_DIR}/filament)
endif()

# Include RapidJSON manually since it's header-only
include_directories(
    ${CMAKE_SOURCE_DIR}/external/include/rapidjson
)

# Add include directories for the pre-built libraries
include_directories(
    ${FILAMENT_DIR}/include
    ${GLFW_DIR}/include
)

# Link libraries for Filament and GLFW
link_directories(
    ${FILAMENT_DIR}/lib
    ${GLFW_DIR}/lib
)

# Specify source files
set(SOURCES
    KrysalisNative.cpp
    SceneBuilder.cpp
    Utils.cpp
)

set(HEADERS
    KrysalisNative.h
    SceneBuilder.h
    Utils.h
)

# Add the library
add_library(KrysalisNative SHARED ${SOURCES} ${HEADERS})

# Set target properties for the output
set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Link the libraries (Filament, GLFW, etc.)
target_link_libraries(KrysalisNative PRIVATE
    filament
    glfw
)

# Define build configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNative PRIVATE _DEBUG)
    target_compile_options(KrysalisNative PRIVATE -g)  # Debug symbols
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNative PRIVATE NDEBUG)
    target_compile_options(KrysalisNative PRIVATE -O3)  # Optimization
endif()

# Post-build commands: Material and Mesh Compilation
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling materials and meshes"
)

# Compile Materials
file(GLOB MATERIAL_FILES "${MaterialsDirectory}/*.mat")
foreach(matfile ${MATERIAL_FILES})
    get_filename_component(matname "${matfile}" NAME_WE)
    add_custom_command(TARGET KrysalisNative POST_BUILD
        COMMAND "${BinDirectory}/matc.exe" -o "${MaterialsDirectory}/${matname}.filamat" "${matfile}"
        COMMENT "Compiling material: ${matfile}"
        VERBATIM
    )
endforeach()

# Compile Meshes
file(GLOB MESH_FILES "${MeshesDirectory}/*.obj")
foreach(meshfile ${MESH_FILES})
    get_filename_component(meshname "${meshfile}" NAME_WE)
    add_custom_command(TARGET KrysalisNative POST_BUILD
        COMMAND "${BinDirectory}/filamesh.exe" "${meshfile}" "${MeshesDirectory}/${meshname}.filamesh"
        COMMENT "Compiling mesh: ${meshfile}"
        VERBATIM
    )
endforeach()

# Post-build copy step for the files
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying files after build"
)

# Copy .filamat files to the destination directory
file(GLOB COMPILED_MATERIAL_FILES "${MaterialsDirectory}/*.filamat")
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${MaterialsDirectory}" "${ResonitePath}/rml_mods/Krysalis/materials"
    COMMENT "Copying .filamat files to ${ResonitePath}/rml_mods/Krysalis/materials"
)

# Copy .exe files to the bin directory
file(GLOB EXE_FILES "${BinDirectory}/*.exe")
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${BinDirectory}" "${ResonitePath}/rml_mods/Krysalis/bin"
    COMMENT "Copying .exe files to ${ResonitePath}/rml_mods/Krysalis/bin"
)
