using System.Diagnostics;
using Xunit;

[assembly: CollectionBehavior(DisableTestParallelization = true)]

namespace KrysalisManagedTests
{
    public class KrysalisManagedTestsHelper
    {
        public static int Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("No argument passed. Exiting.");
                return 1;
            }

            // Run the specified test function
            switch (args[0])
            {
                case "main_ffi":
                    return KrysalisManaged.main_ffi() ? 0 : 1;

                case "other_test":
                    return KrysalisManaged.other_test() ? 0 : 1;

                default:
                    Console.WriteLine($"Unknown argument: {args[0]}");
                    return 1;
            }
        }
    }

    public class KrysalisManagedTests
    {
        private int RunHelperProgram(string testName)
        {
            // Get the path to the compiled helper executable
            string helperPath = "bin/Helper/KrysalisManagedTestsHelper.exe"; 

            // Set up the process to run the helper program
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                FileName = helperPath,
                Arguments = testName,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };

            using (Process process = new Process())
            {
                process.StartInfo = startInfo;
                process.Start();

                // Optionally capture the output or error if needed
                string output = process.StandardOutput.ReadToEnd();
                string error = process.StandardError.ReadToEnd();

                process.WaitForExit();

                if (process.ExitCode != 0)
                {
                    // If there's an error, print it for debugging
                    Debug.WriteLine($"Error: {error}");
                }

                return process.ExitCode; // Return the exit code (0 for success)
            }
        }

        [Fact]
        public void TestMainFfi()
        {
            // Run the helper program with the "main_ffi" argument
            int exitCode = RunHelperProgram("main_ffi");

            // Assert that the helper program returned success (exit code 0)
            Assert.Equal(0, exitCode);
        }
    }
}
