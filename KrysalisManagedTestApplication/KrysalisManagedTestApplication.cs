using System;
using KrysalisManagedAPI;

namespace KrysalisManagedTestApplication
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            bool result = false;

            switch (args[0])
            {
                case "test":
                    result = TestRenderer();
                    break;
                default:
                    throw new Exception("Invalid argument");
            }

            if (result)
            {
                Console.WriteLine("Test passed");
                Environment.Exit(0);
            }
            else
            {
                throw new Exception("Test failed");
                Environment.Exit(1);
            }
        }
    }
}
