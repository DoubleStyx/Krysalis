# Krysalis
A [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod for [Resonite](https://resonite.com/) that replaces the Unity renderer in Resonite with a separate rendering pipeline. It is also a prerequisite to a butterfly.

## Installation
1. Install [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader).
1. Place [KrysalisManaged.dll](https://github.com/DoubleStyx/Krysalis/releases/latest/download/KrysalisManaged.dll) and [KrysalisNative.dll](https://github.com/DoubleStyx/Krysalis/releases/latest/download/KrysalisManaged.dll) into your `rml_mods` folder. This folder should be at `C:\Program Files (x86)\Steam\steamapps\common\Resonite\rml_mods` for a default install. You can create it if it's missing, or if you launch the game once with ResoniteModLoader installed it will create this folder for you.
1. Start the game. If you want to verify that the mod is working you can check your Resonite logs.

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

### Build Configuration
- **Platform**: The project currently only supports **Windows** builds and targets **x64** architecture.
- **Dependencies**: Ensure that the `C:/Libraries` directory contains the necessary GLFW and Filament files as outlined above. Future updates may allow for more flexible paths or versions, but for now, use the specified versions for compatibility.

### Building the Solution
1. **Open the Solution**: Open the `.sln` file in Visual Studio 2022.
2. **Build the Solution**: In Visual Studio, build the solution using the **Release** configuration. This will compile both the C# and C++ parts of the mod.
3. **Deploying the DLLs**: After a successful build, the generated DLLs (`KrysalisManaged.dll` and `KrysalisNative.dll`) will be automatically copied to the `rml_mods` folder of your Resonite installation.

   If the files aren’t automatically moved, you can manually copy them to:
   
   ```bash
   C:\Program Files (x86)\Steam\steamapps\common\Resonite\rml_mods
   ```

### Testing
Once the build is complete and the DLLs are in place, launch Resonite and verify that the mod works by checking the game logs. You should see relevant log entries from the `Krysalis` mod if everything is set up correctly.