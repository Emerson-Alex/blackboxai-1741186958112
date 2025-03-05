#include "SETimelineTestMetrics.h"
#include "SETimelineTestLogger.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "HAL/PlatformTime.h"

FSETimelineTestMetrics& FSETimelineTestMetrics::Get()
{
    static FSETimelineTestMetrics Instance;
    return Instance;
}

FSETimelineTestMetrics::FSETimelineTestMetrics()
    : bInitialized(false)
    , bCollectingMetrics(false)
    , LastFrameTime(0.0f)
    , LastGPUTime(0.0f)
    , LastCPUTime(0.0f)
    , LastMemoryUsage(0)
    , LastDrawCalls(0)
    , LastTriangles(0)
    , TimeSinceLastCollection(0.0f)
{
    // Set default settings
    MetricsSettings.SampleRate = 60.0f;
    MetricsSettings.WindowSize = 60;
    MetricsSettings.WarningThreshold = 0.8f;
    MetricsSettings.ErrorThreshold = 0.95f;
    MetricsSettings.CollectionInterval = 0.016f;
    MetricsSettings.bTrackFrameTime = true;
    MetricsSettings.bTrackGPUTime = true;
    MetricsSettings.bTrackCPUTime = true;
    MetricsSettings.bTrackMemoryUsage = true;
    MetricsSettings.bTrackDrawCalls = true;
    MetricsSettings.bTrackTriangles = true;

    MetricsWindow.WindowSize = MetricsSettings.WindowSize;
    MetricsWindow.SampleInterval = 1.0f / MetricsSettings.SampleRate;
}

void FSETimelineTestMetrics::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    ClearMetrics();
    bInitialized = true;
}

void FSETimelineTestMetrics::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    EndTest();
    ClearMetrics();
    bInitialized = false;
}

void FSETimelineTestMetrics::BeginTest(const FString& TestName)
{
    if (!bInitialized || bCollectingMetrics)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    CurrentTest = TestName;
    TestStartTime = FDateTime::Now();
    bCollectingMetrics = true;
    MetricsWindow.Samples.Empty();
    TimeSinceLastCollection = 0.0f;
}

void FSETimelineTestMetrics::EndTest()
{
    if (!bInitialized || !bCollectingMetrics)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    if (!CurrentTest.IsEmpty())
    {
        TestMetrics.Add(CurrentTest, MetricsWindow.Samples);
    }
    bCollectingMetrics = false;
    CurrentTest.Empty();
}

void FSETimelineTestMetrics::TrackFrameTime(float FrameTime)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackFrameTime)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastFrameTime = FrameTime;
    TimeSinceLastCollection += FrameTime;

    if (TimeSinceLastCollection >= MetricsSettings.CollectionInterval)
    {
        AddSample(CreateSample());
        TimeSinceLastCollection = 0.0f;
    }
}

void FSETimelineTestMetrics::TrackGPUTime(float GPUTime)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackGPUTime)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastGPUTime = GPUTime;
}

void FSETimelineTestMetrics::TrackCPUTime(float CPUTime)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackCPUTime)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastCPUTime = CPUTime;
}

void FSETimelineTestMetrics::TrackMemoryUsage(SIZE_T MemoryUsage)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackMemoryUsage)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastMemoryUsage = MemoryUsage;
}

void FSETimelineTestMetrics::TrackDrawCalls(int32 DrawCalls)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackDrawCalls)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastDrawCalls = DrawCalls;
}

void FSETimelineTestMetrics::TrackTriangles(int32 Triangles)
{
    if (!bInitialized || !bCollectingMetrics || !MetricsSettings.bTrackTriangles)
    {
        return;
    }

    FScopeLock Lock(&MetricsLock);
    LastTriangles = Triangles;
}

SETimelineTest::FPerformanceMetrics FSETimelineTestMetrics::GetPerformanceMetrics() const
{
    FScopeLock Lock(&MetricsLock);
    return CalculateMetrics(MetricsWindow.Samples);
}

void FSETimelineTestMetrics::SetMetricsSettings(const FMetricsSettings& Settings)
{
    FScopeLock Lock(&MetricsLock);
    MetricsSettings = Settings;
    MetricsWindow.WindowSize = Settings.WindowSize;
    MetricsWindow.SampleInterval = 1.0f / Settings.SampleRate;
}

SETimelineTest::FPerformanceMetrics FSETimelineTestMetrics::GetMetricsForTimeRange(
    const FDateTime& Start,
    const FDateTime& End) const
{
    FScopeLock Lock(&MetricsLock);
    TArray<SETimelineTest::FMetricsSample> FilteredSamples;

    for (const auto& Sample : MetricsWindow.Samples)
    {
        if (Sample.Timestamp >= Start && Sample.Timestamp <= End)
        {
            FilteredSamples.Add(Sample);
        }
    }

    return CalculateMetrics(FilteredSamples);
}

SETimelineTest::FPerformanceMetrics FSETimelineTestMetrics::GetMetricsForTest(const FString& TestName) const
{
    FScopeLock Lock(&MetricsLock);
    const TArray<SETimelineTest::FMetricsSample>* TestSamples = TestMetrics.Find(TestName);
    if (TestSamples)
    {
        return CalculateMetrics(*TestSamples);
    }
    return SETimelineTest::FPerformanceMetrics();
}

void FSETimelineTestMetrics::ClearMetrics()
{
    FScopeLock Lock(&MetricsLock);
    MetricsWindow.Samples.Empty();
    TestMetrics.Empty();
    LastFrameTime = 0.0f;
    LastGPUTime = 0.0f;
    LastCPUTime = 0.0f;
    LastMemoryUsage = 0;
    LastDrawCalls = 0;
    LastTriangles = 0;
    TimeSinceLastCollection = 0.0f;
}

