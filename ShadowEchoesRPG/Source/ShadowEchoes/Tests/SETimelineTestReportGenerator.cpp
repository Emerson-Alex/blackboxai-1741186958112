#include "SETimelineTestReportGenerator.h"
#include "SETimelineTestLogger.h"
#include "SETimelineTestConfig.h"
#include "SETimelineTestReportTemplate.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "ImageUtils.h"

FSETimelineTestReportGenerator& FSETimelineTestReportGenerator::Get()
{
    static FSETimelineTestReportGenerator Instance;
    return Instance;
}

FSETimelineTestReportGenerator::FSETimelineTestReportGenerator()
    : bInitialized(false)
{
    // Set default options
    ReportOptions.bIncludeSystemInfo = true;
    ReportOptions.bIncludeTimestamp = true;
    ReportOptions.bIncludeScreenshots = true;
    ReportOptions.bCompressScreenshots = true;
    ReportOptions.bIncludePerformanceData = true;
    ReportOptions.bIncludeTestLogs = true;
    ReportOptions.Formats = { EReportFormat::HTML, EReportFormat::JSON };
    ReportOptions.OutputPath = TEXT("Saved/Testing/Reports");
    ReportOptions.ReportTitle = TEXT("Timeline Test Report");
    ReportOptions.ReportDescription = TEXT("Test results for Timeline post-process system");
}

void FSETimelineTestReportGenerator::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    // Create output directory
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*ReportOptions.OutputPath))
    {
        PlatformFile.CreateDirectoryTree(*ReportOptions.OutputPath);
    }

    bInitialized = true;
}

void FSETimelineTestReportGenerator::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    bInitialized = false;
}

bool FSETimelineTestReportGenerator::GenerateReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    if (!bInitialized)
    {
        return false;
    }

    bool bSuccess = true;

    for (const auto& Format : ReportOptions.Formats)
    {
        switch (Format)
        {
            case EReportFormat::HTML:
                bSuccess &= GenerateHTMLReport(Results);
                break;
            case EReportFormat::CSV:
                bSuccess &= GenerateCSVReport(Results);
                break;
            case EReportFormat::JSON:
                bSuccess &= GenerateJSONReport(Results);
                break;
            case EReportFormat::XML:
                bSuccess &= GenerateXMLReport(Results);
                break;
            case EReportFormat::PDF:
                bSuccess &= GeneratePDFReport(Results);
                break;
        }
    }

    return bSuccess;
}

void FSETimelineTestReportGenerator::SetReportOptions(const FReportOptions& Options)
{
    ReportOptions = Options;
}

bool FSETimelineTestReportGenerator::GenerateHTMLReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    // Get HTML template
    FString Template = FSETimelineTestReportTemplate::Get().GetHTMLTemplate();

    // Replace template variables
    Template = Template.Replace(TEXT("{{REPORT_TITLE}}"), *ReportOptions.ReportTitle);
    Template = Template.Replace(TEXT("{{REPORT_DESCRIPTION}}"), *ReportOptions.ReportDescription);
    Template = Template.Replace(TEXT("{{CSS_STYLES}}"), *FSETimelineTestReportTemplate::Get().GetCSSStyles());
    Template = Template.Replace(TEXT("{{JAVASCRIPT}}"), *FSETimelineTestReportTemplate::Get().GetJavaScript());

    // Generate sections
    Template = Template.Replace(TEXT("{{HEADER}}"), *GenerateHeaderSection(Results));
    Template = Template.Replace(TEXT("{{SUMMARY}}"), *GenerateSummarySection(Results));
    Template = Template.Replace(TEXT("{{TEST_RESULTS}}"), *GenerateTestResultsSection(Results));
    Template = Template.Replace(TEXT("{{PERFORMANCE_DATA}}"), *GeneratePerformanceSection(Results));
    Template = Template.Replace(TEXT("{{SCREENSHOTS}}"), *GenerateScreenshotsSection(Results));
    Template = Template.Replace(TEXT("{{SYSTEM_INFO}}"), *GenerateSystemInfoSection());
    Template = Template.Replace(TEXT("{{FOOTER}}"), *GenerateFooterSection());

    // Save report
    return SaveReport(Template, TEXT("html"));
}

bool FSETimelineTestReportGenerator::GenerateCSVReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    FString Content = TEXT("Test Name,Status,Duration,Category,Priority,Error Message\n");

    for (const auto& Result : Results.Results)
    {
        Content += FString::Printf(TEXT("%s,%s,%.3f,%s,%s,%s\n"),
            *Result.TestName,
            *FormatStatus(Result.Status),
            Result.Duration,
            *FormatCategory(Result.Category),
            *FormatPriority(Result.Priority),
            *Result.ErrorMessage.Replace(TEXT(","), TEXT(";")));
    }

    return SaveReport(Content, TEXT("csv"));
}

