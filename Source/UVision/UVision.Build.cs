using UnrealBuildTool;

public class UVision : ModuleRules
{
	public UVision(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"UVision/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UVision/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RenderCore",
				"Networking",
				"Sockets"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				"Renderer"
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
