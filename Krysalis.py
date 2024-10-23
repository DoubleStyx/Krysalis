import os
import subprocess
import sys
import shutil
import xml.etree.ElementTree as ET

current_directory = os.path.dirname(os.path.abspath(__file__))

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

def build_project(project_name):
    print(f"Building {project_name}...")

    if get_project_type(project_name) == "dotnet":
        dotnet_build_command = [
            "dotnet", 
            "build", 
            os.path.join(current_directory, project_name, f"{project_name}.csproj"),
            "--configuration",
            "Release"
        ]
        run_command(dotnet_build_command, cwd=project_name)
    elif get_project_type(project_name) == "cargo":
        cargo_build_command = ["cargo", "build", "--release", "--manifest-path", os.path.join(current_directory, project_name, "Cargo.toml")]
        run_command(cargo_build_command)
    else:
        print(f"Error: Could not determine the build system for {project_name}.")
        sys.exit(1)
    print(f"Build completed for {project_name}.")

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

def copy_dll(source, destination):
    dll_path = os.path.join(get_output_path(source), f"{source}.dll")
    print(f"Source DLL path: {dll_path}")

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


def run_tests(project_name):
    print(f"Running tests for {project_name}...")

    if get_project_type(project_name) == "dotnet":
        dotnet_test_command = ["dotnet", "test", os.path.join(current_directory, project_name), "--configuration", "Release"]
        run_command(dotnet_test_command, cwd=project_name)
    elif get_project_type(project_name) == "cargo":
        test_command = ["cargo", "test", "--release", "--manifest-path", os.path.join(current_directory, project_name, "Cargo.toml")]
        run_command(test_command)
    else:
        print(f"Error: Could not determine how to run tests for {project_name}.")
        sys.exit(1)
    print(f"Tests completed for {project_name}.")

def get_project_type(project_name):
    if os.path.exists(os.path.join(current_directory, project_name, f"{project_name}.csproj")):
        return "dotnet"
    elif os.path.exists(os.path.join(current_directory, project_name, "Cargo.toml")):
        return "cargo"
    else:
        print(f"Error: Could not determine the project type for {project_name}.")
        return "unknown"

def main():
    print("Starting build and test process...")

    build_project("KrysalisNative")
    build_project("KrysalisNativeTests")
    build_project("KrysalisManaged")
    build_project("KrysalisManagedTests")
    build_project("Krysalis")

    print("Building completed.")


    print("Copying DLLs...")

    copy_dll("KrysalisNative", "KrysalisManaged")
    copy_dll("KrysalisNative", "KrysalisManagedTests")

    print("DLLs copied.")


    print("Running tests...")
    run_tests("KrysalisNativeTests")
    run_tests("KrysalisManagedTests")
    print("Tests completed.")

    print("Build and test process completed successfully.")

if __name__ == "__main__":
    main()
