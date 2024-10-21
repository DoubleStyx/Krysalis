# Set the minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Define the project name and version
project(KrysalisNative VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard (equivalent to /std:c++20 in MSBuild)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent module
include(FetchContent)

# FetchContent for RapidJSON
FetchContent_Declare(
    RapidJSON
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG        v1.1.0  # Specify the version or tag
)

# Make RapidJSON available
FetchContent_MakeAvailable(RapidJSON)

# Specify source files (equivalent to ClCompile and ClInclude in MSBuild)
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

# Set target properties for the output (like DynamicLibrary in MSBuild)
set_target_properties(KrysalisNative PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Link the RapidJSON library
target_include_directories(KrysalisNative PRIVATE ${rapidjson_SOURCE_DIR}/include)

# Define build configurations (equivalent to Debug/Release settings in MSBuild)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNative PRIVATE _DEBUG)
    target_compile_options(KrysalisNative PRIVATE -g)  # Debug symbols
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNative PRIVATE NDEBUG)
    target_compile_options(KrysalisNative PRIVATE -O3)  # Optimization
endif()

# Link directories and dependencies (equivalent to AdditionalLibraryDirectories)
target_link_directories(KrysalisNative PRIVATE
    ${CommonLibDirs}
    ${FilamentLibDirDebug}  # You can adjust this for Release/Debug
)

# Post-build commands: Material and Mesh Compilation (Exec commands)
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling materials and meshes"
)

# Compile Materials
file(GLOB MATERIAL_FILES "${MaterialsDirectory}/*.mat")
foreach(matfile ${MATERIAL_FILES})
    add_custom_command(TARGET KrysalisNative POST_BUILD
        COMMAND "${BinDirectory}/matc.exe" -o "${MaterialsDirectory}/${matfile}.filamat" "${matfile}"
        COMMENT "Compiling material: ${matfile}"
        VERBATIM
    )
endforeach()

# Compile Meshes
file(GLOB MESH_FILES "${MeshesDirectory}/*.obj")
foreach(meshfile ${MESH_FILES})
    add_custom_command(TARGET KrysalisNative POST_BUILD
        COMMAND "${BinDirectory}/filamesh.exe" "${meshfile}" "${MeshesDirectory}/${meshfile}.filamesh"
        COMMENT "Compiling mesh: ${meshfile}"
        VERBATIM
    )
endforeach()

# Post-build copy step for the files (like PostBuild in MSBuild)
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying files after build"
)

# Copy .filamat files to the destination directory
file(GLOB COMPILED_MATERIAL_FILES "${MaterialsDirectory}/*.filamat")
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MaterialsDirectory} ${ResonitePath}/rml_mods/Krysalis/materials
    COMMENT "Copying .filamat files to ${ResonitePath}/rml_mods/Krysalis/materials"
)

# Copy .exe files to the bin directory
file(GLOB EXE_FILES "${BinDirectory}/*.exe")
add_custom_command(TARGET KrysalisNative POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${BinDirectory} ${ResonitePath}/rml_mods/Krysalis/bin
    COMMENT "Copying .exe files to ${ResonitePath}/rml_mods/Krysalis/bin"
)
