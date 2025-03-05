#pragma once

#include "CoreMinimal.h"

/**
 * Template for Timeline test reports
 */
class SHADOWECHOES_API FSETimelineTestReportTemplate
{
public:
    /** Get template instance */
    static FSETimelineTestReportTemplate& Get();

    /** Get HTML template */
    FString GetHTMLTemplate() const;

    /** Get CSS styles */
    FString GetCSSStyles() const;

    /** Get JavaScript */
    FString GetJavaScript() const;

    /** Get header template */
    FString GetHeaderTemplate() const;

    /** Get summary template */
    FString GetSummaryTemplate() const;

    /** Get test results template */
    FString GetTestResultsTemplate() const;

    /** Get performance template */
    FString GetPerformanceTemplate() const;

    /** Get screenshots template */
    FString GetScreenshotsTemplate() const;

    /** Get system info template */
    FString GetSystemInfoTemplate() const;

    /** Get footer template */
    FString GetFooterTemplate() const;

    /** Get chart templates */
    FString GetTestResultsChartTemplate() const;
    FString GetPerformanceChartTemplate() const;
    FString GetTestDurationChartTemplate() const;
    FString GetMemoryUsageChartTemplate() const;

    /** Get chart options */
    FString GetChartOptions() const;

    /** Get test result row template */
    FString GetTestResultRowTemplate() const;

    /** Get test details row template */
    FString GetTestDetailsRowTemplate() const;

    /** Get screenshot modal template */
    FString GetScreenshotModalTemplate() const;

    /** Get performance metrics row template */
    FString GetPerformanceMetricsRowTemplate() const;

    /** Get system info row template */
    FString GetSystemInfoRowTemplate() const;

private:
    /** Constructor */
    FSETimelineTestReportTemplate();

    /** HTML template */
    static const TCHAR* HTMLTemplate;

    /** CSS styles */
    static const TCHAR* CSSStyles;

    /** JavaScript */
    static const TCHAR* JavaScript;

    /** Section templates */
    static const TCHAR* HeaderTemplate;
    static const TCHAR* SummaryTemplate;
    static const TCHAR* TestResultsTemplate;
    static const TCHAR* PerformanceTemplate;
    static const TCHAR* ScreenshotsTemplate;
    static const TCHAR* SystemInfoTemplate;
    static const TCHAR* FooterTemplate;

    /** Chart templates */
    static const TCHAR* TestResultsChartTemplate;
    static const TCHAR* PerformanceChartTemplate;
    static const TCHAR* TestDurationChartTemplate;
    static const TCHAR* MemoryUsageChartTemplate;

    /** Chart options */
    static const TCHAR* ChartOptions;

    /** Component templates */
    static const TCHAR* TestResultRowTemplate;
    static const TCHAR* TestDetailsRowTemplate;
    static const TCHAR* ScreenshotModalTemplate;
    static const TCHAR* PerformanceMetricsRowTemplate;
    static const TCHAR* SystemInfoRowTemplate;

    /** Helper functions */
    FString LoadTemplate(const TCHAR* Template) const;
    FString ReplaceVariable(const FString& Template, const TCHAR* Variable, const FString& Value) const;
    FString FormatChartData(const TArray<FString>& Labels, const TArray<float>& Values) const;
    FString FormatChartOptions(const FString& Title, const FString& Type, const FString& Color) const;

    /** Prevent copying */
    FSETimelineTestReportTemplate(const FSETimelineTestReportTemplate&) = delete;
    FSETimelineTestReportTemplate& operator=(const FSETimelineTestReportTemplate&) = delete;
};
