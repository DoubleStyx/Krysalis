# Krysalis

Krysalis is a high-performance DirectX12 VR renderer with optional support for [Resonite](https://resonite.com/) (via `Krysalis.dll` which functions as a [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod). It can be used to replace the default Unity renderer in Resonite, adding support for modern VR rendering techniques. Krysalis is also a prerequisite for a butterfly.

The Krysalis project is split into three components:

- **`KrysalisNative.dll`**: Contains the renderer code written in Rust using graphics API bindings.
- **`KrysalisManaged.dll`**: The C# API, providing a user-friendly wrapper around the Rust FFI (Foreign Function Interface).
- **`Krysalis.dll`**: Integrates the renderer into Resonite, hooking into the game to leverage `KrysalisManaged` calls.

If you want to see what's being worked on currently, check out the [Krysalis project board](https://github.com/users/DoubleStyx/projects/3).

## Installation for Resonite

1. **Install the ResoniteModLoader**: Follow the instructions on the [ResoniteModLoader GitHub page](https://github.com/DoubleStyx/ResoniteModLoader) to install the mod loader.

2. **Extract Krysalis Mod Files**:

   - Download the latest release of Krysalis.
   - Extract the provided ZIP file into the `rml_mods` directory of your Resonite installation. The ZIP should contain:
     - `Krysalis.dll` (place in the root of the `rml_mods` folder).
     - The `Krysalis` folder, containing subfolder assets, `KrysalisManaged.dll`, and `KrysalisNative.dll`.

3. **Launch Resonite**:

   - Start the game as usual.
   - To verify that the mod is working, check the log files for entries from the `Krysalis` mod.

## Setting Up the Development Environment

To build and test Krysalis locally, you'll need to set up a few dependencies.

### Prerequisites

Before building the project, make sure the following tools are installed:

1. **Visual Studio 2022** (with C# and .NET Workloads)

   - Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/).
   - During installation, select the **.NET Desktop Development** workload, which includes C# project support.

2. **Rust and Cargo**

   - Install **Rustup** by following the instructions on [Rust's official website](https://www.rust-lang.org/tools/install).
   - After installation, run the following commands to ensure Rust is set up correctly:

     ```bash
     rustup install stable
     rustup update
     ```

3. **Python 3.x**

   - Download and install [Python 3.x](https://www.python.org/downloads/).
   - Ensure Python is added to your system's PATH by running:

     ```bash
     python --version
     ```

4. **DX12 SDK**

- DirectX 12 is included with the Windows SDK.
- Install the [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/).
- Ensure that the **Graphics Tools** feature is installed:
  - Go to **Apps & Features** > **Optional Features** > **Add a Feature**.
  - Select **Graphics Tools** and install.

### Building and Testing

Once all dependencies are installed and configured, follow these steps to build and test the project:

1. **Clone the Krysalis Repository**:

```bash
git clone https://github.com/DoubleStyx/Krysalis.git
cd Krysalis
```

2. **Run the Python Build Automation Script**:

```bash
python Krysalis.py
```

This script will:

- Build both the Rust and C# components.
- Handle library copying to Resonite and the test project.

3. **Run the Tests**:

- Navigate to the test project's output directory:

```bash
cd KrysalisManagedTests\bin\Release\net8.0
```

- Run the `KrysalisManagedTests` executable:

```bash
KrysalisManagedTests.exe
```

A successful test run will output confirmation that all steps were completed successfully.

## Planned Features and Architecture

The Krysalis renderer will feature a growing list of advanced rendering techniques as the project matures:

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