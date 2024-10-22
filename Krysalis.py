import os
import subprocess
import sys

# Paths to your project directories
KrysalisNativeDir = os.path.join("KrysalisNative")
KrysalisNativeTestsDir = os.path.join("KrysalisNativeTests")
KrysalisManagedDir = os.path.join("KrysalisManaged")
KrysalisManagedTestsDir = os.path.join("KrysalisManagedTests")
KrysalisDir = os.path.join("Krysalis")

# Helper function to run a shell command
def run_command(command, cwd=None):
    print(f"Running command: {' '.join(command)}")
    try:
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
        print(result.stdout)
        print(result.stderr)  # Also print the stderr in case there are any warnings or errors
    except subprocess.CalledProcessError as e:
        # Log both stdout and stderr for debugging
        print(f"Error running command: {' '.join(command)}")
        print("Standard Output:")
        print(e.stdout)
        print("Standard Error:")
        print(e.stderr)
        sys.exit(1)

# Build function for a project
def build_project(project_name, project_dir):
    print(f"Building {project_name}...")
    build_dir = os.path.join(project_dir, "build")
    source_dir = project_dir

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    cmake_command = ["cmake", "-S", source_dir, "-B", build_dir, "-G", "Visual Studio 17 2022", "-A", "x64"]
    build_command = ["cmake", "--build", build_dir, "--config", "Release"]

    run_command(cmake_command)
    run_command(build_command)

    print(f"Build completed for {project_name}.")

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

    build_project("KrysalisNative", KrysalisNativeDir)
    build_project("KrysalisNativeTests", KrysalisNativeTestsDir)
    build_project("KrysalisManaged", KrysalisManagedDir)
    build_project("KrysalisManagedTests", KrysalisManagedTestsDir)
    build_project("Krysalis", KrysalisDir)

    run_tests("KrysalisNativeTests", KrysalisNativeTestsDir)
    run_tests("KrysalisManagedTests", KrysalisManagedTestsDir)

    print("Build and test process completed successfully.")

if __name__ == "__main__":
    main()
