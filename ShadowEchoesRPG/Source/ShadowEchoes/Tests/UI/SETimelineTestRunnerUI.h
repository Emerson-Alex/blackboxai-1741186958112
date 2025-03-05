#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SETimelineTestData.h"
#include "SETimelineTestStyle.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "SETimelineTestResultEntry.h"
#include "SETimelineTestRunnerUI.generated.h"

/**
 * UI for running and monitoring Timeline tests
 */
UCLASS()
class SHADOWECHOES_API USETimelineTestRunnerUI : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Initialize the UI */
    void Initialize();

    /** Update test progress */
    void UpdateProgress(float Progress);

    /** Add test result */
    void AddTestResult(const SETimelineTest::FTestResult& Result);

    /** Clear test results */
    void ClearTestResults();

    /** Set test categories */
    void SetTestCategories(const TArray<FString>& Categories);

    /** Set test priorities */
    void SetTestPriorities(const TArray<FString>& Priorities);

    /** Set test options */
    void SetTestOptions(const SETimelineTest::FTestExecutionOptions& Options);

    /** Get selected categories */
    TArray<FString> GetSelectedCategories() const;

    /** Get selected priorities */
    TArray<FString> GetSelectedPriorities() const;

    /** Get test options */
    SETimelineTest::FTestExecutionOptions GetTestOptions() const;

    /** Get test filter */
    FString GetTestFilter() const;

    /** Set test filter */
    void SetTestFilter(const FString& Filter);

    /** Set test status */
    void SetTestStatus(const FString& Status);

    /** Set error message */
    void SetErrorMessage(const FString& Message);

    /** Show loading state */
    void ShowLoading(bool bShow);

    /** Show error state */
    void ShowError(bool bShow);

    /** Show success state */
    void ShowSuccess(bool bShow);

    /** Show warning state */
    void ShowWarning(bool bShow);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Create widget bindings */
    void CreateBindings();

    /** Update widget appearance */
    void UpdateAppearance();

    /** Handle run button clicked */
    UFUNCTION()
    void OnRunButtonClicked();

    /** Handle stop button clicked */
    UFUNCTION()
    void OnStopButtonClicked();

    /** Handle clear button clicked */
    UFUNCTION()
    void OnClearButtonClicked();

    /** Handle category selection changed */
    UFUNCTION()
    void OnCategorySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    /** Handle priority selection changed */
    UFUNCTION()
    void OnPrioritySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    /** Handle option changed */
    UFUNCTION()
    void OnOptionChanged();

    /** Handle filter changed */
    UFUNCTION()
    void OnFilterChanged(const FText& Text);

    /** Handle test result clicked */
    UFUNCTION()
    void OnTestResultClicked(USETimelineTestResultEntry* ResultEntry);

    /** Handle test result expanded */
    UFUNCTION()
    void OnTestResultExpanded(USETimelineTestResultEntry* ResultEntry);

    /** Handle test result collapsed */
    UFUNCTION()
    void OnTestResultCollapsed(USETimelineTestResultEntry* ResultEntry);

private:
    /** Widget references */
    UPROPERTY()
    UScrollBox* ResultsBox;

    UPROPERTY()
    UButton* RunButton;

    UPROPERTY()
    UButton* StopButton;

    UPROPERTY()
    UButton* ClearButton;

    UPROPERTY()
    UComboBoxString* CategoryComboBox;

    UPROPERTY()
    UComboBoxString* PriorityComboBox;

    UPROPERTY()
    UCheckBox* StopOnFailureCheckBox;

    UPROPERTY()
    UCheckBox* CaptureScreenshotsCheckBox;

    UPROPERTY()
    UCheckBox* GenerateReportsCheckBox;

    UPROPERTY()
    UProgressBar* ProgressBar;

    UPROPERTY()
    UTextBlock* StatusText;

    UPROPERTY()
    UTextBlock* ErrorText;

    UPROPERTY()
    UEditableTextBox* FilterBox;

    UPROPERTY()
    UBorder* BackgroundBorder;

    /** Test results */
    TArray<USETimelineTestResultEntry*> TestResults;

    /** Style */
    FSETimelineTestStyle* Style;

    /** Create widgets */
    void CreateWidgets();

    /** Create results box */
    UScrollBox* CreateResultsBox();

    /** Create run button */
    UButton* CreateRunButton();

    /** Create stop button */
    UButton* CreateStopButton();

    /** Create clear button */
    UButton* CreateClearButton();

    /** Create category combo box */
    UComboBoxString* CreateCategoryComboBox();

    /** Create priority combo box */
    UComboBoxString* CreatePriorityComboBox();

    /** Create stop on failure checkbox */
    UCheckBox* CreateStopOnFailureCheckBox();

    /** Create capture screenshots checkbox */
    UCheckBox* CreateCaptureScreenshotsCheckBox();

    /** Create generate reports checkbox */
    UCheckBox* CreateGenerateReportsCheckBox();

    /** Create progress bar */
    UProgressBar* CreateProgressBar();

    /** Create status text */
    UTextBlock* CreateStatusText();

    /** Create error text */
    UTextBlock* CreateErrorText();

    /** Create filter box */
    UEditableTextBox* CreateFilterBox();

    /** Create background border */
    UBorder* CreateBackgroundBorder();

    /** Update colors */
    void UpdateColors();

    /** Update fonts */
    void UpdateFonts();

    /** Update layout */
    void UpdateLayout();

    /** Update animations */
    void UpdateAnimations();

    /** Update visibility */
    void UpdateVisibility();

    /** Update enabled state */
    void UpdateEnabledState();

    /** Update test results */
    void UpdateTestResults();

    /** Update test options */
    void UpdateTestOptions();

    /** Update test filter */
    void UpdateTestFilter();

    /** Update test status */
    void UpdateTestStatus();

    /** Update error message */
    void UpdateErrorMessage();

    /** Update loading state */
    void UpdateLoadingState();

    /** Update error state */
    void UpdateErrorState();

    /** Update success state */
    void UpdateSuccessState();

    /** Update warning state */
    void UpdateWarningState();

    /** Delegates */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunTests);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopTests);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearTests);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTestResultClicked, USETimelineTestResultEntry*, ResultEntry);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTestResultExpanded, USETimelineTestResultEntry*, ResultEntry);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTestResultCollapsed, USETimelineTestResultEntry*, ResultEntry);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRunTests OnRunTests;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStopTests OnStopTests;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnClearTests OnClearTests;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTestResultClicked OnTestResultClicked;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTestResultExpanded OnTestResultExpanded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnTestResultCollapsed OnTestResultCollapsed;
};
