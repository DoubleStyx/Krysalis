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
    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool runWindow(bool isUnitTest);

    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void openLogFile();

    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void closeLogFile();
}
