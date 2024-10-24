using System;
using KrysalisManagedAPI;

namespace KrysalisManagedTestApplication
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("Hello world");
            bool result = false;

            switch (args[0])
            {
                case "TestRenderer":
                    result = KrysalisManagedAPI.KrysalisManagedAPI.StartRenderer();
                    break;
                case "ForceFail":
                    result = false;
                    break;
                default:
                     Console.WriteLine("Invalid argument");
                     result = false;
                     break;
            }

            if (result)
            {
                Console.WriteLine("Test passed");
            }
            else
            {
                Console.WriteLine("Test failed");
            }
        }
    }
}
