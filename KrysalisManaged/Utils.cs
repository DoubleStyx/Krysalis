// Utils.cs
using System;
using System.Runtime.InteropServices;
using FrooxEngine;
using ResoniteModLoader;

namespace KrysalisManaged;
internal class Utils
{
    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void startRenderingThread();  // Return type changed to void

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogCallback(string message);

    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void RegisterLogCallback(LogCallback callback);

    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void TestLogger([MarshalAs(UnmanagedType.LPStr)] string msg);  // Marshaling the string correctly

    public static void LogMessage(string message)
    {
        ResoniteMod.Msg($"Native: {message}");
    }

    public static void InitializeLogging()
    {
        RegisterLogCallback(LogMessage);  // Register the C# callback
    }
}
