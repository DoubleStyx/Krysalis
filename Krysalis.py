import os
import subprocess
import sys

def run_command(command, cwd=None):
    print(f"Running command: {' '.join(command)}")
    try:
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
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

    if os.path.exists(os.path.join(project_name, f"{project_name}.csproj")):
        dotnet_build_command = [
            "dotnet", 
            "build", 
            os.path.join(project_name, f"{project_name}.csproj"),
            "--configuration",
            "Release"
        ]
        run_command(dotnet_build_command, cwd=project_name)
    elif os.path.exists(os.path.join(project_name, "Cargo.toml")):
        cargo_build_command = ["cargo", "build", "--release"]
        run_command(cargo_build_command)
    else:
        print(f"Error: Could not determine the build system for {project_name}.")
        sys.exit(1)
    print(f"Build completed for {project_name}.")

def run_tests(project_name):
    print(f"Running tests for {project_name}...")

    if os.path.exists(os.path.join(project_name, f"{project_name}.csproj")):
        dotnet_test_command = ["dotnet", "test", project_name, "--configuration", "Release"]
        run_command(dotnet_test_command, cwd=project_name)
    elif os.path.exists(os.path.join(project_name, "Cargo.toml")):
        test_command = ["cargo", "test", "--release"]
        run_command(test_command)
    else:
        print(f"Error: Could not determine how to run tests for {project_name}.")
        sys.exit(1)
    print(f"Tests completed for {project_name}.")

def main():
    print("Starting build and test process...")

    build_project("KrysalisNative")
    build_project("KrysalisNativeTests")
    build_project("KrysalisManaged")
    build_project("KrysalisManagedTests")
    build_project("Krysalis")

    run_tests("KrysalisNativeTests")
    run_tests("KrysalisManagedTests")

    print("Build and test process completed successfully.")

if __name__ == "__main__":
    main()