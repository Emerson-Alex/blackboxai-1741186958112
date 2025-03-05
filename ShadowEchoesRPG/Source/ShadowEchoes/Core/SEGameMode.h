// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/SETypes.h"
#include "SEGameMode.generated.h"

class UTimelineManager;
class UQuestManager;
class ASEHUD;

/**
 * ASEGameMode - Base game mode for Shadow Echoes
 */
UCLASS()
class SHADOWECHOES_API ASEGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASEGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Get the timeline manager */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Systems")
    UTimelineManager* GetTimelineManager() const { return TimelineManager; }

    /** Get the quest manager */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Systems")
    UQuestManager* GetQuestManager() const { return QuestManager; }

protected:
    /** Timeline manager */
    UPROPERTY()
    TObjectPtr<UTimelineManager> TimelineManager;

    /** Quest manager */
    UPROPERTY()
    TObjectPtr<UQuestManager> QuestManager;

    /** Initialize game systems */
    virtual void InitializeGameSystems();

    /** Initialize quest system */
    virtual void InitializeQuestSystem();

    /** Load initial quests */
    virtual void LoadInitialQuests();

    /** Create default quests */
    virtual void CreateDefaultQuests();

    /** Connect systems to HUD */
    virtual void ConnectSystemsToHUD();

private:
    /** Logger category */
    static const FName LogCategory;
};
