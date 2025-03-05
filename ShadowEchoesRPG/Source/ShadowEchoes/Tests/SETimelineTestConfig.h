#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"

/**
 * Configuration manager for Timeline test system
 */
class SHADOWECHOES_API FSETimelineTestConfig
{
public:
    /** Get config instance */
    static FSETimelineTestConfig& Get();

    /** Initialize config */
    void Initialize();

    /** Shutdown config */
    void Shutdown();

    /** Load config */
    bool LoadConfig();

    /** Save config */
    bool SaveConfig();

    /** Core settings */
    struct FCoreSettings
    {
        bool bEnableTestFeatures;
        bool bEnablePerformanceTests;
        bool bEnableVisualTests;
        bool bEnableAutomation;
    };

    /** Runner settings */
    struct FRunnerSettings
    {
        FIntPoint DefaultWindowSize;
        bool bRememberWindowPosition;
        bool bRememberWindowSize;
        float DefaultTimeoutSeconds;
        int32 MaxParallelTests;
        bool bCaptureScreenshots;
        bool bGenerateReports;
        FString ReportOutputPath;
        FString ScreenshotOutputPath;
    };

    /** Performance settings */
    struct FPerformanceSettings
    {
        float FrameTimeWarningThreshold;
        float FrameTimeErrorThreshold;
        float GPUTimeWarningThreshold;
        float GPUTimeErrorThreshold;
        float CPUTimeWarningThreshold;
        float CPUTimeErrorThreshold;
        int32 MemoryWarningThresholdMB;
        int32 MemoryErrorThresholdMB;
        int32 DrawCallWarningThreshold;
        int32 DrawCallErrorThreshold;
        int32 TriangleWarningThreshold;
        int32 TriangleErrorThreshold;
    };

    /** Quality settings */
    struct FQualitySettings
    {
        float ScreenshotComparisonThreshold;
        float MinimumFPS;
        float TargetFPS;
        float MaximumFPS;
        bool bEnableVSync;
        bool bEnableFrameRateLimiting;
    };

    /** Transition settings */
    struct FTransitionSettings
    {
        float DefaultDuration;
        FString DefaultBlendMode;
        float DefaultIntensity;
        bool bEnablePostProcessing;
        bool bEnableBloom;
        bool bEnableDepthOfField;
        bool bEnableMotionBlur;
        bool bEnableTemporalAA;
    };

    /** Color settings */
    struct FColorSettings
    {
        FString PrimaryColor;
        FString SecondaryColor;
        FString SuccessColor;
        FString WarningColor;
        FString ErrorColor;
        FString InfoColor;
    };

    /** Logging settings */
    struct FLoggingSettings
    {
        bool bLogToFile;
        bool bLogToConsole;
        bool bLogTimestamp;
        bool bLogTestName;
        bool bLogCategory;
        bool bLogStackTrace;
        FString MinLogLevel;
        FString LogFilePath;
        int32 MaxLogFiles;
        int32 MaxLogSizeMB;
    };

    /** Automation settings */
    struct FAutomationSettings
    {
        bool bStopOnFailure;
        int32 RetryCount;
        bool bCaptureScreenshots;
        bool bGenerateReports;
        bool bCleanupOnCompletion;
        bool bParallelExecution;
        int32 MaxParallelTests;
    };

    /** Development settings */
    struct FDevelopmentSettings
    {
        bool bEnableHotReload;
        bool bEnableDebugInfo;
        bool bEnableProfiler;
        bool bEnableMemoryTracker;
        bool bEnableCrashReporting;
        bool bEnableShaderDebugging;
    };

    /** Test category settings */
    struct FTestCategorySettings
    {
        bool bEnableCore;
        bool bEnableVisual;
        bool bEnablePerformance;
        bool bEnableIntegration;
        bool bEnableStress;
    };

    /** Test priority settings */
    struct FTestPrioritySettings
    {
        bool bEnableCritical;
        bool bEnableHigh;
        bool bEnableMedium;
        bool bEnableLow;
    };

    /** Test requirement settings */
    struct FTestRequirementSettings
    {
        int32 MinimumMemoryMB;
        int32 MinimumCPUCores;
        int32 MinimumGPUMemoryMB;
        bool bRequiresVisuals;
        float MinimumFrameRate;
    };

    /** Test environment settings */
    struct FTestEnvironmentSettings
    {
        bool bUseTestWorld;
        bool bUseTestGameMode;
        bool bUseTestPlayerController;
        bool bUseTestPawn;
        bool bUseTestCamera;
    };

    /** Test scenario settings */
    struct FTestScenarioSettings
    {
        bool bEnableBasicTests;
        bool bEnableVisualTests;
        bool bEnablePerformanceTests;
        bool bEnableIntegrationTests;
        bool bEnableStressTests;
    };

    /** Test metrics settings */
    struct FTestMetricsSettings
    {
        float SampleRate;
        int32 WindowSize;
        float WarningThreshold;
        float ErrorThreshold;
        float CollectionInterval;
        bool bTrackFrameTime;
        bool bTrackGPUTime;
        bool bTrackCPUTime;
        bool bTrackMemoryUsage;
        bool bTrackDrawCalls;
        bool bTrackTriangles;
    };

