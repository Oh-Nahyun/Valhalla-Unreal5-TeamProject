// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ValhallaProject : ModuleRules
{
	public ValhallaProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AnimGraphRuntime", "AIModule", "Niagara", "UMG", "GameplayTags", "Slate", "SlateCore", "ChaosVehicles" });
	}
}
