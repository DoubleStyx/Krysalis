// Utils.cs
using System.Runtime.InteropServices;
using ResoniteModLoader;

namespace KrysalisManaged;
internal class Utils
{
    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void startRenderingThread();

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogCallback(string message);

    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void RegisterLogCallback(LogCallback callback);

    [DllImport("KrysalisNative.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void TestLogger([MarshalAs(UnmanagedType.LPStr)] string msg);

    public static void LogMessage(string message)
    {
        ResoniteMod.Msg($"Native: {message}");
    }

    public static void InitializeLogging()
    {
        RegisterLogCallback(LogMessage);
    }
}


