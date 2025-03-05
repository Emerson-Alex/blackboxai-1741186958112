#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SETimelineTestData.h"
#include "SETimelineTestStyle.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "SETimelineTestResultEntry.generated.h"

/**
 * Widget for displaying a single test result entry
 */
UCLASS()
class SHADOWECHOES_API USETimelineTestResultEntry : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Set test result data */
    void SetTestResult(const SETimelineTest::FTestResult& InResult);

    /** Get test result data */
    const SETimelineTest::FTestResult& GetTestResult() const { return TestResult; }

    /** Set expanded state */
    void SetExpanded(bool bInExpanded);

    /** Get expanded state */
    bool IsExpanded() const { return bExpanded; }

    /** Toggle expanded state */
    void ToggleExpanded();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Create widget bindings */
    void CreateBindings();

    /** Update widget appearance */
    void UpdateAppearance();

    /** Update test status */
    void UpdateTestStatus();

    /** Update test details */
    void UpdateTestDetails();

    /** Update performance metrics */
    void UpdatePerformanceMetrics();

    /** Update screenshots */
    void UpdateScreenshots();

    /** Update test logs */
    void UpdateTestLogs();

    /** Handle expand button clicked */
    UFUNCTION()
    void OnExpandButtonClicked();

    /** Handle screenshot clicked */
    UFUNCTION()
    void OnScreenshotClicked(const FString& ScreenshotPath);

    /** Format duration */
    FString FormatDuration(float Seconds) const;

    /** Format timestamp */
    FString FormatTimestamp(const FDateTime& Timestamp) const;

    /** Format metrics */
    FString FormatMetrics(const SETimelineTest::FPerformanceMetrics& Metrics) const;

    /** Format status */
    FString FormatStatus(SETimelineTest::ETestStatus Status) const;

    /** Format category */
    FString FormatCategory(SETimelineTest::ETestCategory Category) const;

    /** Format priority */
    FString FormatPriority(SETimelineTest::ETestPriority Priority) const;

    /** Get status color */
    FLinearColor GetStatusColor(SETimelineTest::ETestStatus Status) const;

    /** Get category color */
    FLinearColor GetCategoryColor(SETimelineTest::ETestCategory Category) const;

    /** Get priority color */
    FLinearColor GetPriorityColor(SETimelineTest::ETestPriority Priority) const;

private:
    /** Test result data */
    SETimelineTest::FTestResult TestResult;

    /** Expanded state */
    bool bExpanded;

    /** Widget references */
    UPROPERTY()
    UTextBlock* TestNameText;

    UPROPERTY()
    UTextBlock* StatusText;

    UPROPERTY()
    UTextBlock* DurationText;

    UPROPERTY()
    UTextBlock* CategoryText;

    UPROPERTY()
    UTextBlock* PriorityText;

    UPROPERTY()
    UTextBlock* TimestampText;

    UPROPERTY()
    UTextBlock* ErrorMessageText;

    UPROPERTY()
    UVerticalBox* DetailsBox;

    UPROPERTY()
    UVerticalBox* PerformanceBox;

    UPROPERTY()
    UVerticalBox* ScreenshotsBox;

    UPROPERTY()
    UVerticalBox* LogsBox;

    UPROPERTY()
    UButton* ExpandButton;

    UPROPERTY()
    UImage* StatusIcon;

    UPROPERTY()
    UBorder* BackgroundBorder;

    /** Style */
    FSETimelineTestStyle* Style;

    /** Create widgets */
    void CreateWidgets();

    /** Create test name text */
    UTextBlock* CreateTestNameText();

    /** Create status text */
    UTextBlock* CreateStatusText();

    /** Create duration text */
    UTextBlock* CreateDurationText();

    /** Create category text */
    UTextBlock* CreateCategoryText();

    /** Create priority text */
    UTextBlock* CreatePriorityText();

    /** Create timestamp text */
    UTextBlock* CreateTimestampText();

    /** Create error message text */
    UTextBlock* CreateErrorMessageText();

    /** Create details box */
    UVerticalBox* CreateDetailsBox();

    /** Create performance box */
    UVerticalBox* CreatePerformanceBox();

    /** Create screenshots box */
    UVerticalBox* CreateScreenshotsBox();

    /** Create logs box */
    UVerticalBox* CreateLogsBox();

    /** Create expand button */
    UButton* CreateExpandButton();

    /** Create status icon */
    UImage* CreateStatusIcon();

    /** Create background border */
    UBorder* CreateBackgroundBorder();

    /** Add performance metric */
    void AddPerformanceMetric(const FString& Label, const FString& Value);

    /** Add screenshot */
    void AddScreenshot(const FString& Path);

    /** Add log entry */
    void AddLogEntry(const FString& Message, const FString& Level);

    /** Update expand button */
    void UpdateExpandButton();

    /** Update visibility */
    void UpdateVisibility();

    /** Update colors */
    void UpdateColors();

    /** Update fonts */
    void UpdateFonts();

    /** Update layout */
    void UpdateLayout();

    /** Update animations */
    void UpdateAnimations();
};