bool FSETimelineTestReportGenerator::GenerateJSONReport(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();

    // Add metadata
    RootObject->SetStringField(TEXT("title"), ReportOptions.ReportTitle);
    RootObject->SetStringField(TEXT("description"), ReportOptions.ReportDescription);
    RootObject->SetStringField(TEXT("timestamp"), *GetTimestampString());

    // Add summary
    RootObject->SetNumberField(TEXT("totalTests"), Results.TotalTests);
    RootObject->SetNumberField(TEXT("passedTests"), Results.PassedTests);
    RootObject->SetNumberField(TEXT("failedTests"), Results.FailedTests);
    RootObject->SetNumberField(TEXT("skippedTests"), Results.SkippedTests);
    RootObject->SetNumberField(TEXT("totalExecutionTime"), Results.TotalExecutionTime);

    // Add test results
    TArray<TSharedPtr<FJsonValue>> TestResults;
    for (const auto& Result : Results.Results)
    {
        TSharedPtr<FJsonObject> TestObject = MakeShared<FJsonObject>();
        TestObject->SetStringField(TEXT("name"), Result.TestName);
        TestObject->SetStringField(TEXT("status"), *FormatStatus(Result.Status));
        TestObject->SetNumberField(TEXT("duration"), Result.Duration);
        TestObject->SetStringField(TEXT("category"), *FormatCategory(Result.Category));
        TestObject->SetStringField(TEXT("priority"), *FormatPriority(Result.Priority));
        TestObject->SetStringField(TEXT("errorMessage"), Result.ErrorMessage);

        TestResults.Add(MakeShared<FJsonValueObject>(TestObject));
    }
    RootObject->SetArrayField(TEXT("results"), TestResults);

    // Convert to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    return SaveReport(OutputString, TEXT("json"));
}

FString FSETimelineTestReportGenerator::GenerateHeaderSection(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    FString Header = FSETimelineTestReportTemplate::Get().GetHeaderTemplate();
    Header = Header.Replace(TEXT("{{REPORT_TITLE}}"), *ReportOptions.ReportTitle);
    Header = Header.Replace(TEXT("{{REPORT_DESCRIPTION}}"), *ReportOptions.ReportDescription);
    Header = Header.Replace(TEXT("{{TIMESTAMP}}"), *GetTimestampString());
    return Header;
}

FString FSETimelineTestReportGenerator::GenerateSummarySection(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    FString Summary = FSETimelineTestReportTemplate::Get().GetSummaryTemplate();
    Summary = Summary.Replace(TEXT("{{TOTAL_TESTS}}"), *FString::FromInt(Results.TotalTests));
    Summary = Summary.Replace(TEXT("{{PASSED_TESTS}}"), *FString::FromInt(Results.PassedTests));
    Summary = Summary.Replace(TEXT("{{FAILED_TESTS}}"), *FString::FromInt(Results.FailedTests));
    Summary = Summary.Replace(TEXT("{{SKIPPED_TESTS}}"), *FString::FromInt(Results.SkippedTests));
    Summary = Summary.Replace(TEXT("{{TOTAL_DURATION}}"), *FormatDuration(Results.TotalExecutionTime));
    return Summary;
}

FString FSETimelineTestReportGenerator::FormatStatus(SETimelineTest::ETestStatus Status)
{
    switch (Status)
    {
        case SETimelineTest::ETestStatus::Passed:   return TEXT("Passed");
        case SETimelineTest::ETestStatus::Failed:   return TEXT("Failed");
        case SETimelineTest::ETestStatus::Skipped:  return TEXT("Skipped");
        case SETimelineTest::ETestStatus::TimedOut: return TEXT("Timed Out");
        default: return TEXT("Unknown");
    }
}

FString FSETimelineTestReportGenerator::FormatCategory(SETimelineTest::ETestCategory Category)
{
    switch (Category)
    {
        case SETimelineTest::ETestCategory::Core:         return TEXT("Core");
        case SETimelineTest::ETestCategory::Visual:       return TEXT("Visual");
        case SETimelineTest::ETestCategory::Performance:  return TEXT("Performance");
        case SETimelineTest::ETestCategory::Integration:  return TEXT("Integration");
        case SETimelineTest::ETestCategory::Stress:       return TEXT("Stress");
        default: return TEXT("Unknown");
    }
}

FString FSETimelineTestReportGenerator::FormatPriority(SETimelineTest::ETestPriority Priority)
{
    switch (Priority)
    {
        case SETimelineTest::ETestPriority::Critical: return TEXT("Critical");
        case SETimelineTest::ETestPriority::High:     return TEXT("High");
        case SETimelineTest::ETestPriority::Medium:   return TEXT("Medium");
        case SETimelineTest::ETestPriority::Low:      return TEXT("Low");
        default: return TEXT("Unknown");
    }
}

FString FSETimelineTestReportGenerator::FormatDuration(float Seconds)
{
    const int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
    const float RemainingSeconds = Seconds - (Minutes * 60.0f);
    return FString::Printf(TEXT("%d:%05.2f"), Minutes, RemainingSeconds);
}

FString FSETimelineTestReportGenerator::GetTimestampString()
{
    return FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

bool FSETimelineTestReportGenerator::SaveReport(const FString& Content, const FString& Extension)
{
    const FString FilePath = GetReportPath(Extension);
    return FFileHelper::SaveStringToFile(Content, *FilePath);
}

FString FSETimelineTestReportGenerator::GetReportPath(const FString& Extension)
{
    const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    return FPaths::Combine(ReportOptions.OutputPath, 
        FString::Printf(TEXT("TimelineTest_%s.%s"), *Timestamp, *Extension));
}

// Implement other methods similarly...
