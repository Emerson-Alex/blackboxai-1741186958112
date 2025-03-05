#include "SETimelineTestConfig.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

FSETimelineTestConfig& FSETimelineTestConfig::Get()
{
    static FSETimelineTestConfig Instance;
    return Instance;
}

FSETimelineTestConfig::FSETimelineTestConfig()
    : bInitialized(false)
{
}

void FSETimelineTestConfig::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    LoadConfig();
    bInitialized = true;
}

void FSETimelineTestConfig::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    SaveConfig();
    bInitialized = false;
}

bool FSETimelineTestConfig::LoadConfig()
{
    const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("TimelineTest.ini");
    if (!GConfig->LoadFile(ConfigPath))
    {
        return false;
    }

    LoadCoreSettings();
    LoadRunnerSettings();
    LoadPerformanceSettings();
    LoadQualitySettings();
    LoadTransitionSettings();
    LoadColorSettings();
    LoadLoggingSettings();
    LoadAutomationSettings();
    LoadDevelopmentSettings();
    LoadTestCategorySettings();
    LoadTestPrioritySettings();
    LoadTestRequirementSettings();
    LoadTestEnvironmentSettings();
    LoadTestScenarioSettings();
    LoadTestMetricsSettings();
    LoadTestReportingSettings();
    LoadTestUISettings();

    return true;
}

bool FSETimelineTestConfig::SaveConfig()
{
    const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("TimelineTest.ini");

    SaveCoreSettings();
    SaveRunnerSettings();
    SavePerformanceSettings();
    SaveQualitySettings();
    SaveTransitionSettings();
    SaveColorSettings();
    SaveLoggingSettings();
    SaveAutomationSettings();
    SaveDevelopmentSettings();
    SaveTestCategorySettings();
    SaveTestPrioritySettings();
    SaveTestRequirementSettings();
    SaveTestEnvironmentSettings();
    SaveTestScenarioSettings();
    SaveTestMetricsSettings();
    SaveTestReportingSettings();
    SaveTestUISettings();

    return GConfig->SaveFile(ConfigPath);
}

void FSETimelineTestConfig::LoadCoreSettings()
{
    GConfig->GetBool(TEXT("Core"), TEXT("bEnableTestFeatures"), CoreSettings.bEnableTestFeatures, GConfigPath);
    GConfig->GetBool(TEXT("Core"), TEXT("bEnablePerformanceTests"), CoreSettings.bEnablePerformanceTests, GConfigPath);
    GConfig->GetBool(TEXT("Core"), TEXT("bEnableVisualTests"), CoreSettings.bEnableVisualTests, GConfigPath);
    GConfig->GetBool(TEXT("Core"), TEXT("bEnableAutomation"), CoreSettings.bEnableAutomation, GConfigPath);
}

void FSETimelineTestConfig::LoadRunnerSettings()
{
    FString WindowSize;
    if (GConfig->GetString(TEXT("TestRunner"), TEXT("DefaultWindowSize"), WindowSize, GConfigPath))
    {
        FString X, Y;
        if (WindowSize.Split(TEXT(","), &X, &Y))
        {
            RunnerSettings.DefaultWindowSize.X = FCString::Atoi(*X);
            RunnerSettings.DefaultWindowSize.Y = FCString::Atoi(*Y);
        }
    }

    GConfig->GetBool(TEXT("TestRunner"), TEXT("bRememberWindowPosition"), RunnerSettings.bRememberWindowPosition, GConfigPath);
    GConfig->GetBool(TEXT("TestRunner"), TEXT("bRememberWindowSize"), RunnerSettings.bRememberWindowSize, GConfigPath);
    GConfig->GetFloat(TEXT("TestRunner"), TEXT("DefaultTimeoutSeconds"), RunnerSettings.DefaultTimeoutSeconds, GConfigPath);
    GConfig->GetInt(TEXT("TestRunner"), TEXT("MaxParallelTests"), RunnerSettings.MaxParallelTests, GConfigPath);
    GConfig->GetBool(TEXT("TestRunner"), TEXT("bCaptureScreenshots"), RunnerSettings.bCaptureScreenshots, GConfigPath);
    GConfig->GetBool(TEXT("TestRunner"), TEXT("bGenerateReports"), RunnerSettings.bGenerateReports, GConfigPath);
    GConfig->GetString(TEXT("TestRunner"), TEXT("ReportOutputPath"), RunnerSettings.ReportOutputPath, GConfigPath);
    GConfig->GetString(TEXT("TestRunner"), TEXT("ScreenshotOutputPath"), RunnerSettings.ScreenshotOutputPath, GConfigPath);
}

