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
                case "TestRenderer":
                    result = KrysalisManagedAPI.KrysalisManagedAPI.StartRenderer();
                    break;
                default:
                    throw new Exception("Invalid argument");
            }

            if (result)
            {
                Console.WriteLine("Test passed");
            }
            else
            {
                throw new Exception("Test failed");
            }
        }
    }
}
