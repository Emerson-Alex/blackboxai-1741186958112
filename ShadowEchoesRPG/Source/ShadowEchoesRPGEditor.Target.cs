// Copyright Shadow Echoes RPG. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShadowEchoesRPGEditorTarget : TargetRules
{
    public ShadowEchoesRPGEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        ExtraModuleNames.AddRange(new string[] { "ShadowEchoes" });

        // Enable editor-specific features
        bBuildDeveloperTools = true;
        bUseUnityBuild = false; // Disable unity build for faster iteration
        bUsePCHFiles = true;
        bUseXGECompiler = true;

        // Enable debugging tools
        bCompileICU = true;
        bCompilePhysX = true;
        bCompileAPEX = true;
        bCompileNvCloth = true;
        bCompileChaos = true;
        bCompileRecast = true;
        bCompileNavmeshSegment = true;
        
        // Enable enhanced input for editor
        bWithEnhancedInput = true;

        // Enable ray tracing in editor
        bWithRayTracing = true;

        // Enable DLSS and FSR in editor
        bWithDLSS = true;
        bWithFSR = true;

        // Enable Nanite and Lumen in editor
        bWithNanite = true;
        bWithLumen = true;
    }
}
