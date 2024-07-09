using UnrealBuildTool;
using System.Collections.Generic;

public class ClassRoguelikeTarget : TargetRules
{
    public ClassRoguelikeTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange(new string[] { "ClassRoguelike" });


        // Ensure a unique build environment
        bOverrideBuildEnvironment = true;
    }
}
