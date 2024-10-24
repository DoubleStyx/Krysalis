using System;
using KrysalisManagedAPI;

namespace KrysalisManagedTestApplication
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            bool result = KrysalisManagedAPI.KrysalisManagedAPI.StartRenderer();
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