void FSETimelineTestConfig::LoadPerformanceSettings()
{
    GConfig->GetFloat(TEXT("Performance"), TEXT("FrameTimeWarningThreshold"), PerformanceSettings.FrameTimeWarningThreshold, GConfigPath);
    GConfig->GetFloat(TEXT("Performance"), TEXT("FrameTimeErrorThreshold"), PerformanceSettings.FrameTimeErrorThreshold, GConfigPath);
    GConfig->GetFloat(TEXT("Performance"), TEXT("GPUTimeWarningThreshold"), PerformanceSettings.GPUTimeWarningThreshold, GConfigPath);
    GConfig->GetFloat(TEXT("Performance"), TEXT("GPUTimeErrorThreshold"), PerformanceSettings.GPUTimeErrorThreshold, GConfigPath);
    GConfig->GetFloat(TEXT("Performance"), TEXT("CPUTimeWarningThreshold"), PerformanceSettings.CPUTimeWarningThreshold, GConfigPath);
    GConfig->GetFloat(TEXT("Performance"), TEXT("CPUTimeErrorThreshold"), PerformanceSettings.CPUTimeErrorThreshold, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("MemoryWarningThresholdMB"), PerformanceSettings.MemoryWarningThresholdMB, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("MemoryErrorThresholdMB"), PerformanceSettings.MemoryErrorThresholdMB, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("DrawCallWarningThreshold"), PerformanceSettings.DrawCallWarningThreshold, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("DrawCallErrorThreshold"), PerformanceSettings.DrawCallErrorThreshold, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("TriangleWarningThreshold"), PerformanceSettings.TriangleWarningThreshold, GConfigPath);
    GConfig->GetInt(TEXT("Performance"), TEXT("TriangleErrorThreshold"), PerformanceSettings.TriangleErrorThreshold, GConfigPath);
}

void FSETimelineTestConfig::SaveCoreSettings()
{
    GConfig->SetBool(TEXT("Core"), TEXT("bEnableTestFeatures"), CoreSettings.bEnableTestFeatures, GConfigPath);
    GConfig->SetBool(TEXT("Core"), TEXT("bEnablePerformanceTests"), CoreSettings.bEnablePerformanceTests, GConfigPath);
    GConfig->SetBool(TEXT("Core"), TEXT("bEnableVisualTests"), CoreSettings.bEnableVisualTests, GConfigPath);
    GConfig->SetBool(TEXT("Core"), TEXT("bEnableAutomation"), CoreSettings.bEnableAutomation, GConfigPath);
}

void FSETimelineTestConfig::SaveRunnerSettings()
{
    const FString WindowSize = FString::Printf(TEXT("%d,%d"), 
        RunnerSettings.DefaultWindowSize.X, 
        RunnerSettings.DefaultWindowSize.Y);
    GConfig->SetString(TEXT("TestRunner"), TEXT("DefaultWindowSize"), *WindowSize, GConfigPath);

    GConfig->SetBool(TEXT("TestRunner"), TEXT("bRememberWindowPosition"), RunnerSettings.bRememberWindowPosition, GConfigPath);
    GConfig->SetBool(TEXT("TestRunner"), TEXT("bRememberWindowSize"), RunnerSettings.bRememberWindowSize, GConfigPath);
    GConfig->SetFloat(TEXT("TestRunner"), TEXT("DefaultTimeoutSeconds"), RunnerSettings.DefaultTimeoutSeconds, GConfigPath);
    GConfig->SetInt(TEXT("TestRunner"), TEXT("MaxParallelTests"), RunnerSettings.MaxParallelTests, GConfigPath);
    GConfig->SetBool(TEXT("TestRunner"), TEXT("bCaptureScreenshots"), RunnerSettings.bCaptureScreenshots, GConfigPath);
    GConfig->SetBool(TEXT("TestRunner"), TEXT("bGenerateReports"), RunnerSettings.bGenerateReports, GConfigPath);
    GConfig->SetString(TEXT("TestRunner"), TEXT("ReportOutputPath"), *RunnerSettings.ReportOutputPath, GConfigPath);
    GConfig->SetString(TEXT("TestRunner"), TEXT("ScreenshotOutputPath"), *RunnerSettings.ScreenshotOutputPath, GConfigPath);
}

void FSETimelineTestConfig::SavePerformanceSettings()
{
    GConfig->SetFloat(TEXT("Performance"), TEXT("FrameTimeWarningThreshold"), PerformanceSettings.FrameTimeWarningThreshold, GConfigPath);
    GConfig->SetFloat(TEXT("Performance"), TEXT("FrameTimeErrorThreshold"), PerformanceSettings.FrameTimeErrorThreshold, GConfigPath);
    GConfig->SetFloat(TEXT("Performance"), TEXT("GPUTimeWarningThreshold"), PerformanceSettings.GPUTimeWarningThreshold, GConfigPath);
    GConfig->SetFloat(TEXT("Performance"), TEXT("GPUTimeErrorThreshold"), PerformanceSettings.GPUTimeErrorThreshold, GConfigPath);
    GConfig->SetFloat(TEXT("Performance"), TEXT("CPUTimeWarningThreshold"), PerformanceSettings.CPUTimeWarningThreshold, GConfigPath);
    GConfig->SetFloat(TEXT("Performance"), TEXT("CPUTimeErrorThreshold"), PerformanceSettings.CPUTimeErrorThreshold, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("MemoryWarningThresholdMB"), PerformanceSettings.MemoryWarningThresholdMB, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("MemoryErrorThresholdMB"), PerformanceSettings.MemoryErrorThresholdMB, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("DrawCallWarningThreshold"), PerformanceSettings.DrawCallWarningThreshold, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("DrawCallErrorThreshold"), PerformanceSettings.DrawCallErrorThreshold, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("TriangleWarningThreshold"), PerformanceSettings.TriangleWarningThreshold, GConfigPath);
    GConfig->SetInt(TEXT("Performance"), TEXT("TriangleErrorThreshold"), PerformanceSettings.TriangleErrorThreshold, GConfigPath);
}

// Implement other Load/Save functions similarly...