    /** Test reporting settings */
    struct FTestReportingSettings
    {
        bool bGenerateHTML;
        bool bGenerateCSV;
        bool bGenerateJSON;
        bool bGenerateXML;
        bool bGeneratePDF;
        bool bIncludeSystemInfo;
        bool bIncludeTimestamp;
        bool bIncludeScreenshots;
        bool bCompressScreenshots;
        bool bIncludePerformanceData;
        bool bIncludeTestLogs;
    };

    /** Test UI settings */
    struct FTestUISettings
    {
        FString Theme;
        FString FontFamily;
        int32 FontSize;
        float LineHeight;
        int32 Padding;
        int32 Margin;
        int32 BorderRadius;
        float TransitionDuration;
        float AnimationDuration;
        float TooltipDelay;
    };

    /** Get settings */
    const FCoreSettings& GetCoreSettings() const { return CoreSettings; }
    const FRunnerSettings& GetRunnerSettings() const { return RunnerSettings; }
    const FPerformanceSettings& GetPerformanceSettings() const { return PerformanceSettings; }
    const FQualitySettings& GetQualitySettings() const { return QualitySettings; }
    const FTransitionSettings& GetTransitionSettings() const { return TransitionSettings; }
    const FColorSettings& GetColorSettings() const { return ColorSettings; }
    const FLoggingSettings& GetLoggingSettings() const { return LoggingSettings; }
    const FAutomationSettings& GetAutomationSettings() const { return AutomationSettings; }
    const FDevelopmentSettings& GetDevelopmentSettings() const { return DevelopmentSettings; }
    const FTestCategorySettings& GetTestCategorySettings() const { return TestCategorySettings; }
    const FTestPrioritySettings& GetTestPrioritySettings() const { return TestPrioritySettings; }
    const FTestRequirementSettings& GetTestRequirementSettings() const { return TestRequirementSettings; }
    const FTestEnvironmentSettings& GetTestEnvironmentSettings() const { return TestEnvironmentSettings; }
    const FTestScenarioSettings& GetTestScenarioSettings() const { return TestScenarioSettings; }
    const FTestMetricsSettings& GetTestMetricsSettings() const { return TestMetricsSettings; }
    const FTestReportingSettings& GetTestReportingSettings() const { return TestReportingSettings; }
    const FTestUISettings& GetTestUISettings() const { return TestUISettings; }

private:
    /** Constructor */
    FSETimelineTestConfig();

    /** Settings */
    FCoreSettings CoreSettings;
    FRunnerSettings RunnerSettings;
    FPerformanceSettings PerformanceSettings;
    FQualitySettings QualitySettings;
    FTransitionSettings TransitionSettings;
    FColorSettings ColorSettings;
    FLoggingSettings LoggingSettings;
    FAutomationSettings AutomationSettings;
    FDevelopmentSettings DevelopmentSettings;
    FTestCategorySettings TestCategorySettings;
    FTestPrioritySettings TestPrioritySettings;
    FTestRequirementSettings TestRequirementSettings;
    FTestEnvironmentSettings TestEnvironmentSettings;
    FTestScenarioSettings TestScenarioSettings;
    FTestMetricsSettings TestMetricsSettings;
    FTestReportingSettings TestReportingSettings;
    FTestUISettings TestUISettings;

    /** State */
    bool bInitialized;

    /** Load settings from config file */
    void LoadCoreSettings();
    void LoadRunnerSettings();
    void LoadPerformanceSettings();
    void LoadQualitySettings();
    void LoadTransitionSettings();
    void LoadColorSettings();
    void LoadLoggingSettings();
    void LoadAutomationSettings();
    void LoadDevelopmentSettings();
    void LoadTestCategorySettings();
    void LoadTestPrioritySettings();
    void LoadTestRequirementSettings();
    void LoadTestEnvironmentSettings();
    void LoadTestScenarioSettings();
    void LoadTestMetricsSettings();
    void LoadTestReportingSettings();
    void LoadTestUISettings();

    /** Save settings to config file */
    void SaveCoreSettings();
    void SaveRunnerSettings();
    void SavePerformanceSettings();
    void SaveQualitySettings();
    void SaveTransitionSettings();
    void SaveColorSettings();
    void SaveLoggingSettings();
    void SaveAutomationSettings();
    void SaveDevelopmentSettings();
    void SaveTestCategorySettings();
    void SaveTestPrioritySettings();
    void SaveTestRequirementSettings();
    void SaveTestEnvironmentSettings();
    void SaveTestScenarioSettings();
    void SaveTestMetricsSettings();
    void SaveTestReportingSettings();
    void SaveTestUISettings();

    /** Prevent copying */
    FSETimelineTestConfig(const FSETimelineTestConfig&) = delete;
    FSETimelineTestConfig& operator=(const FSETimelineTestConfig&) = delete;
};
