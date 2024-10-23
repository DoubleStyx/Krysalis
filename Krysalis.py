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
    project_dirs = find_projects()  # Automatically detect all projects
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

def copy_dll(source, destination, absolute_destination_path=False):
    dll_path = os.path.join(get_output_path(source), f"{source}.dll")
    destination_dir = destination if absolute_destination_path else get_output_path(destination)

    if not os.path.exists(destination_dir):
        os.makedirs(destination_dir)
        print(f"Created directory: {destination_dir}")

    target_path = os.path.join(destination_dir, f"{source}.dll")
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
    copy_dll("KrysalisNative", "KrysalisNativeTests")
    copy_dll("KrysalisManaged", "KrysalisManagedTests")
    if should_copy_to_resonite:
        copy_dll("KrysalisNative", mods_path, True)
        copy_dll("KrysalisManaged", os.path.join(mods_path, "Krysalis"), True)
        copy_dll("KrysalisNative", os.path.join(mods_path, "Krysalis"), True)

    test_repo()  # Automatically find and run tests for all project directories

if __name__ == "__main__":
    main()
