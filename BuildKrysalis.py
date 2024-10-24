import os
import subprocess
import sys
import shutil
from concurrent.futures import ThreadPoolExecutor, as_completed
import platform

current_directory = os.path.dirname(os.path.abspath(__file__))
should_copy_to_resonite = False

if platform.system() == "Windows":
    mods_path = "C:/Program Files (x86)/Steam/steamapps/common/Resonite/rml_mods/"
    native_ext = ".dll"
    executable_ext = ".exe"
elif platform.system() == "Linux":
    mods_path = "/home/username/.steam/steam/steamapps/common/Resonite/rml_mods/"
    native_ext = ".so"
    executable_ext = ""
elif platform.system() == "Darwin":
    mods_path = "/Users/username/Library/Application Support/Steam/steamapps/common/Resonite/rml_mods/"
    native_ext = ".dylib"
    executable_ext = ""

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

def copy_repo():
    for file in os.listdir("./target/release/"):
        if file.endswith(native_ext):
            shutil.copy2(f"./target/release/{file}", f"KrysalisManagedTestRunner/bin/net8.0")
            shutil.copy2(f"./KrysalisManagedTestApplication/bin/net8.0/KrysalisManagedTestApplication{executable_ext}",
                        f"./KrysalisManagedTestRunner/bin/net8.0")
    if should_copy_to_resonite:
        shutil.copy2(f"./KrysalisMod/bin/Release/net472/KrysalisMod{native_ext}",
                     mods_path)

def main():
    build_repo()
    copy_repo()

if __name__ == "__main__":
    main()
