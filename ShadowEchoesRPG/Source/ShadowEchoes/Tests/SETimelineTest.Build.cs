using UnrealBuildTool;

public class SETimelineTest : ModuleRules
{
    public SETimelineTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "RenderCore",
            "RHI",
            "Slate",
            "SlateCore",
            "UMG",
            "Json",
            "JsonUtilities",
            "HTTP",
            "ImageWrapper",
            "Projects",
            "ShadowEchoes"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "RenderCore",
            "ShaderCore",
            "EngineSettings",
            "UnrealEd",
            "AssetRegistry",
            "DesktopPlatform",
            "AutomationController",
            "AutomationWorker",
            "AutomationWindow",
            "ScreenShotComparisonTools",
            "ImageCore"
        });

        PublicIncludePaths.AddRange(new string[] {
            "ShadowEchoes/Public",
            "ShadowEchoes/Public/Tests",
            "ShadowEchoes/Public/Components",
            "ShadowEchoes/Public/Systems"
        });

        PrivateIncludePaths.AddRange(new string[] {
            "ShadowEchoes/Private",
            "ShadowEchoes/Private/Tests",
            "ShadowEchoes/Private/Components",
            "ShadowEchoes/Private/Systems"
        });

        // Enable test automation
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_AUTOMATION_TESTS=1");
            PublicDefinitions.Add("WITH_DEV_AUTOMATION_TESTS=1");
            PublicDefinitions.Add("WITH_PERF_AUTOMATION_TESTS=1");
        }

        // Enable debug features
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_EDITOR=1");
            PublicDefinitions.Add("WITH_EDITORONLY_DATA=1");
            PublicDefinitions.Add("WITH_ENGINE=1");
            PublicDefinitions.Add("WITH_UNREAL_DEVELOPER_TOOLS=1");
        }

        // Enable performance profiling
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("ENABLE_VISUAL_LOG=1");
            PublicDefinitions.Add("ALLOW_CONSOLE=1");
            PublicDefinitions.Add("STATS=1");
            PublicDefinitions.Add("WITH_PROFILEGPU=1");
        }

        // Enable shader debugging
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_SHADER_DEBUG_INFO=1");
            PublicDefinitions.Add("WITH_SHADER_SOURCE_INFO=1");
        }

        // Enable memory tracking
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("UE_MEMORY_TAGS_TRACE_ENABLED=1");
            PublicDefinitions.Add("UE_TRACE_ENABLED=1");
        }

        // Enable crash reporting
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_CRASH_REPORTING=1");
            PublicDefinitions.Add("WITH_CRASH_GEN=1");
        }

        // Enable logging
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("UE_BUILD_DEVELOPMENT=1");
            PublicDefinitions.Add("UE_BUILD_DEBUG=1");
        }

        // Enable optimization settings
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        bUseUnity = false;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        // Enable hot reload
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            bEnableHotReload = true;
            bDisableUnverifiedCertificates = false;
            bAllowHotReloadFromIDE = true;
        }

        // Enable debug info
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            bDisableDebugInfo = false;
            bOmitPCDebugInfoInDevelopment = false;
            bUsePDBFiles = true;
        }

        // Enable additional features
        bEnableExceptions = true;
        bEnableUndefinedIdentifierWarnings = true;
        bTreatAsEngineModule = false;
        bEnforceIWYU = true;
    }
}
