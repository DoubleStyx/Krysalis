cmake_minimum_required(VERSION 3.18)

project(KrysalisNative VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)

file(MAKE_DIRECTORY ${EXTERNAL_DIR})

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

include_directories(
    ${FILAMENT_DIR}/include       
    ${GLFW_DIR}/include           
    ${CMAKE_SOURCE_DIR}/../external/include/
)

link_directories(
    ${FILAMENT_DIR}/lib/x86_64/md    
    ${GLFW_DIR}/lib-vc2022        
)

set(SOURCES
    KrysalisNative.cpp
    SceneBuilder.cpp
    Utils.cpp
    stb_image_impl.cpp
)

set(HEADERS
    KrysalisNative.h
    SceneBuilder.h
    Utils.h
)

add_library(KrysalisNative SHARED ${SOURCES} ${HEADERS})
set_property(TARGET KrysalisNative PROPERTY CXX_STANDARD 20)

target_link_libraries(KrysalisNative PRIVATE
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
    ${FILAMENT_DIR}/lib/x86_64/md/geometry.lib        
    ${FILAMENT_DIR}/lib/x86_64/md/ibl-lite.lib        
    ${FILAMENT_DIR}/lib/x86_64/md/image.lib         
    ${FILAMENT_DIR}/lib/x86_64/md/matdbg.lib        
    ${FILAMENT_DIR}/lib/x86_64/md/uberarchive.lib     
    ${FILAMENT_DIR}/lib/x86_64/md/uberzlib.lib        
    ${FILAMENT_DIR}/lib/x86_64/md/vkshaders.lib     
    ${FILAMENT_DIR}/lib/x86_64/md/zstd.lib         
    ${FILAMENT_DIR}/lib/x86_64/md/smol-v.lib         
    ${GLFW_DIR}/lib-vc2022/glfw3.lib
)

target_compile_definitions(KrysalisNative PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNative PRIVATE _DEBUG)
    target_compile_options(KrysalisNative PRIVATE /Zi)  
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNative PRIVATE NDEBUG)
    target_compile_options(KrysalisNative PRIVATE /O2) 
endif()