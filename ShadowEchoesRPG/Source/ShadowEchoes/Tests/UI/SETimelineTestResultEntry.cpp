#include "SETimelineTestResultEntry.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
#include "Styling/SlateTypes.h"
#include "Materials/MaterialInstanceDynamic.h"

void USETimelineTestResultEntry::UpdateTestLogs()
{
    if (LogsBox)
    {
        LogsBox->ClearChildren();

        for (const FString& Log : TestResult.Logs)
        {
            AddLogEntry(Log, TEXT("Info"));
        }
    }
}

void USETimelineTestResultEntry::OnExpandButtonClicked()
{
    ToggleExpanded();
}

void USETimelineTestResultEntry::OnScreenshotClicked(const FString& ScreenshotPath)
{
    // Create a modal dialog to display the screenshot
    UImage* ScreenshotImage = NewObject<UImage>(this);
    UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ScreenshotPath));
    if (Texture)
    {
        ScreenshotImage->SetBrushFromTexture(Texture);
    }
}

FString USETimelineTestResultEntry::FormatDuration(float Seconds) const
{
    const int32 Minutes = FMath::FloorToInt(Seconds / 60.0f);
    const float RemainingSeconds = Seconds - (Minutes * 60.0f);
    return FString::Printf(TEXT("%d:%05.2f"), Minutes, RemainingSeconds);
}

FString USETimelineTestResultEntry::FormatTimestamp(const FDateTime& Timestamp) const
{
    return Timestamp.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
}

FString USETimelineTestResultEntry::FormatMetrics(const SETimelineTest::FPerformanceMetrics& Metrics) const
{
    return FString::Printf(TEXT("Frame Time: %.2f ms\nGPU Time: %.2f ms\nCPU Time: %.2f ms\nMemory: %d MB\nDraw Calls: %d\nTriangles: %d"),
        Metrics.AverageFrameTime,
        Metrics.AverageGPUTime,
        Metrics.AverageCPUTime,
        Metrics.AverageMemoryUsage / (1024 * 1024),
        Metrics.DrawCalls,
        Metrics.Triangles);
}

FString USETimelineTestResultEntry::FormatStatus(SETimelineTest::ETestStatus Status) const
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

FString USETimelineTestResultEntry::FormatCategory(SETimelineTest::ETestCategory Category) const
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

FString USETimelineTestResultEntry::FormatPriority(SETimelineTest::ETestPriority Priority) const
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

FLinearColor USETimelineTestResultEntry::GetStatusColor(SETimelineTest::ETestStatus Status) const
{
    return Style->GetStatusColor(Status);
}

FLinearColor USETimelineTestResultEntry::GetCategoryColor(SETimelineTest::ETestCategory Category) const
{
    switch (Category)
    {
        case SETimelineTest::ETestCategory::Core:         return Style->PrimaryColor;
        case SETimelineTest::ETestCategory::Visual:       return Style->InfoColor;
        case SETimelineTest::ETestCategory::Performance:  return Style->WarningColor;
        case SETimelineTest::ETestCategory::Integration:  return Style->SuccessColor;
        case SETimelineTest::ETestCategory::Stress:       return Style->ErrorColor;
        default: return Style->SecondaryColor;
    }
}

FLinearColor USETimelineTestResultEntry::GetPriorityColor(SETimelineTest::ETestPriority Priority) const
{
    switch (Priority)
    {
        case SETimelineTest::ETestPriority::Critical: return Style->ErrorColor;
        case SETimelineTest::ETestPriority::High:     return Style->WarningColor;
        case SETimelineTest::ETestPriority::Medium:   return Style->InfoColor;
        case SETimelineTest::ETestPriority::Low:      return Style->SecondaryColor;
        default: return Style->SecondaryColor;
    }
}

