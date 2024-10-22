// KrysalisManaged.cs
// Main API entry point
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace KrysalisManaged;

public class KrysalisManaged
{
    // Import the function from the Rust library
    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool main_ffi();
}
