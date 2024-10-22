cmake_minimum_required(VERSION 3.18)

project(KrysalisNativeTests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

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
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/
)

link_directories(
    ${FILAMENT_DIR}/lib/x86_64/md    
    ${GLFW_DIR}/lib-vc2022 
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/build/Release/
)

add_executable(KrysalisNativeTests Tests.cpp)
set_property(TARGET KrysalisNativeTests PROPERTY CXX_STANDARD 20)

enable_testing()

add_test(NAME KrysalisNativeTests COMMAND KrysalisNativeTests)

target_link_libraries(KrysalisNativeTests PRIVATE
    ${CMAKE_SOURCE_DIR}/../KrysalisNative/build/Release/KrysalisNative.lib
    gtest_main
    gtest
    opengl32
    Shlwapi
    kernel32
    filament.lib
    filameshio.lib
    utils.lib
    backend.lib
    bluegl.lib
    bluevk.lib
    filaflat.lib
    filamat.lib
    ibl.lib
    meshoptimizer.lib
    ktxreader.lib
    basis_transcoder.lib
    dracodec.lib
    camutils.lib
    stb.lib
    filament-iblprefilter.lib
    gltfio.lib
    gltfio_core.lib
    filabridge.lib
    mikktspace.lib
    shaders.lib
    viewer.lib
    geometry.lib        
    ibl-lite.lib        
    image.lib         
    matdbg.lib        
    uberarchive.lib     
    uberzlib.lib        
    vkshaders.lib     
    zstd.lib         
    smol-v.lib         
    glfw3.lib
    KrysalisNative.lib
)

target_compile_definitions(KrysalisNativeTests PRIVATE _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNativeTests PRIVATE _DEBUG)
    target_compile_options(KrysalisNativeTests PRIVATE /Zi)  
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNativeTests PRIVATE NDEBUG)
    target_compile_options(KrysalisNativeTests PRIVATE /O2) 
endif()