#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "SETimelinePostProcessTestExecutor.h"

/**
 * Generator for Timeline test reports
 */
class SHADOWECHOES_API FSETimelineTestReportGenerator
{
public:
    /** Get generator instance */
    static FSETimelineTestReportGenerator& Get();

    /** Initialize generator */
    void Initialize();

    /** Shutdown generator */
    void Shutdown();

    /** Generate report */
    bool GenerateReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Report formats */
    enum class EReportFormat
    {
        HTML,
        CSV,
        JSON,
        XML,
        PDF
    };

    /** Report options */
    struct FReportOptions
    {
        bool bIncludeSystemInfo;
        bool bIncludeTimestamp;
        bool bIncludeScreenshots;
        bool bCompressScreenshots;
        bool bIncludePerformanceData;
        bool bIncludeTestLogs;
        TArray<EReportFormat> Formats;
        FString OutputPath;
        FString ReportTitle;
        FString ReportDescription;
    };

    /** Set report options */
    void SetReportOptions(const FReportOptions& Options);

    /** Get report options */
    const FReportOptions& GetReportOptions() const { return ReportOptions; }

private:
    /** Constructor */
    FSETimelineTestReportGenerator();

    /** Generate HTML report */
    bool GenerateHTMLReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Generate CSV report */
    bool GenerateCSVReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Generate JSON report */
    bool GenerateJSONReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Generate XML report */
    bool GenerateXMLReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Generate PDF report */
    bool GeneratePDFReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Format test results */
    FString FormatTestResults(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Format performance data */
    FString FormatPerformanceData(const SETimelineTest::FPerformanceMetrics& Metrics);

    /** Format system info */
    FString FormatSystemInfo();

    /** Format screenshots */
    FString FormatScreenshots(const TArray<FString>& Screenshots);

    /** Format test logs */
    FString FormatTestLogs(const TArray<FString>& Logs);

    /** Format timestamp */
    FString FormatTimestamp(const FDateTime& Timestamp);

    /** Format duration */
    FString FormatDuration(float Seconds);

    /** Format status */
    FString FormatStatus(SETimelineTest::ETestStatus Status);

    /** Format category */
    FString FormatCategory(SETimelineTest::ETestCategory Category);

    /** Format priority */
    FString FormatPriority(SETimelineTest::ETestPriority Priority);

    /** Format metrics */
    FString FormatMetrics(const SETimelineTest::FPerformanceMetrics& Metrics);

    /** Format requirements */
    FString FormatRequirements(const SETimelineTest::FTestRequirements& Requirements);

    /** Format parameters */
    FString FormatParameters(const TMap<FString, FString>& Parameters);

    /** Format metadata */
    FString FormatMetadata(const TMap<FString, FString>& Metadata);

    /** Generate charts */
    FString GenerateTestResultsChart(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GeneratePerformanceChart(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateTestDurationChart(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateMemoryUsageChart(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** Generate sections */
    FString GenerateHeaderSection(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateSummarySection(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateTestResultsSection(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GeneratePerformanceSection(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateScreenshotsSection(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GenerateSystemInfoSection();
    FString GenerateFooterSection();

    /** Helper functions */
    bool SaveReport(const FString& Content, const FString& Extension);
    bool SaveScreenshot(const FString& ScreenshotPath, const FString& OutputPath);
    bool CompressScreenshot(const FString& ScreenshotPath, const FString& OutputPath);
    FString GetReportPath(const FString& Extension);
    FString GetScreenshotPath(const FString& ScreenshotName);
    FString GetTimestampString();
    FString GetSystemInfoString();
    FString GetTestSummaryString(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    FString GetPerformanceSummaryString(const FSETimelinePostProcessTestExecutor::FTestResults& Results);

    /** State */
    bool bInitialized;
    FReportOptions ReportOptions;

    /** Prevent copying */
    FSETimelineTestReportGenerator(const FSETimelineTestReportGenerator&) = delete;
    FSETimelineTestReportGenerator& operator=(const FSETimelineTestReportGenerator&) = delete;
};
