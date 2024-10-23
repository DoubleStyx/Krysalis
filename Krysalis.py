import os
import subprocess
import sys
import shutil
from concurrent.futures import ThreadPoolExecutor, as_completed
import xml.etree.ElementTree as ET
import platform

current_directory = os.path.dirname(os.path.abspath(__file__))
mods_path = "C:/Program Files (x86)/Steam/steamapps/common/Resonite/rml_mods/"
should_copy_to_resonite = True

def run_command(command, cwd=None):
    print(f"Running command: {' '.join(command)}")
    try:
        result = subprocess.run(command, cwd=cwd, check=True, capture_output=True, text=True)
        print(result.stdout)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}")
        print(f"Standard Output: {e.stdout}")
        print(f"Standard Error: {e.stderr}")
        sys.exit(1)

def build_solution():
    dotnet_build_command = ["dotnet", "build", "--configuration", "Release"]
    run_command(dotnet_build_command)

def build_workspace():
    cargo_workspace_command = ["cargo", "build", "--release"]
    run_command(cargo_workspace_command)

def build_repo():
    with ThreadPoolExecutor() as executor:
        future_to_build = {
            executor.submit(build_solution): "dotnet",
            executor.submit(build_workspace): "cargo"
        }
        for future in as_completed(future_to_build):
            build_type = future_to_build[future]
            try:
                future.result()
            except Exception as exc:
                print(f"Build for {build_type} generated an exception: {exc}")
                sys.exit(1)
            else:
                print(f"Build for {build_type} completed successfully.")

def get_platform_specific_extension():
    """Return the correct library extension for the current platform."""
    current_platform = platform.system().lower()
    if current_platform == "windows":
        return "dll"
    elif current_platform == "linux":
        return "so"
    elif current_platform == "darwin":
        return "dylib"
    else:
        print(f"Unsupported platform: {current_platform}")
        sys.exit(1)

def get_output_path(project_name):
    project_path = os.path.join(current_directory, project_name)
    project_type = get_project_type(project_path)

    if project_type == "dotnet":
        csproj_path = os.path.join(project_path, f"{project_name}.csproj")
        tree = ET.parse(csproj_path)
        root = tree.getroot()

        target_framework = None
        for element in root.findall(".//TargetFramework"):
            target_framework = element.text
            break

        if target_framework:
            return os.path.join(project_path, "bin", "Release", target_framework)
        else:
            print(f"Error: Could not determine the target framework for {project_name}.")
            return os.path.join(project_path, "bin", "Release")
    elif project_type == "cargo":
        root_target_dir = os.path.join(current_directory, "target", "release")
        if os.path.exists(root_target_dir):
            return root_target_dir
        else:
            print(f"Error: Could not find build output for {project_name}.")
            sys.exit(1)
    else:
        print(f"Error: Could not determine the output path for {project_name}.")
        sys.exit(1)

def copy_dll(source, destination, absolute_destination_path=False):
    source_output_path = get_output_path(source)
    project_type = get_project_type(os.path.join(current_directory, source))

    current_platform = platform.system().lower()
    extension = get_platform_specific_extension()

    if project_type == "cargo":
        # For cargo projects
        if current_platform == "windows":
            dll_filename = f"{source}.dll"
        elif current_platform == "linux":
            dll_filename = f"lib{source}.so"
        elif current_platform == "darwin":
            dll_filename = f"lib{source}.dylib"
        else:
            print(f"Unsupported platform for cargo project: {current_platform}")
            sys.exit(1)
    else:
        # For dotnet projects, always use "{source}.dll" regardless of platform
        dll_filename = f"{source}.dll"

    dll_path = os.path.join(source_output_path, dll_filename)
    if not os.path.exists(dll_path):
        print(f"Error: {dll_filename} for {source} not found in {source_output_path}")
        sys.exit(1)

    if absolute_destination_path:
        destination_dir = destination
    else:
        destination_dir = get_output_path(destination)

    if not os.path.exists(destination_dir):
        os.makedirs(destination_dir)
        print(f"Created directory: {destination_dir}")

    target_path = os.path.join(destination_dir, os.path.basename(dll_path))
    print(f"Copying {dll_path} to {target_path}")
    try:
        shutil.copy2(dll_path, target_path)
        print(f"Copied {dll_path} to {target_path}")
    except Exception as e:
        print(f"Error copying DLL: {e}")
        sys.exit(1)

def get_project_type(project_dir):
    if os.path.exists(os.path.join(project_dir, f"{os.path.basename(project_dir)}.csproj")):
        return "dotnet"
    elif os.path.exists(os.path.join(project_dir, "Cargo.toml")):
        return "cargo"
    else:
        return None

def main():
    build_repo()

    copy_dll("KrysalisNative", "KrysalisManagedTests")
    copy_dll("KrysalisManaged", "KrysalisManagedTests")
    if should_copy_to_resonite:
        copy_dll("KrysalisNative", mods_path, True)
        copy_dll("KrysalisManaged", os.path.join(mods_path, "Krysalis"), True)
        copy_dll("KrysalisNative", os.path.join(mods_path, "Krysalis"), True)

    # add unit tests here

if __name__ == "__main__":
    main()
