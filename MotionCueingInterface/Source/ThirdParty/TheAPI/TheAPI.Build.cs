using System.IO;
using UnrealBuildTool;

public class TheAPI : ModuleRules
{
	public TheAPI(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            // add include path
			string PlatformDir = Target.Platform.ToString();
			string IncPath = Path.Combine(ModuleDirectory, "include");
			PublicSystemIncludePaths.Add(IncPath);

            // add lib path
			string LibPath = Path.Combine(ModuleDirectory, "lib", PlatformDir);
			PublicLibraryPaths.Add(LibPath);

            /*
            // add static library
			string LibName = "TheAPI";
			PublicAdditionalLibraries.Add(LibName + ".lib");

            // add dynamic library
            string DLLName = LibName + ".dll";
			PublicDelayLoadDLLs.Add(DLLName);
            string BinaryPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/ThirdParty", PlatformDir));
            RuntimeDependencies.Add(Path.Combine(BinaryPath, DLLName));
            */

            // add macros
            PublicDefinitions.Add("WITH_THEAPI=1");
            PublicDefinitions.Add("__WINDOWS__");
            //PublicDefinitions.Add("_WIN32_WINNT_WIN10_TH2");
			//PublicDefinitions.Add("_WIN32_WINNT_WIN10_RS1");
			//PublicDefinitions.Add("_WIN32_WINNT_WIN10_RS2");
			//PublicDefinitions.Add("_WIN32_WINNT_WIN10_RS3");
			//PublicDefinitions.Add("_WIN32_WINNT_WIN10_RS4");
			//PublicDefinitions.Add("_WIN32_WINNT_WIN10_RS5");
        }
    }
}
