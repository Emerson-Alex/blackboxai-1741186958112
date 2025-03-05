// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "QuestEntryWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEntrySelected, const FName&, QuestID);

/**
 * Widget for displaying individual quest entries in the quest log
 */
UCLASS()
class SHADOWECHOES_API UQuestEntryWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UQuestEntryWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Quest info */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetQuestInfo(const FQuestInfo& Quest);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void UpdateProgress(float NewProgress);

    /** Quest tracking */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetTracked(bool bTracked);

    /** Quest state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetSelected(bool bSelected);

    /** Quest info getters */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    int32 GetQuestLevel() const { return QuestLevel; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    float GetQuestProgress() const { return Progress; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    bool IsTracked() const { return bIsTracked; }

    /** Selection event */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|UI|Quests")
    FOnQuestEntrySelected OnQuestSelected;

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* QuestTitleText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* QuestLevelText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* QuestProgressBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* SelectButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TrackedIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TimelineIcon;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor NormalColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor SelectedColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor TrackedColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* BrightTimelineIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* DarkTimelineIcon;

private:
    /** Quest data */
    FName QuestID;
    int32 QuestLevel;
    ETimelineState RequiredTimeline;
    float Progress;
    bool bIsTracked;
    bool bIsSelected;

    /** Initialize UI */
    void InitializeWidgets();

    /** Update visuals */
    void UpdateVisuals();
    void UpdateTimelineIcon();
    void UpdateColors();

    /** Button callbacks */
    UFUNCTION()
    void OnSelectButtonClicked();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnQuestSet(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnProgressUpdated(float NewProgress);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnTrackedStateChanged(bool bTracked);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnSelectedStateChanged(bool bSelected);
};
