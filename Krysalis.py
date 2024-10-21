# Build projects in required order and execute unit tests
import os
import subprocess
import sys
import shutil

# Paths to your project directories
KrysalisNativeDir = os.path.join("KrysalisNative")
KrysalisNativeTestsDir = os.path.join("KrysalisNativeTests")
KrysalisManagedDir = os.path.join("KrysalisManaged")
KrysalisManagedTestsDir = os.path.join("KrysalisManagedTests")
KrysalisDir = os.path.join("Krysalis")

# Helper function to run a shell command
def run_command(command, cwd=None):
    try:
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {' '.join(command)}")
        print(e.stdout)
        print(e.stderr)
        sys.exit(1)

# Build function for a project
def build_project(project_name, project_dir):
    print(f"Building {project_name}...")
    build_dir = os.path.join(project_dir, "build")

    # Ensure the build directory exists
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Run CMake configuration and build commands
    cmake_command = ["cmake", ".."]
    build_command = ["cmake", "--build", ".", "--config", "Release"]

    run_command(cmake_command, cwd=build_dir)
    run_command(build_command, cwd=build_dir)

# Test function for running unit tests
def run_tests(project_name, project_dir):
    print(f"Running tests for {project_name}...")
    test_executable = os.path.join(project_dir, "build", "Release", f"{project_name}.exe")

    if not os.path.exists(test_executable):
        print(f"Error: Test executable not found for {project_name}")
        sys.exit(1)

    run_command([test_executable])

# Main function orchestrating the build and test process
def main():
    print("Starting build and test process...")

    # Step 1: Build the projects in the correct order
    build_project("KrysalisNative", KrysalisNativeDir)
    build_project("KrysalisNativeTests", KrysalisNativeTestsDir)
    build_project("KrysalisManaged", KrysalisManagedDir)
    build_project("KrysalisManagedTests", KrysalisManagedTestsDir)
    build_project("Krysalis", KrysalisDir)

    # Step 2: Run the unit tests in the correct order
    run_tests("KrysalisNativeTests", KrysalisNativeTestsDir)
    run_tests("KrysalisManagedTests", KrysalisManagedTestsDir)

    print("Build and test process completed successfully!")

if __name__ == "__main__":
    main()
