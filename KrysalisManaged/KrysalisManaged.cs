using FrooxEngine;
using HarmonyLib;
using ResoniteModLoader;

namespace KrysalisManaged;

public class KrysalisManaged : ResoniteMod {
	internal const string VERSION_CONSTANT = "1.0.0";
	public override string Name => "Krysalis";
	public override string Author => "DoubleStyx";
	public override string Version => VERSION_CONSTANT;
	public override string Link => "https://github.com/DoubleStyx/Krysalis/";

	public override void OnEngineInit() {
		Harmony harmony = new Harmony("net.DoubleStyx.Krysalis");
		harmony.PatchAll();
	}

	[HarmonyPatch(typeof(Engine), "BeginNewUpdate")]
	class Engine_BeginNewUpdate_Patch {
		static void Postfix(Engine __instance) {
			Msg("2 + 3 = " + Utils.Add(2, 3));
			Msg("Engine able to be initialized: " + Utils.StartEngine());
		}
	}
}