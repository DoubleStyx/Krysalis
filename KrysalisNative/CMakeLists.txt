# Set the minimum CMake version
cmake_minimum_required(VERSION 3.14)

# Define the project name and version
project(KrysalisNative VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent module
include(FetchContent)

# FetchContent for RapidJSON without processing its CMakeLists.txt
FetchContent_Declare(
    RapidJSON
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG        v1.1.0  # Specify the version or tag
)

# Download RapidJSON without configuring it
FetchContent_GetProperties(RapidJSON)
if(NOT RapidJSON_POPULATED)
    FetchContent_Populate(RapidJSON)
endif()

# FetchContent for GLFW
FetchContent_Declare(
    GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.4  # Example tag, change if necessary
)

FetchContent_MakeAvailable(GLFW)

# FetchContent for Filament (adjust to match your Filament usage)
FetchContent_Declare(
    Filament
    GIT_REPOSITORY https://github.com/google/filament.git
    GIT_TAG        v1.9.19  # Specify version or tag
)

FetchContent_MakeAvailable(Filament)

# Add include directories for the external libraries
include_directories(
    ${Filament_SOURCE_DIR}
    ${Filament_SOURCE_DIR}/third_party/glm
    ${Filament_SOURCE_DIR}/third_party/filament/include
    ${GLFW_SOURCE_DIR}/include
    ${RapidJSON_SOURCE_DIR}/include
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

# Link directories and dependencies
# Ensure these variables are defined in your CMakeLists.txt or passed as cache variables
# For example:
# set(CommonLibDirs "/path/to/common/libs")
# set(FilamentLibDirDebug "/path/to/filament/libs/debug")
target_link_directories(KrysalisNative PRIVATE
    ${CommonLibDirs}
    ${FilamentLibDirDebug}  # Adjust for Release/Debug
)

# Post-build commands: Material and Mesh Compilation
# Ensure that the variables BinDirectory, MaterialsDirectory, etc., are defined
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
