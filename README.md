# Krysalis

Krysalis is a high-performance VR renderer for [Resonite](https://resonite.com/), built as a [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod. It replaces the default Unity renderer in Resonite, adding support for modern rendering techniques tailored for VR applications. Krysalis is also a key prerequisite for a butterfly.

Krysalis is designed to be crossplatform-friendly by default. It uses Vulkan for Windows and Linux, as well as MoltenVK for macOS support. Netstandard2.0 is used to ensure the C# API is compatible when Resonite migrates to .NET 8.0. Winit, among other libraries, are used to abstract away platform-specific differences.

The Krysalis project is split into three components:
- **`KrysalisNative.dll`**: Contains the renderer code written in Rust using Vulkan bindings.
- **`KrysalisManaged.dll`**: The C# API, providing a user-friendly wrapper around the Rust FFI (Foreign Function Interface).
- **`Krysalis.dll`**: Integrates the renderer into Resonite, hooking into the game to leverage KrysalisManaged calls.

## Installation

1. Install the [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader).
2. Extract the provided zip file into the `rml_mods` directory of your Resonite installation. The zip should contain:
   - `Krysalis.dll` (place in the root of the `rml_mods` folder).
   - The `Krysalis` folder, containing subfolder assets, `KrysalisManaged.dll`, and `KrysalisNative.dll`.
3. Launch Resonite. To verify that the mod is working, check the log files for entries from the `Krysalis` mod.

## Setting Up the Development Environment

To build and test Krysalis locally, you'll need to set up a few dependencies.

### Prerequisites

Before building the project, make sure the following tools are installed:

1. **Visual Studio 2022** (with C# Workflow)
   - Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/).
   - Ensure you have the **.NET Desktop Development** workload installed, which includes C# project support.

2. **Rustup**
   - Install **Rustup** by following the instructions on [Rust's official website](https://www.rust-lang.org/tools/install).
   - After installation, run the following commands to ensure Rust is set up correctly:
     ```bash
     rustup install stable
     rustup update
     ```

3. **Python 3.x**
   - Download and install [Python 3.x](https://www.python.org/downloads/).
   - Confirm Python is added to your system's PATH by running:
     ```bash
     python --version
     ```

4. **Vulkan SDK**
   - Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
   - After installation, ensure the necessary environment variables are set, such as:
     - `VULKAN_SDK=<VulkanSDKPath>`
     - Add `<VulkanSDKPath>\Bin` and `<VulkanSDKPath>\Lib` to your system's `PATH`.

### Building and Testing

Once all dependencies are installed and configured, follow these steps to build and test the project:

1. Clone the Krysalis repository:
    ```bash
    git clone https://github.com/DoubleStyx/Krysalis.git
    cd Krysalis
    ```

2. Run the Python build and test automation script:
    ```bash
    python Krysalis.py
    ```

This script will:
- Build both the Rust and C# components.
- Handle necessary DLL copying between project directories.
- Run unit tests for both native (Rust) and managed (C#) components.

A successful build will output confirmation that all steps were completed successfully.

## Planned Features and Architecture

The Krysalis renderer will feature a wide array of advanced rendering techniques:

- **Multithreaded Rendering**: Leveraging multiple CPU threads for parallelized rendering tasks.
- **Asynchronous Rendering**: Offloading non-blocking GPU tasks such as resource loading.
- **Forward, Deferred, and Forward+ Rendering Paths**: Supporting different lighting models for various rendering needs.
- **Physically-Based Rendering (PBR)**: Photorealistic material and lighting models based on physical principles.
- **Shader Graph System**: A visual, node-based editor for creating shaders without coding.
- **Mesh Shader Emulation**: Emulating next-gen mesh shading for handling complex geometry efficiently.
- **Compute Shader Support**: Utilizing compute shaders for GPGPU tasks such as particle systems, global illumination, etc.
- **Cross-Platform Support**: Ensuring portability across Windows, macOS, and Linux.
- **Ray Tracing Support**: Real-time ray tracing for accurate reflections, shadows, and global illumination.
- **Real-Time Denoising**: Techniques for reducing noise in ray-traced scenes.
- **Variable Rate Shading (VRS)**: Dynamically adjusting shading complexity based on scene regions.
- **Temporal Anti-Aliasing (TAA)**: High-quality anti-aliasing using temporal sampling.
- **Screen Space Reflections (SSR)**: Real-time reflections computed from the screen buffer.
- **Volumetric Rendering**: Realistic rendering of fog, smoke, and other volumetric effects.
- **Global Illumination (GI)**: Simulating indirect lighting for more realistic scenes.
- **Subsurface Scattering (SSS)**: Accurate rendering of materials like skin, wax, and marble.
- **Tiled and Clustered Shading**: Efficient light culling techniques for scenes with numerous light sources.
- **Hybrid Rasterization + Ray Tracing Pipeline**: Combining rasterization and ray tracing for optimized performance and visual fidelity.
- **Adaptive Level of Detail (LOD)**: Dynamically adjusting model complexity based on distance to the camera.
- **Virtual Texturing**: Efficient streaming of high-resolution textures.
- **Post-Processing Effects**: Advanced visual effects such as bloom, depth of field, and motion blur.
- **Dynamic Resolution Scaling (DRS)**: Adjusting resolution on the fly based on performance needs.
- **AI-Assisted Upscaling (DLSS, XeSS, FSR)**: Utilizing AI techniques to upscale lower-resolution frames with minimal loss in quality.
- **Procedural Generation**: Compute-based procedural generation of assets such as textures and landscapes.
- **Virtual Reality (VR) Optimizations**: Specific features for enhancing VR performance, such as foveated rendering.
- **GPU-Driven Rendering**: Offloading rendering control to the GPU for higher scalability.