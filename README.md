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
* Multithreaded rendering
* Asynchronous rendering
* Forward, deferred, and forward+ rendering paths
* PBR shader system
* Shader graph system
* Mesh shader emulation
* Compute shader support
* Cross-platform and standalone support