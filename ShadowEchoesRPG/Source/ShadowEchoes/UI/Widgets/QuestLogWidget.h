// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "QuestLogWidget.generated.h"

class UQuestEntryWidget;
class UQuestObjectiveWidget;
class UVerticalBox;
class UScrollBox;
class UTextBlock;
class UButton;

/**
 * Widget for displaying and managing quests
 */
UCLASS()
class SHADOWECHOES_API UQuestLogWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UQuestLogWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Quest display */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void DisplayQuest(const FQuestInfo& Quest);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void RefreshQuestList();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void FilterQuests(ETimelineState Timeline);

    /** Quest tracking */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void TrackQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void UntrackQuest(const FName& QuestID);

    /** Quest sorting */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SortQuestsByLevel();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SortQuestsByProgress();

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* QuestListBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UScrollBox* QuestScrollBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* QuestTitleText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* QuestDescriptionText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UVerticalBox* ObjectivesBox;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* TrackButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* AbandonButton;

    /** Widget classes */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UQuestEntryWidget> QuestEntryWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UQuestObjectiveWidget> QuestObjectiveWidgetClass;

    /** Timeline filter */
    UPROPERTY(BlueprintReadOnly, Category = "Shadow Echoes|UI|Quests")
    ETimelineState CurrentTimelineFilter;

private:
    /** Active widgets */
    UPROPERTY()
    TMap<FName, UQuestEntryWidget*> QuestEntryWidgets;

    UPROPERTY()
    TMap<FName, UQuestObjectiveWidget*> ObjectiveWidgets;

    /** Currently selected quest */
    UPROPERTY()
    FName SelectedQuestID;

    /** Currently tracked quests */
    UPROPERTY()
    TArray<FName> TrackedQuests;

    /** Initialize UI */
    void InitializeWidgets();
    void SetupCallbacks();

    /** Quest list management */
    void ClearQuestList();
    void AddQuestEntry(const FQuestInfo& Quest);
    void UpdateQuestEntry(const FName& QuestID);

    /** Objective display */
    void DisplayObjectives(const FQuestInfo& Quest);
    void ClearObjectives();
    void UpdateObjectiveProgress(const FName& QuestID, const FName& ObjectiveID);

    /** UI callbacks */
    UFUNCTION()
    void OnQuestSelected(const FName& QuestID);

    UFUNCTION()
    void OnTrackButtonClicked();

    UFUNCTION()
    void OnAbandonButtonClicked();

    /** Quest system callbacks */
    UFUNCTION()
    void OnQuestStarted(const FQuestInfo& Quest);

    UFUNCTION()
    void OnQuestCompleted(const FQuestInfo& Quest);

    UFUNCTION()
    void OnQuestFailed(const FQuestInfo& Quest);

    UFUNCTION()
    void OnObjectiveCompleted(const FName& QuestID, const FName& ObjectiveID);

    UFUNCTION()
    void OnObjectiveProgressUpdated(const FName& QuestID, const FName& ObjectiveID, float Progress);

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnQuestSelected(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnQuestTracked(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnQuestUntracked(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnTimelineFilterChanged(ETimelineState NewFilter);
};
