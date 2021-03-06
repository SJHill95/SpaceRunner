// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpaceRunner : ModuleRules
{
	public SpaceRunner(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Niagara", "UMG" });
	}
}