void USETimelineTestResultEntry::UpdateVisibility()
{
    if (DetailsBox)
    {
        DetailsBox->SetVisibility(bExpanded ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void USETimelineTestResultEntry::UpdateColors()
{
    if (StatusText)
    {
        StatusText->SetColorAndOpacity(FSlateColor(GetStatusColor(TestResult.Status)));
    }

    if (CategoryText)
    {
        CategoryText->SetColorAndOpacity(FSlateColor(GetCategoryColor(TestResult.Category)));
    }

    if (PriorityText)
    {
        PriorityText->SetColorAndOpacity(FSlateColor(GetPriorityColor(TestResult.Priority)));
    }

    if (BackgroundBorder)
    {
        FLinearColor Color = GetStatusColor(TestResult.Status);
        Color.A = 0.1f;
        BackgroundBorder->SetBrushColor(Color);
    }
}

void USETimelineTestResultEntry::UpdateFonts()
{
    if (TestNameText)
    {
        TestNameText->SetFont(Style->BodyFont);
    }

    if (StatusText)
    {
        StatusText->SetFont(Style->BodyFont);
    }

    if (CategoryText)
    {
        CategoryText->SetFont(Style->SmallFont);
    }

    if (PriorityText)
    {
        PriorityText->SetFont(Style->SmallFont);
    }

    if (TimestampText)
    {
        TimestampText->SetFont(Style->SmallFont);
    }

    if (ErrorMessageText)
    {
        ErrorMessageText->SetFont(Style->MonoFont);
    }
}

void USETimelineTestResultEntry::UpdateLayout()
{
    if (BackgroundBorder)
    {
        BackgroundBorder->SetPadding(Style->DefaultPadding);
    }

    if (DetailsBox)
    {
        DetailsBox->SetPadding(Style->ContentPadding);
    }

    if (PerformanceBox)
    {
        PerformanceBox->SetPadding(Style->SmallPadding);
    }

    if (ScreenshotsBox)
    {
        ScreenshotsBox->SetPadding(Style->SmallPadding);
    }

    if (LogsBox)
    {
        LogsBox->SetPadding(Style->SmallPadding);
    }
}

void USETimelineTestResultEntry::UpdateAnimations()
{
    // Add animations for expanding/collapsing details
    if (DetailsBox)
    {
        if (bExpanded)
        {
            // Play expand animation
            PlayAnimation(GetAnimation(TEXT("ExpandDetails")));
        }
        else
        {
            // Play collapse animation
            PlayAnimation(GetAnimation(TEXT("CollapseDetails")));
        }
    }
}

void USETimelineTestResultEntry::AddPerformanceMetric(const FString& Label, const FString& Value)
{
    UHorizontalBox* MetricRow = NewObject<UHorizontalBox>(this);
    
    UTextBlock* LabelText = NewObject<UTextBlock>(this);
    LabelText->SetText(FText::FromString(Label + TEXT(": ")));
    LabelText->SetFont(Style->SmallFont);
    MetricRow->AddChild(LabelText);

    UTextBlock* ValueText = NewObject<UTextBlock>(this);
    ValueText->SetText(FText::FromString(Value));
    ValueText->SetFont(Style->MonoFont);
    MetricRow->AddChild(ValueText);

    PerformanceBox->AddChild(MetricRow);
}

void USETimelineTestResultEntry::AddScreenshot(const FString& Path)
{
    UImage* Screenshot = NewObject<UImage>(this);
    UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *Path));
    if (Texture)
    {
        Screenshot->SetBrushFromTexture(Texture);
        Screenshot->SetDesiredSizeOverride(FVector2D(200.0f, 150.0f));
        
        UButton* ScreenshotButton = NewObject<UButton>(this);
        ScreenshotButton->AddChild(Screenshot);
        ScreenshotButton->OnClicked.AddDynamic(this, &USETimelineTestResultEntry::OnScreenshotClicked);
        
        ScreenshotsBox->AddChild(ScreenshotButton);
    }
}

void USETimelineTestResultEntry::AddLogEntry(const FString& Message, const FString& Level)
{
    UTextBlock* LogText = NewObject<UTextBlock>(this);
    LogText->SetText(FText::FromString(Message));
    LogText->SetFont(Style->MonoFont);
    
    // Set color based on log level
    if (Level.Equals(TEXT("Error")))
    {
        LogText->SetColorAndOpacity(FSlateColor(Style->ErrorColor));
    }
    else if (Level.Equals(TEXT("Warning")))
    {
        LogText->SetColorAndOpacity(FSlateColor(Style->WarningColor));
    }
    else
    {
        LogText->SetColorAndOpacity(FSlateColor(Style->InfoColor));
    }
    
    LogsBox->AddChild(LogText);
}

void USETimelineTestResultEntry::UpdateExpandButton()
{
    if (ExpandButton)
    {
        // Update button icon based on expanded state
        UImage* ButtonIcon = Cast<UImage>(ExpandButton->GetChildAt(0));
        if (ButtonIcon)
        {
            FString IconPath = bExpanded ? TEXT("Texture2D'/Game/UI/Icons/CollapseIcon'") : TEXT("Texture2D'/Game/UI/Icons/ExpandIcon'");
            UTexture2D* IconTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *IconPath));
            if (IconTexture)
            {
                ButtonIcon->SetBrushFromTexture(IconTexture);
            }
        }
    }
}
