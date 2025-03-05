#include "SETimelineTestLogger.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

FSETimelineTestLogger& FSETimelineTestLogger::Get()
{
    static FSETimelineTestLogger Instance;
    return Instance;
}

FSETimelineTestLogger::FSETimelineTestLogger()
    : bInitialized(false)
    , CurrentLogSize(0)
{
    // Set default settings
    LogSettings.bLogToFile = true;
    LogSettings.bLogToConsole = true;
    LogSettings.bLogTimestamp = true;
    LogSettings.bLogTestName = true;
    LogSettings.bLogCategory = true;
    LogSettings.bLogStackTrace = false;
    LogSettings.MinLogLevel = ELogLevel::Info;
    LogSettings.LogFilePath = TEXT("Saved/Testing/Logs");
    LogSettings.MaxLogFiles = 10;
    LogSettings.MaxLogSizeMB = 100;
}

void FSETimelineTestLogger::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    // Create log directory if it doesn't exist
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*LogSettings.LogFilePath))
    {
        PlatformFile.CreateDirectoryTree(*LogSettings.LogFilePath);
    }

    // Set current log file
    const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    CurrentLogFile = FPaths::Combine(LogSettings.LogFilePath, FString::Printf(TEXT("TimelineTest_%s.log"), *Timestamp));

    bInitialized = true;
}

void FSETimelineTestLogger::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    // Export logs if needed
    if (LogSettings.bLogToFile)
    {
        ExportLogsToFile(CurrentLogFile);
    }

    ClearLogEntries();
    bInitialized = false;
}

void FSETimelineTestLogger::LogInfo(const FString& TestName, const FString& Message)
{
    SETimelineTest::FTestLogEntry Entry;
    Entry.Message = Message;
    Entry.Level = SETimelineTest::FTestLogEntry::ELogLevel::Info;
    Entry.TestName = TestName;
    LogEntry(Entry);
}

void FSETimelineTestLogger::LogWarning(const FString& TestName, const FString& Message)
{
    SETimelineTest::FTestLogEntry Entry;
    Entry.Message = Message;
    Entry.Level = SETimelineTest::FTestLogEntry::ELogLevel::Warning;
    Entry.TestName = TestName;
    LogEntry(Entry);
}

void FSETimelineTestLogger::LogError(const FString& TestName, const FString& Message)
{
    SETimelineTest::FTestLogEntry Entry;
    Entry.Message = Message;
    Entry.Level = SETimelineTest::FTestLogEntry::ELogLevel::Error;
    Entry.TestName = TestName;
    LogEntry(Entry);
}

void FSETimelineTestLogger::LogDebug(const FString& TestName, const FString& Message)
{
    SETimelineTest::FTestLogEntry Entry;
    Entry.Message = Message;
    Entry.Level = SETimelineTest::FTestLogEntry::ELogLevel::Debug;
    Entry.TestName = TestName;
    LogEntry(Entry);
}

void FSETimelineTestLogger::LogTestResult(const SETimelineTest::FTestResult& Result)
{
    const FString StatusText = [&Result]()
    {
        switch (Result.Status)
        {
            case SETimelineTest::ETestStatus::Passed:   return TEXT("PASSED");
            case SETimelineTest::ETestStatus::Failed:   return TEXT("FAILED");
            case SETimelineTest::ETestStatus::Skipped:  return TEXT("SKIPPED");
            case SETimelineTest::ETestStatus::TimedOut: return TEXT("TIMED OUT");
            default: return TEXT("UNKNOWN");
        }
    }();

    const FString Message = FString::Printf(TEXT("Test %s: %s (Duration: %.3fs)"),
        *Result.TestName, *StatusText, Result.Duration);

    if (Result.Status == SETimelineTest::ETestStatus::Failed)
    {
        LogError(Result.TestName, Message);
        if (!Result.ErrorMessage.IsEmpty())
        {
            LogError(Result.TestName, Result.ErrorMessage);
        }
    }
    else
    {
        LogInfo(Result.TestName, Message);
    }
}

