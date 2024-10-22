import os
import subprocess
import sys

# Get the absolute path of the script directory
script_dir = os.path.abspath(os.path.dirname(__file__))

# Paths to your project directories using absolute paths
KrysalisNativeDir = os.path.join(script_dir, "KrysalisNative")
KrysalisNativeTestsDir = os.path.join(script_dir, "KrysalisNativeTests")
KrysalisManagedDir = os.path.join(script_dir, "KrysalisManaged")
KrysalisManagedTestsDir = os.path.join(script_dir, "KrysalisManagedTests")
KrysalisDir = os.path.join(script_dir, "Krysalis")

# Build output directories rooted at the project directory
build_dir_native = os.path.join(KrysalisNativeDir, "build")
build_dir_tests = os.path.join(KrysalisNativeTestsDir, "build")
build_dir_managed = os.path.join(KrysalisManagedDir, "build")
build_dir_managed_tests = os.path.join(KrysalisManagedTestsDir, "build")
build_dir_krysalis = os.path.join(KrysalisDir, "build")

# Helper function to run a shell command
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

# Build function for a project
def build_project(project_name, project_dir, build_dir):
    print(f"Building {project_name}...")

    if os.path.exists(os.path.join(project_dir, f"{project_name}.csproj")):
        # Use dotnet build for C# projects
        dotnet_build_command = ["dotnet", "build", os.path.join(project_dir, f"{project_name}.csproj"), "--configuration", "Release"]
        run_command(dotnet_build_command, cwd=project_dir)

    elif os.path.exists(os.path.join(project_dir, "CMakeLists.txt")):
        # Use CMake for C++ projects
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)

        # Specify the output directory to the Release folder under the build directory
        cmake_command = [
            "cmake", 
            "-S", project_dir, 
            "-B", build_dir, 
            "-G", "Visual Studio 17 2022", 
            "-A", "x64", 
            f"-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE={os.path.join(build_dir, 'Release')}"
        ]
        build_command = ["cmake", "--build", build_dir, "--config", "Release"]

        run_command(cmake_command)
        run_command(build_command)

    else:
        print(f"Error: Could not determine the build system for {project_name}.")
        sys.exit(1)

    print(f"Build completed for {project_name}.")

# Test function for running unit tests
def run_tests(project_name, project_dir, build_dir):
    print(f"Running tests for {project_name}...")

    if os.path.exists(os.path.join(project_dir, f"{project_name}.csproj")):
        # Use dotnet test for C# projects
        dotnet_test_command = ["dotnet", "test", project_dir, "--configuration", "Release"]
        run_command(dotnet_test_command, cwd=project_dir)

    elif os.path.exists(os.path.join(project_dir, "CMakeLists.txt")):
        # Use the compiled executable for C++ projects
        test_executable = os.path.join(build_dir, "Release", f"{project_name}.exe")

        if not os.path.exists(test_executable):
            print(f"Error: Test executable not found for {project_name}")
            sys.exit(1)

        run_command([test_executable])

    else:
        print(f"Error: Could not determine how to run tests for {project_name}.")
        sys.exit(1)

    print(f"Tests completed for {project_name}.")

# Main function orchestrating the build and test process
def main():
    print("Starting build and test process...")

    # Build all projects, detecting project type dynamically
    build_project("KrysalisNative", KrysalisNativeDir, build_dir_native)
    build_project("KrysalisNativeTests", KrysalisNativeTestsDir, build_dir_tests)
    build_project("KrysalisManaged", KrysalisManagedDir, build_dir_managed)
    build_project("KrysalisManagedTests", KrysalisManagedTestsDir, build_dir_managed_tests)
    build_project("Krysalis", KrysalisDir, build_dir_krysalis)

    # Run tests for the projects
    run_tests("KrysalisNativeTests", KrysalisNativeTestsDir, build_dir_tests)
    run_tests("KrysalisManagedTests", KrysalisManagedTestsDir, build_dir_managed_tests)

    print("Build and test process completed successfully.")

if __name__ == "__main__":
    main()
