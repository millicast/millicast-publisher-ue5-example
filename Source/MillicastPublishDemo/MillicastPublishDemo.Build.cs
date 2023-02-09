// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MillicastPublishDemo : ModuleRules
{
	public MillicastPublishDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "MillicastPublisher" });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "Renderer",
            "RHI",
            "RHICore",
			"RenderCore"
        });
	}
}
