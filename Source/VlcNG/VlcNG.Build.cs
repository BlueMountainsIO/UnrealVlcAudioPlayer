/* Copyright (C) 2018-2019 Blue Mountains GmbH. All Rights Reserved.
*/

using UnrealBuildTool;
using System.IO;
using System.Collections.Generic;

public class VlcNG : ModuleRules
{
    public VlcNG(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.Add("VlcNG/Private");
		
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject"
            }
		);

        PublicDefinitions.Add("DLL_EXPORT=1");

        string RuntimePath = Path.Combine(Target.UEThirdPartyBinariesDirectory , "VLC", Target.Platform.ToString());
		string SourcePath = Path.Combine(Target.UEThirdPartySourceDirectory, "VLC");

        PublicIncludePaths.Add(Path.Combine(SourcePath, "include"));
        PublicLibraryPaths.Add(Path.Combine(SourcePath, "lib"));
		
        PublicAdditionalLibraries.Add("libvlc.lib");
        PublicAdditionalLibraries.Add("libvlccore.lib");

		List<string> Dlls = new List<string>();
		Dlls.Add("libvlc.dll");
		Dlls.Add("libvlccore.dll");

		PublicDelayLoadDLLs.AddRange(Dlls);

		foreach (string Dll in Dlls)
		{
			RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/VLC/" + Target.Platform.ToString() + "/" + Dll);
		}
		
		foreach (string FileName in Directory.EnumerateFiles(Path.Combine(RuntimePath, "plugins"), "*", SearchOption.AllDirectories))
		{
			string DependencyName = FileName.Substring(Target.UEThirdPartyBinariesDirectory.Length).Replace('\\', '/');
			RuntimeDependencies.Add("$(EngineDir)/Binaries/ThirdParty/" + DependencyName);
		}
    }
}
