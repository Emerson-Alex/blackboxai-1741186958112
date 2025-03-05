// Copyright Shadow Echoes RPG. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShadowEchoesRPGTarget : TargetRules
{
    public ShadowEchoesRPGTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;

        ExtraModuleNames.AddRange(new string[] { "ShadowEchoes" });

        // Enable features needed for our game
        bUsesSteam = false;
        bUsesOnlineSubsystem = false;
        bUseLoggingInShipping = true;

        // Enable debug features in non-shipping builds
        if (Configuration != UnrealTargetConfiguration.Shipping)
        {
            bUseLoggingInShipping = true;
            bUseMallocProfiler = true;
            bUseAutomationWorker = true;
            bUsesStats = true;
        }

        // Enable Nanite and Lumen
        bWithNanite = true;
        bWithLumen = true;

        // Enable additional features
        bCompileICU = true;
        bCompilePhysX = true;
        bCompileAPEX = true;
        bCompileNvCloth = true;
        bCompileChaos = true;
        bCompileRecast = true;
        bCompileNavmeshSegment = true;
        bCompileOpus = true;
        
        // Enable DLSS and FSR
        bWithDLSS = true;
        bWithFSR = true;

        // Enable ray tracing
        bWithRayTracing = true;

        // Enable enhanced input
        bWithEnhancedInput = true;
    }
}
