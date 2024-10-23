# Krysalis

Krysalis is a high-performance VR renderer with support for [Resonite](https://resonite.com/) (built as a [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod). It can be used to replace the default Unity renderer in Resonite, adding support for modern VR rendering techniques. Krysalis is also a prerequisite for a butterfly.

Krysalis is designed to be cross-platform-friendly by default. It uses Vulkan, DirectX, and Metal per platform. `netstandard2.0` is used to ensure the C# API is compatible when Resonite migrates to .NET 8.0. Libraries like `winit` are used to abstract away platform-specific differences.

The Krysalis project is split into three components:

- **`KrysalisNative.dll`**: Contains the renderer code written in Rust using graphics API bindings.
- **`KrysalisManaged.dll`**: The C# API, providing a user-friendly wrapper around the Rust FFI (Foreign Function Interface).
- **`Krysalis.dll`**: Integrates the renderer into Resonite, hooking into the game to leverage `KrysalisManaged` calls.

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

4. **Graphics API SDKs**

   Depending on your development platform, install the appropriate graphics API SDKs.

   - **Windows**:

     - **Vulkan SDK**:

       - Download and install the Vulkan SDK from [LunarG's website](https://vulkan.lunarg.com/sdk/home#windows).
       - Follow the installer instructions. The default settings should suffice.
       - After installation, set the `VULKAN_SDK` environment variable:

         ```powershell
         setx VULKAN_SDK "C:\VulkanSDK\1.3.xxx.x" /M
         ```

     - **DirectX 12 SDK**:

       - DirectX 12 is included with the Windows SDK.
       - Install the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/).
       - Ensure that the **Graphics Tools** feature is installed:
         - Go to **Apps & Features** > **Optional Features** > **Add a Feature**.
         - Select **Graphics Tools** and install.

   - **Linux**:

     - **Vulkan SDK**:

       - Download the Vulkan SDK from [LunarG's website](https://vulkan.lunarg.com/sdk/home#linux).
       - Extract the downloaded package.
       - Set up the environment variables:

         ```bash
         tar -xzf vulkansdk-linux-x86_64-1.3.xxx.x.tar.gz
         cd 1.3.xxx.x
         source setup-env.sh
         ```

       - You may want to add the `source setup-env.sh` line to your `~/.bashrc` or `~/.profile` for persistence.

     - **Dependencies**:

       - Install required packages:

         ```bash
         sudo apt-get install libvulkan1 mesa-vulkan-drivers vulkan-utils
         ```

   - **macOS**:

     - **Vulkan SDK with MoltenVK**:

       - Download the Vulkan SDK from [LunarG's website](https://vulkan.lunarg.com/sdk/home#mac).
       - Install the SDK by following the instructions.

     - **Metal Development Tools**:

       - Metal is included with Xcode.
       - Install the latest version of [Xcode](https://developer.apple.com/xcode/).
       - Ensure Command Line Tools are installed:

         ```bash
         xcode-select --install
         ```

     - **Set Up Environment Variables**:

       - Add the following lines to your shell configuration file (`~/.zshrc` or `~/.bash_profile`):

         ```bash
         export VULKAN_SDK="/path/to/vulkansdk-macos-1.3.xxx.x/macOS"
         export PATH="$VULKAN_SDK/bin:$PATH"
         export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
         export VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
         export VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"
         ```

       - Replace `/path/to/vulkansdk-macos-1.3.xxx.x/macOS` with the actual path where you installed the SDK.

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
   - Ensure platform-specific dependencies are correctly configured.

3. **Run the Tests**:

   - Navigate to the test project's output directory:

     - **Windows**:

       ```bash
       cd KrysalisManagedTests\bin\Release\net8.0
       ```

     - **Linux/macOS**:

       ```bash
       cd KrysalisManagedTests/bin/Release/net8.0
       ```

   - Run the `KrysalisManagedTests` executable:

     - **Windows**:

       ```bash
       KrysalisManagedTests.exe
       ```

     - **Linux/macOS**:

       ```bash
       ./KrysalisManagedTests
       ```

   A successful test run will output confirmation that all steps were completed successfully.

### Additional Notes

- **Environment Variables**:

  - Ensure that any environment variables required by the graphics SDKs are correctly set.
  - On Windows, you may need to restart your command prompt or IDE after setting environment variables.

- **Platform-Specific Considerations**:

  - On **macOS**, remember that Vulkan functions are implemented via MoltenVK on top of Metal.
  - On **Linux**, make sure your GPU drivers support Vulkan.

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