void FSETimelineTestLogger::LogTestSuiteResult(const SETimelineTest::FTestSuiteResult& Result)
{
    const FString Message = FString::Printf(TEXT("Test Suite %s completed: %d total, %d passed, %d failed, %d skipped (Duration: %.3fs)"),
        *Result.SuiteName,
        Result.TotalTests,
        Result.PassedTests,
        Result.FailedTests,
        Result.SkippedTests,
        Result.TotalDuration);

    LogInfo(Result.SuiteName, Message);
}

void FSETimelineTestLogger::LogPerformanceMetrics(const FString& TestName, const SETimelineTest::FPerformanceMetrics& Metrics)
{
    const FString Message = FString::Printf(TEXT("Performance Metrics:\n")
        TEXT("  Frame Time: %.3f ms (min: %.3f ms, max: %.3f ms)\n")
        TEXT("  GPU Time: %.3f ms (peak: %.3f ms)\n")
        TEXT("  CPU Time: %.3f ms (peak: %.3f ms)\n")
        TEXT("  Memory Usage: %llu MB (peak: %llu MB)\n")
        TEXT("  Draw Calls: %d\n")
        TEXT("  Triangles: %d"),
        Metrics.AverageFrameTime,
        Metrics.MinFrameTime,
        Metrics.MaxFrameTime,
        Metrics.AverageGPUTime,
        Metrics.PeakGPUTime,
        Metrics.AverageCPUTime,
        Metrics.PeakCPUTime,
        Metrics.AverageMemoryUsage / (1024 * 1024),
        Metrics.PeakMemoryUsage / (1024 * 1024),
        Metrics.DrawCalls,
        Metrics.Triangles);

    LogInfo(TestName, Message);
}

void FSETimelineTestLogger::LogEntry(const SETimelineTest::FTestLogEntry& Entry)
{
    if (static_cast<int32>(Entry.Level) < static_cast<int32>(LogSettings.MinLogLevel))
    {
        return;
    }

    FScopeLock Lock(&LogLock);

    // Add to log entries
    LogEntries.Add(Entry);

    // Format log message
    const FString FormattedMessage = FormatLogEntry(Entry);

    // Write to console if enabled
    if (LogSettings.bLogToConsole)
    {
        WriteToConsole(FormattedMessage);
    }

    // Write to file if enabled
    if (LogSettings.bLogToFile)
    {
        WriteToLogFile(FormattedMessage);
    }
}

FString FSETimelineTestLogger::FormatLogEntry(const SETimelineTest::FTestLogEntry& Entry) const
{
    FString Message;

    // Add timestamp
    if (LogSettings.bLogTimestamp)
    {
        Message += FString::Printf(TEXT("[%s] "), *FormatTimestamp(Entry.Timestamp));
    }

    // Add log level
    Message += FString::Printf(TEXT("[%s] "), *GetLogLevelString(Entry.Level));

    // Add test name
    if (LogSettings.bLogTestName && !Entry.TestName.IsEmpty())
    {
        Message += FString::Printf(TEXT("[%s] "), *Entry.TestName);
    }

    // Add category
    if (LogSettings.bLogCategory && !Entry.Category.IsEmpty())
    {
        Message += FString::Printf(TEXT("[%s] "), *Entry.Category);
    }

    // Add message
    Message += Entry.Message;

    // Add stack trace
    if (LogSettings.bLogStackTrace)
    {
        Message += TEXT("\n") + FormatStackTrace();
    }

    return Message;
}

void FSETimelineTestLogger::WriteToLogFile(const FString& Message)
{
    if (!bInitialized || CurrentLogFile.IsEmpty())
    {
        return;
    }

    // Check if we need to rotate logs
    const int64 NewSize = CurrentLogSize + Message.Len() * sizeof(TCHAR);
    if (NewSize > LogSettings.MaxLogSizeMB * 1024 * 1024)
    {
        RotateLogFiles();
    }

    // Append to log file
    FFileHelper::SaveStringToFile(Message + TEXT("\n"), *CurrentLogFile, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), FILEWRITE_Append);
    CurrentLogSize = NewSize;
}

