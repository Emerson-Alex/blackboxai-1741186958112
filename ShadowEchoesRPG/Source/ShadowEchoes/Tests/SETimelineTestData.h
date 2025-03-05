#pragma once

#include "CoreMinimal.h"

namespace SETimelineTest
{
    /** Test status */
    enum class ETestStatus
    {
        None,
        Running,
        Passed,
        Failed,
        Skipped,
        TimedOut
    };

    /** Test category */
    enum class ETestCategory
    {
        Core,
        Visual,
        Performance,
        Integration,
        Stress
    };

    /** Test priority */
    enum class ETestPriority
    {
        Critical,
        High,
        Medium,
        Low
    };

    /** Test log entry */
    struct FTestLogEntry
    {
        /** Log levels */
        enum class ELogLevel
        {
            Info,
            Warning,
            Error,
            Debug,
            Verbose
        };

        FString Message;
        ELogLevel Level;
        FString TestName;
        FString Category;
        FDateTime Timestamp;
        FString StackTrace;
    };

    /** Test metrics sample */
    struct FMetricsSample
    {
        float FrameTime;
        float GPUTime;
        float CPUTime;
        SIZE_T MemoryUsage;
        int32 DrawCalls;
        int32 Triangles;
        FDateTime Timestamp;
    };

    /** Test metrics window */
    struct FMetricsWindow
    {
        TArray<FMetricsSample> Samples;
        int32 WindowSize;
        float SampleInterval;
    };

    /** Performance metrics */
    struct FPerformanceMetrics
    {
        float AverageFrameTime;
        float MinFrameTime;
        float MaxFrameTime;
        float AverageGPUTime;
        float PeakGPUTime;
        float AverageCPUTime;
        float PeakCPUTime;
        SIZE_T AverageMemoryUsage;
        SIZE_T PeakMemoryUsage;
        int32 DrawCalls;
        int32 Triangles;
        float ExecutionTime;

        FPerformanceMetrics()
            : AverageFrameTime(0.0f)
            , MinFrameTime(MAX_FLT)
            , MaxFrameTime(-MAX_FLT)
            , AverageGPUTime(0.0f)
            , PeakGPUTime(0.0f)
            , AverageCPUTime(0.0f)
            , PeakCPUTime(0.0f)
            , AverageMemoryUsage(0)
            , PeakMemoryUsage(0)
            , DrawCalls(0)
            , Triangles(0)
            , ExecutionTime(0.0f)
        {}
    };

    /** Test requirements */
    struct FTestRequirements
    {
        int32 MinimumMemoryMB;
        int32 MinimumCPUCores;
        int32 MinimumGPUMemoryMB;
        bool bRequiresVisuals;
        float MinimumFrameRate;

        FTestRequirements()
            : MinimumMemoryMB(512)
            , MinimumCPUCores(2)
            , MinimumGPUMemoryMB(1024)
            , bRequiresVisuals(true)
            , MinimumFrameRate(30.0f)
        {}
    };

    /** Test config */
    struct FTestConfig
    {
        FString TestName;
        FString Description;
        ETestCategory Category;
        ETestPriority Priority;
        FTestRequirements Requirements;
        TMap<FString, FString> Parameters;
        TMap<FString, FString> Metadata;
        bool bEnabled;

        FTestConfig()
            : Category(ETestCategory::Core)
            , Priority(ETestPriority::Medium)
            , bEnabled(true)
        {}
    };

    /** Test result */
    struct FTestResult
    {
        FString TestName;
        ETestStatus Status;
        float Duration;
        FString ErrorMessage;
        ETestCategory Category;
        ETestPriority Priority;
        FDateTime Timestamp;
        FPerformanceMetrics Performance;
        TArray<FString> Screenshots;
        TArray<FString> Logs;

        FTestResult()
            : Status(ETestStatus::None)
            , Duration(0.0f)
            , Category(ETestCategory::Core)
            , Priority(ETestPriority::Medium)
            , Timestamp(FDateTime::Now())
        {}
    };

    /** Test suite result */
    struct FTestSuiteResult
    {
        FString SuiteName;
        int32 TotalTests;
        int32 PassedTests;
        int32 FailedTests;
        int32 SkippedTests;
        float TotalDuration;
        FDateTime StartTime;
        FDateTime EndTime;
        TArray<FTestResult> Results;

        FTestSuiteResult()
            : TotalTests(0)
            , PassedTests(0)
            , FailedTests(0)
            , SkippedTests(0)
            , TotalDuration(0.0f)
            , StartTime(FDateTime::Now())
            , EndTime(FDateTime::Now())
        {}
    };

    /** Test execution options */
    struct FTestExecutionOptions
    {
        bool bStopOnFailure;
        int32 RetryCount;
        float TimeoutSeconds;
        bool bCaptureScreenshots;
        bool bGenerateReports;
        bool bCleanupOnCompletion;
        bool bParallelExecution;
        int32 MaxParallelTests;
        FString ReportPath;
        FString ScreenshotPath;

        FTestExecutionOptions()
            : bStopOnFailure(false)
            , RetryCount(3)
            , TimeoutSeconds(30.0f)
            , bCaptureScreenshots(true)
            , bGenerateReports(true)
            , bCleanupOnCompletion(true)
            , bParallelExecution(true)
            , MaxParallelTests(4)
            , ReportPath(TEXT("Saved/Testing/Reports"))
            , ScreenshotPath(TEXT("Saved/Testing/Screenshots"))
        {}
    };

    /** Test environment */
    struct FTestEnvironment
    {
        FString OperatingSystem;
        FString CPUModel;
        int32 CPUCores;
        SIZE_T TotalMemory;
        FString GPUModel;
        SIZE_T GPUMemory;
        FString EngineVersion;
        FString BuildConfiguration;
        FString RenderAPI;
        FString AudioAPI;
        FString InputAPI;
        FString NetworkAPI;
        FString PhysicsAPI;
        FString ScriptingAPI;
        FString AssetAPI;
        FString FileSystemAPI;
        FString ThreadingAPI;
        FString TimingAPI;
        FString MathAPI;
        FString RandomAPI;
        FString SecurityAPI;
        FString EncryptionAPI;
        FString CompressionAPI;
        FString ValidationAPI;
        FString VerificationAPI;
        FString CleanupAPI;
    };
};
