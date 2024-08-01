using UnrealBuildTool;

public class ClassRoguelike : ModuleRules
{
    public ClassRoguelike(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "Niagara",
            "UMG",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });

        PublicIncludePaths.AddRange(new string[] {
            "ClassRoguelike/Public"
        });

        PrivateIncludePaths.AddRange(new string[] {
            "ClassRoguelike/Private"
        });
    }
}
