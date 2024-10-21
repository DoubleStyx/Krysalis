// Utils.cs
// helper/utility functions for managed tasks
using System.Runtime.InteropServices;
using ResoniteModLoader;
using FrooxEngine;
using HarmonyLib;

namespace KrysalisManaged;
internal class Utils
{
    /*
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogCallback(string message);

    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void startRenderingThread();

    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void registerLogCallback(LogCallback callback);

    [DllImport("Krysalis/KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void openLogFile();

    public static void LogMessage(string message)
    {
        ResoniteMod.Msg($"Native: {message}");
    }

    public static void InitializeLogging()
    {
        registerLogCallback(LogMessage);
        ResoniteMod.Msg("Registered log callback");
        openLogFile();
        ResoniteMod.Msg("Opened log file");
    }
    */
}


