// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class firstperson415 : ModuleRules
{
	public firstperson415(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
