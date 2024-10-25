import os
import subprocess
import sys
import shutil
from concurrent.futures import ThreadPoolExecutor, as_completed

current_directory = os.path.dirname(os.path.abspath(__file__))
mods_path = "C:/Program Files (x86)/Steam/steamapps/common/Resonite/rml_mods/"
should_copy_to_resonite = False

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
        if file.endswith(".dll"):
            shutil.copy2(f"./target/release/{file}", "KrysalisManagedTestRunner/bin/Release/net8.0/")
            shutil.copy2("./KrysalisManagedTestApplication/bin/Release/net8.0/KrysalisManagedTestApplication.exe",
                "./KrysalisManagedTestRunner/bin/Release/net8.0/")
    if should_copy_to_resonite:
        shutil.copy2("./KrysalisMod/bin/Release/net472/KrysalisMod.dll",
        mods_path)

def main():
    build_repo()

    copy_repo()

if __name__ == "__main__":
    main()
