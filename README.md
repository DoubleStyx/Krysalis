# Krysalis
A [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod for [Resonite](https://resonite.com/) that replaces the Unity renderer in Resonite with a custom-written high-performance VR renderer. It is also a prerequisite to a butterfly.

The renderer will support VR, physically-based rendering, asynchronous timewarp, async rendering, forward+ pipelines, shader graphs, multithreading, and many other modern realtime rendering techniques. Currently, only Window is officially supported as this is the main platform Resonite is built against. However, the project could be rebuilt for other platform targets with some tweaks as Rust and C# are cross-platform (except for windowing libraries mostly). macOS support will require either MoltenVK integration or supporting Metal directly through an abstraction layer.

The main renderer code is written in Rust using Vulkan bindings and is contained within `KrysalisNative.dll`. The renderer's C# API is provided through `KryalisManaged.dll` and provides a lightweight wrapper that's friendlier than working directly with Rust's FFI calls. `Krysalis.dll` contains the actual patches for Resonite to hook KrysalisManaged calls into it.

## Installation
1. Install [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader).
2. Extract the contents of the provided zip file into your `rml_mods` directory. This will include:
   - `Krysalis.dll`, which should be placed at the root of the `rml_mods` directory.
   - The `Krysalis` folder, which contains all relevant subfolder assets, `KrysalisManaged.dll`, and `KrysalisNative.dll`.
3. Start the game. If you want to verify that the mod is working, you can check your Resonite logs for relevant entries from the `Krysalis` mod.

## Setting Up the Development Environment
To build and test the Krysalis project, you'll need a few tools installed and configured on your system.

### Prerequisites
Before building the project, ensure you have the following tools installed on your machine:

1. Visual Studio 2022 (with C# workflow)
- Download and install **Visual Studio 2022**.
- Ensure that the **.NET Desktop Development** workload is installed, which includes support for building C# projects.

2. Rustup
- Install **Rustup**, the Rust toolchain installer, by following the instructions on [Rust's official website](https://www.rust-lang.org/tools/install).
- After installation, ensure that Rust is available by running the following command in a terminal:
    ```bash
    rustup install stable
    rustup update
    ```

3. Python 3.x
- Install **Python 3.x** from [python.org](https://www.python.org/downloads/).
- Ensure Python is available in your system's PATH by running the following command:
    ```bash
    python --version
    ```
- Python is used to run the build and test automation script.

4. Vulkan SDK
- Download and install the **Vulkan SDK** from [LunarG's website](https://vulkan.lunarg.com/sdk/home).
- Ensure the Vulkan SDK environment is set up by checking the installation folder and adding the necessary environment variables.

### Environment Variable Configuration
After installing the Vulkan SDK, ensure the following environment variables are set (replace `<VulkanSDKPath>` with the actual installation path):
  - `VULKAN_SDK=<VulkanSDKPath>`
  - Add `<VulkanSDKPath>\Bin` and `<VulkanSDKPath>\Lib` to your system's `PATH`.

### Building and Testing

Once all prerequisites are installed and configured, you can build and test the Krysalis project by running the provided Python automation script. Follow these steps:

1. Clone the repository:
    ```bash
    git clone https://github.com/DoubleStyx/Krysalis.git
    cd Krysalis
    ```

2. Run the build and test script using Python:
    ```bash
    python Krysalis.py
    ```

   The script will:
   - Build the Rust and C# components of the project.
   - Copy necessary DLLs between project folders.
   - Run the unit tests for both the native and managed components.

If all steps succeed, the build and tests will complete successfully, and a message will confirm that the process was successful.

## Planned Features and Architecture

- **Multithreaded Rendering**  
  Efficient use of modern CPUs by parallelizing rendering tasks across multiple threads to improve performance.

- **Asynchronous Rendering**  
  Offloading tasks like texture streaming, resource loading, and GPU commands to improve overall rendering performance without stalling the main rendering loop.

- **Forward, Deferred, and Forward+ Rendering Paths**  
  Supporting multiple rendering pipelines to accommodate different needs, including Forward for simple scenes, Deferred for complex lighting, and Forward+ for optimized light culling.

- **Physically Based Rendering (PBR) Shader System**  
  High-quality materials and lighting models based on real-world physical properties to achieve photorealistic rendering.

- **Shader Graph System**  
  A node-based visual shader editor allowing artists and developers to create complex shaders without manually writing code.

- **Mesh Shader Emulation**  
  Simulating next-gen GPU mesh shading pipelines for higher geometric complexity by replacing traditional vertex and geometry shaders.

- **Compute Shader Support**  
  Utilizing compute shaders for tasks like particle systems, global illumination, post-processing effects, and more general-purpose GPU (GPGPU) computing.

- **Cross-Platform and Standalone Support**  
  Ensuring the renderer works across different platforms (Windows, macOS, Linux) and can function standalone from any specific engine dependencies.

- **Ray Tracing Support**  
  Integrating real-time ray tracing (RT) for global illumination, reflections, and refractions using modern APIs like DXR and Vulkan Ray Tracing.

- **Real-Time Denoising**  
  Real-time denoising techniques like temporal or spatial denoising to improve ray tracing and overall image quality, reducing noise in reflections and shadows.

- **Variable Rate Shading (VRS)**  
  Using variable shading rates to dynamically adjust shading complexity based on screen regions, improving performance without sacrificing noticeable visual quality.

- **Temporal Anti-Aliasing (TAA)**  
  High-quality anti-aliasing method that uses data from multiple frames to smooth out edges and eliminate aliasing artifacts.

- **Screen Space Reflections (SSR)**  
  Real-time reflections computed by sampling the screen buffer, providing more dynamic and accurate reflections without requiring additional ray tracing.

- **Volumetric Rendering**  
  Support for rendering volumetric effects like fog, smoke, and clouds with physically accurate scattering and absorption for realistic atmospheres.

- **Global Illumination (GI)**  
  Techniques like voxel-based or probe-based global illumination to simulate indirect lighting, improving overall scene realism by bouncing light between surfaces.

- **Subsurface Scattering (SSS)**  
  Rendering materials like skin, wax, and marble with physically accurate light diffusion beneath the surface, simulating organic translucency.

- **Tiled and Clustered Shading**  
  Advanced light culling techniques that divide the screen into tiles or clusters to optimize lighting computations for scenes with large numbers of dynamic lights.

- **Hybrid Rendering Pipeline (Rasterization + Ray Tracing)**  
  A hybrid approach combining traditional rasterization for basic geometry and real-time ray tracing for effects like reflections and global illumination.

- **Adaptive Level of Detail (LOD)**  
  Dynamically adjusting the complexity of models and textures based on distance from the camera to balance performance and quality.

- **Virtual Texturing**  
  Efficient texture streaming technique that allows extremely high-resolution textures by only loading visible portions into memory, reducing texture memory usage.

- **Decoupled Shading (Shading at a Reduced Rate)**  
  Separate shading computations from geometric detail, allowing more efficient shading without impacting the geometric detail of the scene.

- **Post-Processing Effects**  
  Support for advanced effects like bloom, depth of field, lens flares, motion blur, and chromatic aberration to enhance visual quality.

- **Dynamic Resolution Scaling (DRS)**  
  Dynamically adjusting the rendering resolution based on performance metrics, allowing for smoother frame rates without sacrificing too much visual fidelity.

- **AI-Assisted Upscaling (DLSS/XeSS/FSR)**  
  Integration with AI-based upscaling technologies (like NVIDIA DLSS, Intel XeSS, or AMD FSR) to render at lower resolutions while upscaling the final output to a higher resolution, maintaining performance and image quality.

- **Procedural Generation Support**  
  Compute-based generation of textures, landscapes, and models procedurally at runtime, enabling more dynamic and scalable content creation.

- **Virtual Reality (VR) Optimizations**  
  Specific optimizations for VR rendering like foveated rendering, stereo rendering paths, and low-latency frame reprojection to meet the demands of immersive experiences.

- **GPU-Driven Rendering**  
  Offloading rendering pipeline control to the GPU for better scalability and performance, especially on modern hardware with complex scenes.

These features collectively ensure the Krysalis project will be at the cutting edge of modern rendering techniques, supporting high-performance real-time graphics across multiple platforms.