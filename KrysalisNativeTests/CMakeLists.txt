cmake_minimum_required(VERSION 3.14)  # Specify a minimum CMake version
project(KrysalisNativeTests)

# Set the C++ standard (equivalent to LanguageStandard in vcxproj)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the Debug/Release configurations (equivalent to MSBuild configurations)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Add the source files (equivalent to ClCompile)
add_executable(KrysalisNativeTests
    Tests.cpp  # You can add more source files here if needed
)

# Include directories (equivalent to AdditionalIncludeDirectories)
target_include_directories(KrysalisNativeTests PRIVATE
    ${VCInstallDir}/UnitTest/include
    ${CommonIncludeDirs}
    ${KrysalisNativeDir}
)

# Define Debug/Release specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(KrysalisNativeTests PRIVATE
        _DEBUG
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    )
    target_link_directories(KrysalisNativeTests PRIVATE
        ${VCInstallDir}/UnitTest/lib
        ${CommonLibDirs}
        ${FilamentLibDirDebug}
        ${CMAKE_BINARY_DIR}/x64/Debug
    )
    target_link_libraries(KrysalisNativeTests PRIVATE
        ${CommonDependencies}
        KrysalisNative  # Link to the KrysalisNative library
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(KrysalisNativeTests PRIVATE
        NDEBUG
        _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    )
    target_link_directories(KrysalisNativeTests PRIVATE
        ${VCInstallDir}/UnitTest/lib
        ${CommonLibDirs}
        ${FilamentLibDirRelease}
        ${CMAKE_BINARY_DIR}/x64/Release
    )
    target_link_libraries(KrysalisNativeTests PRIVATE
        ${CommonDependencies}
        KrysalisNative
    )
endif()

# Set the properties specific to each build type
set_target_properties(KrysalisNativeTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}
)

