// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/SETypes.h"
#include "SEGameInstance.generated.h"

class UTimelineManager;
class UQuestManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineStateChanged, ETimelineState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, const FQuestInfo&, Quest, EQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelUp, int32, NewLevel);

/**
 * Game Instance class for Shadow Echoes RPG
 * Manages global game state and systems
 */
UCLASS()
class SHADOWECHOES_API USEGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    USEGameInstance();

    virtual void Init() override;
    virtual void Shutdown() override;

    /** Timeline management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void SetTimelineState(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    ETimelineState GetCurrentTimelineState() const { return CurrentTimelineState; }

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void RegisterTimelineManager(UTimelineManager* Manager);

    /** Quest management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    void StartQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    void CompleteQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    void FailQuest(const FName& QuestID);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    EQuestState GetQuestState(const FName& QuestID) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    TArray<FQuestInfo> GetActiveQuests() const;

    /** Player progression */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Player")
    void AddExperience(int32 XP);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Player")
    void AddCurrency(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Player")
    int32 GetPlayerLevel() const { return PlayerLevel; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Player")
    int32 GetPlayerCurrency() const { return PlayerCurrency; }

    /** Save/Load system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|SaveGame")
    bool SaveGame();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|SaveGame")
    bool LoadGame();

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Timeline|Events")
    FOnTimelineStateChanged OnTimelineStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Quests|Events")
    FOnQuestStateChanged OnQuestStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Player|Events")
    FOnPlayerLevelUp OnPlayerLevelUp;

protected:
    /** Timeline settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline")
    ETimelineState DefaultTimelineState;

    /** Experience settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Player")
    UCurveFloat* ExperienceCurve;

    /** Manager references */
    UPROPERTY()
    UTimelineManager* TimelineManager;

    UPROPERTY()
    UQuestManager* QuestManager;

private:
    /** Current game state */
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    /** Player data */
    UPROPERTY()
    int32 PlayerLevel;

    UPROPERTY()
    int32 PlayerExperience;

    UPROPERTY()
    int32 PlayerCurrency;

    /** Quest data */
    UPROPERTY()
    TMap<FName, EQuestState> QuestStates;

    /** Initialize systems */
    void InitializeManagers();
    void InitializePlayerData();
    void LoadQuestData();

    /** Experience helpers */
    void CheckLevelUp();
    int32 GetExperienceForLevel(int32 Level) const;

    /** Save/Load helpers */
    void SavePlayerData();
    void SaveQuestData();
    void LoadPlayerData();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Quests|Events")
    void BP_OnQuestStateChanged(const FQuestInfo& Quest, EQuestState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Player|Events")
    void BP_OnPlayerLevelUp(int32 NewLevel);
};
