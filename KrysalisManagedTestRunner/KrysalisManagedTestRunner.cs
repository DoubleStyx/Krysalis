using System.Diagnostics;
using Xunit;
using System;

namespace KrysalisManagedTestRunner
{
    public class KrysalisManagedTestRunner
    {
        public string RunTest(string testName) 
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "KrysalisManagedTestApplication.exe",
                    Arguments = testName,
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };

            process.Start();
            process.WaitForExit();
            string output = process.StandardOutput.ReadToEnd();
            
            return output;
        }

        [Fact]
        public void TestRenderer()
        {
            Assert.Contains("Test passed", RunTest("TestRenderer"));
        }

        [Fact]
        public void ForceFail()
        {
            Assert.Contains("Test failed", RunTest("ForceFail"));
        }

        [Fact]
        public void ForcePass()
        {
            Assert.Contains("Test passed", RunTest("ForcePass"));
        }
    }
}