void FSETimelineTestLogger::WriteToConsole(const FString& Message)
{
#if !NO_LOGGING
    const FString ColoredMessage = GetLogLevelColor(LogSettings.MinLogLevel) + Message + TEXT("\033[0m");
    UE_LOG(LogTemp, Log, TEXT("%s"), *ColoredMessage);
#endif
}

void FSETimelineTestLogger::RotateLogFiles()
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    // Delete oldest log file if we've reached max files
    TArray<FString> LogFiles;
    IFileManager::Get().FindFiles(LogFiles, *FPaths::Combine(LogSettings.LogFilePath, TEXT("*.log")), true, false);
    if (LogFiles.Num() >= LogSettings.MaxLogFiles)
    {
        FDateTime OldestTime = FDateTime::MaxValue();
        FString OldestFile;

        for (const FString& LogFile : LogFiles)
        {
            const FDateTime FileTime = PlatformFile.GetTimeStamp(*LogFile);
            if (FileTime < OldestTime)
            {
                OldestTime = FileTime;
                OldestFile = LogFile;
            }
        }

        if (!OldestFile.IsEmpty())
        {
            PlatformFile.DeleteFile(*OldestFile);
        }
    }

    // Create new log file
    const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    CurrentLogFile = FPaths::Combine(LogSettings.LogFilePath, FString::Printf(TEXT("TimelineTest_%s.log"), *Timestamp));
    CurrentLogSize = 0;
}

FString FSETimelineTestLogger::GetLogLevelColor(SETimelineTest::FTestLogEntry::ELogLevel Level) const
{
    switch (Level)
    {
        case SETimelineTest::FTestLogEntry::ELogLevel::Info:    return TEXT("\033[32m"); // Green
        case SETimelineTest::FTestLogEntry::ELogLevel::Warning: return TEXT("\033[33m"); // Yellow
        case SETimelineTest::FTestLogEntry::ELogLevel::Error:   return TEXT("\033[31m"); // Red
        case SETimelineTest::FTestLogEntry::ELogLevel::Debug:   return TEXT("\033[36m"); // Cyan
        default: return TEXT("\033[0m");  // Reset
    }
}

FString FSETimelineTestLogger::GetLogLevelString(SETimelineTest::FTestLogEntry::ELogLevel Level) const
{
    switch (Level)
    {
        case SETimelineTest::FTestLogEntry::ELogLevel::Info:    return TEXT("INFO");
        case SETimelineTest::FTestLogEntry::ELogLevel::Warning: return TEXT("WARNING");
        case SETimelineTest::FTestLogEntry::ELogLevel::Error:   return TEXT("ERROR");
        case SETimelineTest::FTestLogEntry::ELogLevel::Debug:   return TEXT("DEBUG");
        default: return TEXT("UNKNOWN");
    }
}

FString FSETimelineTestLogger::FormatTimestamp(const FDateTime& Timestamp) const
{
    return Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S.%s"));
}

void FSETimelineTestLogger::SetLogSettings(const FLogSettings& Settings)
{
    FScopeLock Lock(&LogLock);
    LogSettings = Settings;
}

TArray<SETimelineTest::FTestLogEntry> FSETimelineTestLogger::GetLogEntries() const
{
    FScopeLock Lock(&LogLock);
    return LogEntries;
}

void FSETimelineTestLogger::ClearLogEntries()
{
    FScopeLock Lock(&LogLock);
    LogEntries.Empty();
}

bool FSETimelineTestLogger::ExportLogsToFile(const FString& FilePath) const
{
    FScopeLock Lock(&LogLock);

    FString Content;
    for (const auto& Entry : LogEntries)
    {
        Content += FormatLogEntry(Entry) + TEXT("\n");
    }

    return FFileHelper::SaveStringToFile(Content, *FilePath);
}