void FSETimelineTestMetrics::AddSample(const SETimelineTest::FMetricsSample& Sample)
{
    MetricsWindow.Samples.Add(Sample);
    if (MetricsWindow.Samples.Num() > MetricsWindow.WindowSize)
    {
        MetricsWindow.Samples.RemoveAt(0);
    }
}

SETimelineTest::FMetricsSample FSETimelineTestMetrics::CreateSample() const
{
    SETimelineTest::FMetricsSample Sample;
    Sample.FrameTime = LastFrameTime;
    Sample.GPUTime = LastGPUTime;
    Sample.CPUTime = LastCPUTime;
    Sample.MemoryUsage = LastMemoryUsage;
    Sample.DrawCalls = LastDrawCalls;
    Sample.Triangles = LastTriangles;
    Sample.Timestamp = FDateTime::Now();
    return Sample;
}

SETimelineTest::FPerformanceMetrics FSETimelineTestMetrics::CalculateMetrics(
    const TArray<SETimelineTest::FMetricsSample>& Samples) const
{
    SETimelineTest::FPerformanceMetrics Metrics;
    if (Samples.Num() == 0)
    {
        return Metrics;
    }

    // Calculate averages and peaks
    float TotalFrameTime = 0.0f;
    float TotalGPUTime = 0.0f;
    float TotalCPUTime = 0.0f;
    SIZE_T TotalMemoryUsage = 0;
    int32 TotalDrawCalls = 0;
    int32 TotalTriangles = 0;

    Metrics.MinFrameTime = MAX_FLT;
    Metrics.MaxFrameTime = -MAX_FLT;

    for (const auto& Sample : Samples)
    {
        // Frame time
        TotalFrameTime += Sample.FrameTime;
        Metrics.MinFrameTime = FMath::Min(Metrics.MinFrameTime, Sample.FrameTime);
        Metrics.MaxFrameTime = FMath::Max(Metrics.MaxFrameTime, Sample.FrameTime);

        // GPU time
        TotalGPUTime += Sample.GPUTime;
        Metrics.PeakGPUTime = FMath::Max(Metrics.PeakGPUTime, Sample.GPUTime);

        // CPU time
        TotalCPUTime += Sample.CPUTime;
        Metrics.PeakCPUTime = FMath::Max(Metrics.PeakCPUTime, Sample.CPUTime);

        // Memory usage
        TotalMemoryUsage += Sample.MemoryUsage;
        Metrics.PeakMemoryUsage = FMath::Max(Metrics.PeakMemoryUsage, Sample.MemoryUsage);

        // Draw calls and triangles
        TotalDrawCalls += Sample.DrawCalls;
        TotalTriangles += Sample.Triangles;
    }

    const float NumSamples = static_cast<float>(Samples.Num());
    Metrics.AverageFrameTime = TotalFrameTime / NumSamples;
    Metrics.AverageGPUTime = TotalGPUTime / NumSamples;
    Metrics.AverageCPUTime = TotalCPUTime / NumSamples;
    Metrics.AverageMemoryUsage = static_cast<SIZE_T>(TotalMemoryUsage / Samples.Num());
    Metrics.DrawCalls = static_cast<int32>(TotalDrawCalls / Samples.Num());
    Metrics.Triangles = static_cast<int32>(TotalTriangles / Samples.Num());

    if (TestStartTime != FDateTime())
    {
        Metrics.ExecutionTime = (FDateTime::Now() - TestStartTime).GetTotalSeconds();
    }

    return Metrics;
}

bool FSETimelineTestMetrics::ExportMetricsToCSV(const FString& FilePath) const
{
    FScopeLock Lock(&MetricsLock);
    FString Content = TEXT("Timestamp,FrameTime,GPUTime,CPUTime,MemoryUsage,DrawCalls,Triangles\n");

    for (const auto& Sample : MetricsWindow.Samples)
    {
        Content += FString::Printf(TEXT("%s,%.3f,%.3f,%.3f,%llu,%d,%d\n"),
            *Sample.Timestamp.ToString(),
            Sample.FrameTime,
            Sample.GPUTime,
            Sample.CPUTime,
            Sample.MemoryUsage,
            Sample.DrawCalls,
            Sample.Triangles);
    }

    return FFileHelper::SaveStringToFile(Content, *FilePath);
}

bool FSETimelineTestMetrics::ExportMetricsToJSON(const FString& FilePath) const
{
    FScopeLock Lock(&MetricsLock);
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> SamplesArray;

    for (const auto& Sample : MetricsWindow.Samples)
    {
        TSharedPtr<FJsonObject> SampleObject = MakeShared<FJsonObject>();
        SampleObject->SetStringField(TEXT("Timestamp"), Sample.Timestamp.ToString());
        SampleObject->SetNumberField(TEXT("FrameTime"), Sample.FrameTime);
        SampleObject->SetNumberField(TEXT("GPUTime"), Sample.GPUTime);
        SampleObject->SetNumberField(TEXT("CPUTime"), Sample.CPUTime);
        SampleObject->SetNumberField(TEXT("MemoryUsage"), Sample.MemoryUsage);
        SampleObject->SetNumberField(TEXT("DrawCalls"), Sample.DrawCalls);
        SampleObject->SetNumberField(TEXT("Triangles"), Sample.Triangles);
        SamplesArray.Add(MakeShared<FJsonValueObject>(SampleObject));
    }

    RootObject->SetArrayField(TEXT("Samples"), SamplesArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    return FFileHelper::SaveStringToFile(OutputString, *FilePath);
}
