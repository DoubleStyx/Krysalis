import os
import subprocess
import sys
import shutil
from concurrent.futures import ThreadPoolExecutor, as_completed
import xml.etree.ElementTree as ET

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
    dotnet_build_command = ["dotnet", "build", "Krysalis.sln", "--configuration", "Release"]
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
            else:
                print(f"Build for {build_type} completed successfully.")

def run_dotnet_test(project_dir):
    dotnet_test_command = ["dotnet", "test", "--configuration", "Release"]
    run_command(dotnet_test_command, cwd=project_dir)

def run_cargo_test(project_dir):
    cargo_test_command = ["cargo", "test", "--release"]
    run_command(cargo_test_command, cwd=project_dir)

def test_repo():
    project_dirs = find_projects()
    with ThreadPoolExecutor() as executor:
        future_to_test = {
            executor.submit(run_dotnet_test if get_project_type(project_dir) == "dotnet" else run_cargo_test, project_dir): project_dir for project_dir in project_dirs
        }
        for future in as_completed(future_to_test):
            project_dir = future_to_test[future]
            try:
                future.result()
            except Exception as exc:
                print(f"Tests for {project_dir} generated an exception: {exc}")
            else:
                print(f"Tests for {project_dir} completed successfully.")

def find_projects():
    project_dirs = []
    for entry in os.listdir(current_directory):
        project_dir = os.path.join(current_directory, entry)
        if os.path.isdir(project_dir) and (os.path.exists(os.path.join(project_dir, 'Cargo.toml')) or os.path.exists(os.path.join(project_dir, f"{entry}.csproj"))):
            project_dirs.append(entry)
    return project_dirs

def get_output_path(project_name):
    project_path = os.path.join(current_directory, project_name)
    project_type = get_project_type(project_path)

    if project_type == "dotnet":
        csproj_path = os.path.join(project_path, f"{project_name}.csproj")
        tree = ET.parse(csproj_path)
        root = tree.getroot()

        # Namespace handling for XML parsing
        namespace = {'msbuild': 'http://schemas.microsoft.com/developer/msbuild/2003'}
        ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')

        target_framework = None
        for element in root.findall(".//msbuild:TargetFramework", namespace):
            target_framework = element.text
            break

        if target_framework:
            return os.path.join(project_path, "bin", "Release", target_framework)
        else:
            print(f"Error: Could not determine the target framework for {project_name}.")
            return os.path.join(project_path, "bin", "Release")
    elif project_type == "cargo":
        # In a workspace, Cargo builds to the root 'target' directory
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

    if project_type == "cargo":
        # Handle Rust DLL naming conventions
        if os.name == 'nt':
            # Windows
            dll_filename = f"{source}.dll"
            lib_filename = f"lib{source}.dll"
        else:
            # Unix-like systems
            dll_filename = f"lib{source}.so"
            lib_filename = dll_filename  # On Unix, it's typically 'lib<name>.so'

        possible_filenames = [dll_filename, lib_filename]
    else:
        # .NET projects
        dll_filename = f"{source}.dll"
        possible_filenames = [dll_filename]

    # Find the correct DLL file
    for filename in possible_filenames:
        dll_path = os.path.join(source_output_path, filename)
        if os.path.exists(dll_path):
            break
    else:
        print(f"Error: DLL for {source} not found in {source_output_path}")
        sys.exit(1)

    # Determine destination directory
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

    # Uncomment the copy_dll calls as needed
    copy_dll("KrysalisNative", "KrysalisManagedTests")
    copy_dll("KrysalisNative", "KrysalisNativeTests")
    copy_dll("KrysalisManaged", "KrysalisManagedTests")
    if should_copy_to_resonite:
        copy_dll("KrysalisNative", mods_path, True)
        copy_dll("KrysalisManaged", os.path.join(mods_path, "Krysalis"), True)
        copy_dll("KrysalisNative", os.path.join(mods_path, "Krysalis"), True)

    # Uncomment if you want to run tests after building and copying
    test_repo()

if __name__ == "__main__":
    main()
