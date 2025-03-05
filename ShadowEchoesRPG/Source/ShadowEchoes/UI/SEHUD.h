// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/SETypes.h"
#include "SEHUD.generated.h"

class UTimelineIndicatorWidget;
class UCombatWidget;
class UQuestLogWidget;
class UQuestManager;

/**
 * ASEHUD - Main HUD class for Shadow Echoes
 */
UCLASS()
class SHADOWECHOES_API ASEHUD : public AHUD
{
    GENERATED_BODY()

public:
    ASEHUD();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Get the timeline indicator */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UTimelineIndicatorWidget* GetTimelineIndicator() const { return TimelineIndicator; }

    /** Get combat widget */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UCombatWidget* GetCombatWidget() const { return CombatWidget; }

    /** Get quest log widget */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UQuestLogWidget* GetQuestLogWidget() const { return QuestLogWidget; }

    /** Show quest notification */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void ShowQuestNotification(const FText& QuestTitle, const FText& Message);

    /** Update quest log */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void UpdateQuestLog(const TArray<FQuestData>& Quests);

    /** Toggle quest log visibility */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void ToggleQuestLog();

    /** Set quest manager reference */
    void SetQuestManager(UQuestManager* InQuestManager);

protected:
    /** Widget classes */
    UPROPERTY(EditDefaultsOnly, Category = "UI|Widgets")
    TSubclassOf<UUserWidget> TimelineIndicatorClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Widgets")
    TSubclassOf<UUserWidget> CombatWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Widgets")
    TSubclassOf<UUserWidget> QuestLogWidgetClass;

    /** Widget instances */
    UPROPERTY()
    TObjectPtr<UTimelineIndicatorWidget> TimelineIndicator;

    UPROPERTY()
    TObjectPtr<UCombatWidget> CombatWidget;

    UPROPERTY()
    TObjectPtr<UQuestLogWidget> QuestLogWidget;

    /** Quest manager reference */
    UPROPERTY()
    TObjectPtr<UQuestManager> QuestManager;

    /** Initialize widgets */
    virtual void CreateWidgets();

    /** Handle quest events */
    UFUNCTION()
    void OnQuestAdded(const FQuestData& QuestData);

    UFUNCTION()
    void OnQuestCompleted(const FQuestData& QuestData);

    UFUNCTION()
    void OnQuestFailed(const FQuestData& QuestData);

    UFUNCTION()
    void OnQuestProgressUpdated(const FName& QuestID, const FQuestObjective& UpdatedObjective);

    /** Bind quest events */
    void BindQuestEvents();

    /** Unbind quest events */
    void UnbindQuestEvents();

private:
    /** Logger category */
    static const FName LogCategory;
};
