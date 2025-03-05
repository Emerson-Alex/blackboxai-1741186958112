#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "HAL/CriticalSection.h"

/**
 * Logger for Timeline test execution and results
 */
class SHADOWECHOES_API FSETimelineTestLogger
{
public:
    /** Get logger instance */
    static FSETimelineTestLogger& Get();

    /** Initialize logger */
    void Initialize();

    /** Shutdown logger */
    void Shutdown();

    /** Log levels */
    enum class ELogLevel
    {
        Info,
        Warning,
        Error,
        Debug,
        Verbose
    };

    /** Log info message */
    void LogInfo(const FString& TestName, const FString& Message);

    /** Log warning message */
    void LogWarning(const FString& TestName, const FString& Message);

    /** Log error message */
    void LogError(const FString& TestName, const FString& Message);

    /** Log debug message */
    void LogDebug(const FString& TestName, const FString& Message);

    /** Log test result */
    void LogTestResult(const SETimelineTest::FTestResult& Result);

    /** Log test suite result */
    void LogTestSuiteResult(const SETimelineTest::FTestSuiteResult& Result);

    /** Log performance metrics */
    void LogPerformanceMetrics(const FString& TestName, const SETimelineTest::FPerformanceMetrics& Metrics);

    /** Log system info */
    void LogSystemInfo(const SETimelineTest::FTestEnvironment& Environment);

    /** Get log entries */
    TArray<SETimelineTest::FTestLogEntry> GetLogEntries() const;

    /** Get log entries for test */
    TArray<SETimelineTest::FTestLogEntry> GetLogEntriesForTest(const FString& TestName) const;

    /** Get log entries by level */
    TArray<SETimelineTest::FTestLogEntry> GetLogEntriesByLevel(SETimelineTest::FTestLogEntry::ELogLevel Level) const;

    /** Get log entries by category */
    TArray<SETimelineTest::FTestLogEntry> GetLogEntriesByCategory(const FString& Category) const;

    /** Get log entries in time range */
    TArray<SETimelineTest::FTestLogEntry> GetLogEntriesInTimeRange(const FDateTime& Start, const FDateTime& End) const;

    /** Clear log entries */
    void ClearLogEntries();

    /** Export logs to file */
    bool ExportLogsToFile(const FString& FilePath) const;

    /** Export logs to CSV */
    bool ExportLogsToCSV(const FString& FilePath) const;

    /** Export logs to JSON */
    bool ExportLogsToJSON(const FString& FilePath) const;

    /** Get log settings */
    struct FLogSettings
    {
        bool bLogToFile;
        bool bLogToConsole;
        bool bLogTimestamp;
        bool bLogTestName;
        bool bLogCategory;
        bool bLogStackTrace;
        ELogLevel MinLogLevel;
        FString LogFilePath;
        int32 MaxLogFiles;
        int32 MaxLogSizeMB;
    };
    const FLogSettings& GetLogSettings() const { return LogSettings; }

    /** Set log settings */
    void SetLogSettings(const FLogSettings& Settings);

private:
    /** Constructor */
    FSETimelineTestLogger();

    /** Log entry */
    void LogEntry(const SETimelineTest::FTestLogEntry& Entry);

    /** Format log entry */
    FString FormatLogEntry(const SETimelineTest::FTestLogEntry& Entry) const;

    /** Write to log file */
    void WriteToLogFile(const FString& Message);

    /** Write to console */
    void WriteToConsole(const FString& Message);

    /** Rotate log files */
    void RotateLogFiles();

    /** Get log level color */
    FString GetLogLevelColor(SETimelineTest::FTestLogEntry::ELogLevel Level) const;

    /** Get log level string */
    FString GetLogLevelString(SETimelineTest::FTestLogEntry::ELogLevel Level) const;

    /** Format timestamp */
    FString FormatTimestamp(const FDateTime& Timestamp) const;

    /** Format stack trace */
    FString FormatStackTrace() const;

    /** State */
    bool bInitialized;
    FLogSettings LogSettings;
    TArray<SETimelineTest::FTestLogEntry> LogEntries;
    mutable FCriticalSection LogLock;
    FString CurrentLogFile;
    int32 CurrentLogSize;

    /** Prevent copying */
    FSETimelineTestLogger(const FSETimelineTestLogger&) = delete;
    FSETimelineTestLogger& operator=(const FSETimelineTestLogger&) = delete;
};

/** Log macros */
#define SE_TEST_LOG_INFO(TestName, Message) \
    FSETimelineTestLogger::Get().LogInfo(TestName, Message)

#define SE_TEST_LOG_WARNING(TestName, Message) \
    FSETimelineTestLogger::Get().LogWarning(TestName, Message)

#define SE_TEST_LOG_ERROR(TestName, Message) \
    FSETimelineTestLogger::Get().LogError(TestName, Message)

#define SE_TEST_LOG_DEBUG(TestName, Message) \
    FSETimelineTestLogger::Get().LogDebug(TestName, Message)

#define SE_TEST_LOG_RESULT(Result) \
    FSETimelineTestLogger::Get().LogTestResult(Result)

#define SE_TEST_LOG_SUITE_RESULT(Result) \
    FSETimelineTestLogger::Get().LogTestSuiteResult(Result)

#define SE_TEST_LOG_METRICS(TestName, Metrics) \
    FSETimelineTestLogger::Get().LogPerformanceMetrics(TestName, Metrics)

#define SE_TEST_LOG_SYSTEM_INFO(Environment) \
    FSETimelineTestLogger::Get().LogSystemInfo(Environment)
