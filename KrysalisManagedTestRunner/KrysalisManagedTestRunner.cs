using System.Diagnostics;
using Xunit;
using System;

namespace KrysalisManagedTestRunner
{
    public class KrysalisManagedTestRunner
    {
        public void RunTest(string testName) 
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "KrysalisManagedTestApplication.exe",
                    Arguments = "TestRenderer",
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };

            process.Start();
            process.WaitForExit();
            string output = process.StandardOutput.ReadToEnd();
            Console.WriteLine(output);
            
            Assert.True(output.Contains("Test passed"));
        }

        [Fact]
        public void TestRenderer()
        {
            RunTest("TestRenderer");
        }
    }
}
