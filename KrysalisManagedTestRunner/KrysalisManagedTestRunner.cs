using System.Diagnostics;
using Xunit;

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
                    FileName = "../KrysalisManagedTestApplication/bin/Release/netstandard2.0/KrysalisManagedTestApplication.exe",
                    Arguments = "TestRenderer",
                    RedirectStandardOutput = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };

            process.Start();
            process.WaitForExit();

            Assert.Equal(0, process.ExitCode);
        }

        [Fact]
        public void TestRenderer()
        {
            RunTest("TestRenderer");
        }
    }
}
