cmake_minimum_required(VERSION 3.15)

# Declare the project as a C# project
project(KrysalisManaged LANGUAGES CSharp)

# Include C# utilities (for handling XAML and other designer properties)
include(CSharpUtilities)

# Set the target framework, .NET version, and other properties
set_property(TARGET KrysalisManaged PROPERTY DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
set_property(TARGET KrysalisManaged PROPERTY LANG_VERSION "10.0")
set_property(TARGET KrysalisManaged PROPERTY ALLOW_UNSAFE_BLOCKS TRUE)
set_property(TARGET KrysalisManaged PROPERTY DETERMINISTIC TRUE)

# If Debug configuration is used, embed the debug type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET KrysalisManaged PROPERTY DEBUG_TYPE "embedded")
endif()

# Add C# source files (replace with actual paths in your project)
add_executable(KrysalisManaged
    KrysalisManaged.cs
    Utils.cs
)

# Handle project references and dependencies
# You can reference external libraries or native DLLs like in the .csproj file.

# References to external .NET assemblies (equivalent to <Reference Include="..." />)
set_property(TARGET KrysalisManaged PROPERTY VS_DOTNET_REFERENCES
    "System"
    "System.Core"
    "System.Xml"
    "System.Xml.Linq"
    "Microsoft.CSharp"
    "FrooxEngine"
    "HarmonyLib"
    "ResoniteModLoader"
)

# Link the native DLL (KrysalisNative.dll) using NativeReference equivalent in CMake
set(ResonitePath "C:/Program Files (x86)/Steam/steamapps/common/Resonite/")
if (NOT EXISTS "${ResonitePath}")
    set(ResonitePath "$ENV{HOME}/.steam/steam/steamapps/common/Resonite/")
endif()

# Make sure the native DLL is available and not copied (matching <Private>False)
add_custom_command(TARGET KrysalisManaged POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ResonitePath}/rml_libs/KrysalisNative.dll"
        "${CMAKE_BINARY_DIR}/KrysalisNative.dll"
)

# Set the assembly properties (RootNamespace and AssemblyName equivalent)
set_property(TARGET KrysalisManaged PROPERTY VS_DOTNET_ROOT_NAMESPACE "KrysalisManaged")
set_property(TARGET KrysalisManaged PROPERTY OUTPUT_NAME "KrysalisManaged")

# Add additional properties if needed based on your .csproj file
