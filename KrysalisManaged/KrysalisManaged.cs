// KrysalisManaged.cs
using FrooxEngine;
using HarmonyLib;
using ResoniteModLoader;

namespace KrysalisManaged;

public class KrysalisManaged : ResoniteMod
{
    internal const string VERSION_CONSTANT = "1.0.0";
    public override string Name => "Krysalis";
    public override string Author => "DoubleStyx";
    public override string Version => VERSION_CONSTANT;
    public override string Link => "https://github.com/DoubleStyx/Krysalis/";

    public override void OnEngineInit()
    {
        Harmony harmony = new Harmony("net.DoubleStyx.Krysalis");
        harmony.PatchAll();
        Utils.InitializeLogging();
        Utils.TestLogger("Initializing window...");
        Utils.startRenderingThread();
        Utils.TestLogger("Window initialized");
    }

    // We don't need to patch to initialize the window, just to get data from the Unity runner
    /*
    [HarmonyPatch(typeof(Engine), "FinishInitialization")]
    class Engine_FinishInitialization_Patch
    {
        static void Postfix(Engine __instance)
        {
            Utils.TestLogger("Initializing window...");
            Utils.startRenderingThread();
            Utils.TestLogger("Window initialized");
        }
    }
    */
}
