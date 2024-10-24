import BuildKrysalis
import os
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed

def test_solution():
    dotnet_build_command = ["dotnet", "test", "--configuration", "Release"]
    BuildKrysalis.run_command(dotnet_build_command)

def test_workspace():
    cargo_workspace_command = ["cargo", "test", "--release"]
    BuildKrysalis.run_command(cargo_workspace_command)

def test_repo():
    with ThreadPoolExecutor() as executor:
        future_to_build = {
            executor.submit(test_solution): "dotnet",
            executor.submit(test_workspace): "cargo"
        }
        for future in as_completed(future_to_build):
            build_type = future_to_build[future]
            try:
                future.result()
            except Exception as exc:
                print(f"Build for {build_type} generated an exception: {exc}")
                sys.exit(1)

def main():
    BuildKrysalis.build_repo()

    #BuildKrysalis.copy_repo()
    
    #test_repo()

if __name__ == "__main__":
    main()