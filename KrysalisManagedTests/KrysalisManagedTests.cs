using System;
using KrysalisManaged;

namespace KrysalisManagedTests
{
    class Program
    {
        #if Windows
        [STAThread]
        #endif
        static void Main(string[] args)
        {
            bool result = KrysalisManaged.KrysalisManaged.main_ffi();
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
