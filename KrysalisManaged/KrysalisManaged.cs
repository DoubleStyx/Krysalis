using FrooxEngine;
using HarmonyLib;
using ResoniteModLoader;

namespace KrysalisManaged;
//More info on creating mods can be found https://github.com/DoubleStyx/ResoniteModLoader/wiki/Creating-Mods
public class KrysalisManaged : ResoniteMod {
	internal const string VERSION_CONSTANT = "1.0.0"; //Changing the version here updates it in all locations needed
	public override string Name => "KrysalisManaged";
	public override string Author => "DoubleStyx";
	public override string Version => VERSION_CONSTANT;
	public override string Link => "https://github.com/DoubleStyx/KrysalisManaged/";

	public override void OnEngineInit() {
		Harmony harmony = new Harmony("net.DoubleStyx.KrysalisManaged");
		harmony.PatchAll();
	}
}
