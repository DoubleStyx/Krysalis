using FrooxEngine;
using HarmonyLib;
using ResoniteModLoader;

namespace Krysalis;
//More info on creating mods can be found https://github.com/resonite-modding-group/ResoniteModLoader/wiki/Creating-Mods
public class Krysalis : ResoniteMod {
	internal const string VERSION_CONSTANT = "0.0.1"; //Changing the version here updates it in all locations needed
	public override string Name => "Krysalis";
	public override string Author => "DoubleStyx";
	public override string Version => "0.0.1";
	public override string Link => "https://github.com/DoubleStyx/Krysalis";

	public override void OnEngineInit() {
		Harmony harmony = new Harmony("net.doublestyx.Krysalis");
		harmony.PatchAll();
	}

	// //Example of how a HarmonyPatch can be formatted, Note that the following isn't a real patch and will not compile.
	// [HarmonyPatch(typeof(ClassNameHere), "MethodNameHere")]
	// class ClassNameHere_MethodNameHere_Patch {
	// 	static void Postfix(ClassNameHere __instance) {
	// 		Msg("Postfix from Krysalis");
	// 	}
	// }
}
