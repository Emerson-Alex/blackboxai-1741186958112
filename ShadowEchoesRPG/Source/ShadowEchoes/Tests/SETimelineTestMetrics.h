#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "HAL/CriticalSection.h"

/**
 * Performance metrics tracker for Timeline tests
 */
class SHADOWECHOES_API FSETimelineTestMetrics
{
public:
    /** Get metrics instance */
    static FSETimelineTestMetrics& Get();

    /** Initialize metrics */
    void Initialize();

    /** Shutdown metrics */
    void Shutdown();

    /** Begin test metrics collection */
    void BeginTest(const FString& TestName);

    /** End test metrics collection */
    void EndTest();

    /** Track frame time */
    void TrackFrameTime(float FrameTime);

    /** Track GPU time */
    void TrackGPUTime(float GPUTime);

    /** Track CPU time */
    void TrackCPUTime(float CPUTime);

    /** Track memory usage */
    void TrackMemoryUsage(SIZE_T MemoryUsage);

    /** Track draw calls */
    void TrackDrawCalls(int32 DrawCalls);

    /** Track triangles */
    void TrackTriangles(int32 Triangles);

    /** Get performance metrics */
    SETimelineTest::FPerformanceMetrics GetPerformanceMetrics() const;

    /** Get metrics window */
    const SETimelineTest::FMetricsWindow& GetMetricsWindow() const { return MetricsWindow; }

    /** Get metrics settings */
    struct FMetricsSettings
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
    const FMetricsSettings& GetMetricsSettings() const { return MetricsSettings; }

    /** Set metrics settings */
    void SetMetricsSettings(const FMetricsSettings& Settings);

    /** Get metrics for time range */
    SETimelineTest::FPerformanceMetrics GetMetricsForTimeRange(const FDateTime& Start, const FDateTime& End) const;

    /** Get metrics for test */
    SETimelineTest::FPerformanceMetrics GetMetricsForTest(const FString& TestName) const;

    /** Clear metrics */
    void ClearMetrics();

    /** Export metrics to CSV */
    bool ExportMetricsToCSV(const FString& FilePath) const;

    /** Export metrics to JSON */
    bool ExportMetricsToJSON(const FString& FilePath) const;

private:
    /** Constructor */
    FSETimelineTestMetrics();

    /** Add metrics sample */
    void AddSample(const SETimelineTest::FMetricsSample& Sample);

    /** Calculate metrics */
    SETimelineTest::FPerformanceMetrics CalculateMetrics(const TArray<SETimelineTest::FMetricsSample>& Samples) const;

    /** Format metrics for export */
    FString FormatMetricsForCSV(const TArray<SETimelineTest::FMetricsSample>& Samples) const;
    TSharedPtr<FJsonObject> FormatMetricsForJSON(const TArray<SETimelineTest::FMetricsSample>& Samples) const;

    /** State */
    bool bInitialized;
    bool bCollectingMetrics;
    FString CurrentTest;
    FDateTime TestStartTime;
    FMetricsSettings MetricsSettings;
    SETimelineTest::FMetricsWindow MetricsWindow;
    TMap<FString, TArray<SETimelineTest::FMetricsSample>> TestMetrics;
    mutable FCriticalSection MetricsLock;

    /** Performance tracking */
    float LastFrameTime;
    float LastGPUTime;
    float LastCPUTime;
    SIZE_T LastMemoryUsage;
    int32 LastDrawCalls;
    int32 LastTriangles;
    float TimeSinceLastCollection;

    /** Prevent copying */
    FSETimelineTestMetrics(const FSETimelineTestMetrics&) = delete;
    FSETimelineTestMetrics& operator=(const FSETimelineTestMetrics&) = delete;
};

/** Metrics macros */
#define SE_TEST_TRACK_FRAME_TIME(Time) \
    FSETimelineTestMetrics::Get().TrackFrameTime(Time)

#define SE_TEST_TRACK_GPU_TIME(Time) \
    FSETimelineTestMetrics::Get().TrackGPUTime(Time)

#define SE_TEST_TRACK_CPU_TIME(Time) \
    FSETimelineTestMetrics::Get().TrackCPUTime(Time)

#define SE_TEST_TRACK_MEMORY_USAGE(Usage) \
    FSETimelineTestMetrics::Get().TrackMemoryUsage(Usage)

#define SE_TEST_TRACK_DRAW_CALLS(Calls) \
    FSETimelineTestMetrics::Get().TrackDrawCalls(Calls)

#define SE_TEST_TRACK_TRIANGLES(Count) \
    FSETimelineTestMetrics::Get().TrackTriangles(Count)

#define SE_TEST_BEGIN_METRICS(TestName) \
    FSETimelineTestMetrics::Get().BeginTest(TestName)

#define SE_TEST_END_METRICS() \
    FSETimelineTestMetrics::Get().EndTest()

#define SE_TEST_GET_METRICS() \
    FSETimelineTestMetrics::Get().GetPerformanceMetrics()
