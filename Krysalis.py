import os
import subprocess
import sys
import shutil

current_directory = os.path.dirname(os.path.abspath(__file__))

def run_command(command, cwd=None):
    print(f"Running command: {' '.join(command)}")
    try:
        result = subprocess.run(command, cwd=cwd, check=True)
        print(result.stdout)
        print(result.stderr)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}")
        print("Standard Output:")
        print(e.stdout)
        print("Standard Error:")
        print(e.stderr)
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

def copy_dll(source, destination):
    dll_path = None
    if get_project_type(source) == "cargo":
        dll_path = os.path.join(current_directory, source, "target", "release", f"{source}.dll")
    elif get_project_type(source) == "dotnet":
        dll_path = os.path.join(current_directory, source, "bin", "Release", f"{source}.dll")
    else:
        print(f"Error: DLL not found at {dll_path}")
        sys.exit(1)

    target_dir = None
    if get_project_type(destination) == "dotnet":
        target_dir = os.path.join(current_directory, destination, "bin", "Release", "net8.0")
    elif get_project_type(destination) == "cargo":
        target_dir = os.path.join(current_directory, destination, "target", "release")
    else:
        print(f"Error: Could not determine the target directory for {destination}.")
        sys.exit(1)

    print(f"Copying {dll_path} to {target_dir}")
    try:
        shutil.copy2(dll_path, target_dir)
        print(f"Copied {dll_path} to {target_dir}")
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
        return "unknown"

def main():
    print("Starting build and test process...")

    build_project("KrysalisNative")
    build_project("KrysalisNativeTests")
    build_project("KrysalisManaged")
    build_project("KrysalisManagedTests")
    build_project("Krysalis")

    copy_dll("KrysalisNative", "KrysalisManaged")
    copy_dll("KrysalisNative", "KrysalisManagedTests")

    run_tests("KrysalisNativeTests")
    run_tests("KrysalisManagedTests")

    print("Build and test process completed successfully.")

if __name__ == "__main__":
    main()
