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
        Utils.InitializeLogging();  // Initialize logging callback
    }

    [HarmonyPatch(typeof(Engine), "BeginNewUpdate")]
    class Engine_BeginNewUpdate_Patch
    {
        static void Postfix(Engine __instance)
        {
            Utils.TestLogger("Hello world!");  // Test the logger
        }
    }
}
