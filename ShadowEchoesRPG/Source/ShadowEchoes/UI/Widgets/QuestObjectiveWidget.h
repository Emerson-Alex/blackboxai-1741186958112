// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "QuestObjectiveWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;

/**
 * Widget for displaying individual quest objectives and their progress
 */
UCLASS()
class SHADOWECHOES_API UQuestObjectiveWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UQuestObjectiveWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Objective info */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetObjectiveInfo(const FName& InObjectiveID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void UpdateProgress(float NewProgress);

    /** Objective state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetComplete(bool bComplete);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void SetOptional(bool bOptional);

    /** Timeline state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Getters */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    float GetProgress() const { return Progress; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    bool IsComplete() const { return bIsComplete; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Quests")
    bool IsOptional() const { return bIsOptional; }

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* ObjectiveText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* ObjectiveProgressBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* CompleteIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* OptionalIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TimelineIcon;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor NormalColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor CompleteColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor OptionalColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* BrightTimelineIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* DarkTimelineIcon;

private:
    /** Objective data */
    FName ObjectiveID;
    float Progress;
    bool bIsComplete;
    bool bIsOptional;
    ETimelineState RequiredTimeline;

    /** Initialize UI */
    void InitializeWidgets();

    /** Update visuals */
    void UpdateVisuals();
    void UpdateTimelineIcon();
    void UpdateColors();
    void UpdateProgressDisplay();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnObjectiveSet(const FQuestObjectiveInfo& Objective);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnProgressUpdated(float NewProgress);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnCompleteStateChanged(bool bComplete);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnOptionalStateChanged(bool bOptional);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Quests")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
