import os
import subprocess
import sys
import shutil
import xml.etree.ElementTree as ET
from concurrent.futures import ThreadPoolExecutor, as_completed

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
    # Build the .NET solution
    dotnet_sln_command = ["dotnet", "build", "Krysalis.sln", "--configuration", "Release"]
    run_command(dotnet_sln_command)

def build_workspace():
    # Build the Cargo workspace
    cargo_workspace_command = ["cargo", "build", "--release"]
    run_command(cargo_workspace_command)

def run_dotnet_tests():
    # Run .NET tests
    dotnet_test_command = ["dotnet", "test", "Krysalis.sln", "--configuration", "Release"]
    run_command(dotnet_test_command)

def run_cargo_tests():
    # Run Rust tests
    cargo_test_command = ["cargo", "test", "--release"]
    run_command(cargo_test_command)

def parallel_build():
    # Run Rust and .NET builds in parallel
    with ThreadPoolExecutor() as executor:
        future_to_build = {
            executor.submit(build_solution): "dotnet",
            executor.submit(build_workspace): "cargo"
        }
        for future in as_completed(future_to_build):
            build_system = future_to_build[future]
            try:
                future.result()
            except Exception as exc:
                print(f"Build for {build_system} generated an exception: {exc}")
            else:
                print(f"{build_system.capitalize()} build completed successfully.")

def parallel_tests():
    # Run Rust and .NET tests in parallel
    with ThreadPoolExecutor() as executor:
        future_to_test = {
            executor.submit(run_dotnet_tests): "dotnet",
            executor.submit(run_cargo_tests): "cargo"
        }
        for future in as_completed(future_to_test):
            test_system = future_to_test[future]
            try:
                future.result()
            except Exception as exc:
                print(f"Tests for {test_system} generated an exception: {exc}")
            else:
                print(f"{test_system.capitalize()} tests completed successfully.")

def get_output_path(project_name):
    if get_project_type(project_name) == "dotnet":
        csproj_path = os.path.join(current_directory, project_name, f"{project_name}.csproj")
        tree = ET.parse(csproj_path)
        root = tree.getroot()

        target_framework = None
        for element in root.findall(".//TargetFramework"):
            target_framework = element.text
            break

        if target_framework:
            return os.path.join(current_directory, project_name, "bin", "Release", target_framework)
        else:
            print(f"Error: Could not determine the target framework for {project_name}.")
            return os.path.join(current_directory, project_name, "bin", "Release")
    elif get_project_type(project_name) == "cargo":
        return os.path.join(current_directory, project_name, "target", "release")
    else:
        print(f"Error: Could not determine the output path for {project_name}.")
        sys.exit(1)

def copy_dll(source, destination, absolute_destination_path=False, condition=False):
    dll_path = os.path.join(get_output_path(source), f"{source}.dll")
    print(f"Source DLL path: {dll_path}")

    destination_dir = None

    if absolute_destination_path:
        destination_dir = destination
    else:
        destination_dir = get_output_path(destination)
    print(f"Destination directory: {destination_dir}")

    if not os.path.exists(destination_dir):
        os.makedirs(destination_dir)
        print(f"Created directory: {destination_dir}")

    target_path = os.path.join(destination_dir, f"{source}.dll")
    print(f"Target path: {target_path}")

    print(f"Copying {dll_path} to {target_path}")
    try:
        shutil.copy2(dll_path, target_path)
        print(f"Copied {dll_path} to {target_path}")
    except Exception as e:
        print(f"Error copying DLL: {e}")
        sys.exit(1)

def get_project_type(project_name):
    if os.path.exists(os.path.join(current_directory, project_name, f"{project_name}.csproj")):
        return "dotnet"
    elif os.path.exists(os.path.join(current_directory, project_name, "Cargo.toml")):
        return "cargo"
    else:
        print(f"Error: Could not determine the project type for {project_name}.")
        return "unknown"

def main():
    print("Starting build...")
    parallel_build()
    print("Building completed.")

    print("Copying DLLs...")
    copy_dll("KrysalisNative", "KrysalisManaged")
    copy_dll("KrysalisNative", "KrysalisManagedTests")
    if should_copy_to_resonite:
        copy_dll("KrysalisNative", mods_path, True, should_copy_to_resonite)
        copy_dll("KrysalisManaged", os.path.join(mods_path, "Krysalis"), True, should_copy_to_resonite)
        copy_dll("KrysalisNative", os.path.join(mods_path, "Krysalis"), True, should_copy_to_resonite)
    print("DLLs copied.")

    # Run tests in parallel
    print("Running tests...")
    #parallel_tests()
    #run_cargo_tests()
    #run_dotnet_tests()
    dotnet_test_command = ["dotnet", "test", "KrysalisManagedTests/KrysalisManagedTests.csproj", "--configuration", "Release"]
    run_command(dotnet_test_command)

    cargo_test_command = ["cargo", "test", "KrysalisNativeTest/Cargo.toml", "--release"]
    run_command(cargo_test_command)
    print("Tests completed.")

if __name__ == "__main__":
    main()
