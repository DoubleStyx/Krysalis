# Krysalis
A [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader) mod for [Resonite](https://resonite.com/) that replaces the Unity renderer in Resonite with a custom-written renderer. It is also a prerequisite to a butterfly.

## Installation
1. Install [ResoniteModLoader](https://github.com/DoubleStyx/ResoniteModLoader).
2. Extract the contents of the provided zip file into your `rml_mods` directory. This will include:
   - `Krysalis.dll`, which should be placed at the root of the `rml_mods` directory.
   - The `Krysalis` folder, which contains all relevant subfolder assets, `KrysalisManaged.dll`, and `KrysalisNative.dll`.
3. Start the game. If you want to verify that the mod is working, you can check your Resonite logs for relevant entries from the `Krysalis` mod.

## Building
Build details will be added once the project is in a stable configuration. For now, note that it uses .NET, Rust, and Python.

## Planned Features and Architecture
* Multithreaded rendering
* Asynchronous rendering
* Forward, deferred, and forward+ rendering paths
* PBR shader system
* Shader graph system
* Mesh shader emulation
* Compute shader support
* Cross-platform and standalone support