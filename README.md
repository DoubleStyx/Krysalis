# Krysalis
A [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod for [Resonite](https://resonite.com/) that replaces the Unity renderer in Resonite with a separate rendering pipeline based on the [Filament framework](https://github.com/google/filament/releases). It is also a prerequisite to a butterfly.

## Installation
1. Install [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader).
2. Extract the contents of the provided zip file into your `rml_mods` directory. This will include:
   - `Krysalis.dll`, which should be placed at the root of the `rml_mods` directory.
   - The `Krysalis` folder, which contains all relevant subfolder assets, `KrysalisManaged.dll`, and `KrysalisNative.dll`.
3. Start the game. If you want to verify that the mod is working, you can check your Resonite logs for relevant entries from the `Krysalis` mod.

## Building
### Prerequisites
1. **Visual Studio 2022**: You need Visual Studio 2022 with both the **.NET** and **C++ Desktop Development** workloads installed.
2. **Libraries**: Download and install the following libraries in the `C:/Libraries` directory:
   - **GLFW** (version 3.4): Download the Windows binary package from the [GLFW releases page](https://github.com/glfw/glfw/releases), and extract it to `C:/Libraries/glfw-3.4.bin.WIN64`.
   - **Filament** (version 1.55): Download the Windows version of Filament from the [Filament releases page](https://github.com/google/filament/releases), and extract it to `C:/Libraries/filament-windows`.
   
   After downloading and extracting, your `C:/Libraries` directory should look like this:
   
   ```bash
   d-----        10/16/2024   7:53 PM                filament-windows
   d-----         9/28/2024   7:20 PM                glfw-3.4.bin.WIN64
   ```

3. **Resonite**: Install Resonite. The mod is tested within the Resonite environment, so you'll need to have it installed to verify any changes. The ResoniteModLoader should also be installed and configured to load the mod.

### Building the Solution
1. **Open the Solution**: Open the `.sln` file in Visual Studio 2022.
2. **Build the Solution**: In Visual Studio, build the solution using either the **Debug** or **Release** configuration.
3. **Deploying the DLLs**: After a successful build, the generated DLLs and asset files will be automatically copied to the `rml_mods` folder of your Resonite installation.

### Testing
Once the build is complete and the DLLs are in place, launch Resonite and verify that the mod works by checking the game logs. You should see relevant log entries from the `Krysalis` mod if everything is set up correctly. You can also view Krysalis-specific logs in `rml_mods/Krysalis/logs`.

## Planned Features and Architecture
* Multithreaded rendering
* Asynchronous rendering
* Forward, deferred, and forward+ rendering paths
* PBR shader system
* Shader graph system
* Mesh shader emulation
* Compute shader support
* Cross-platform and standalone support