// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Combat/SECombatTypes.h"
#include "SEDungeonManager.generated.h"

class USEGameInstance;
class UTimelineManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDungeonStarted, const FName&, DungeonID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDungeonCompleted, const FName&, DungeonID, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLegendaryRewardGranted, const FName&, ItemID, const FName&, DungeonID);

USTRUCT(BlueprintType)
struct FDungeonProgress
{
    GENERATED_BODY()

    UPROPERTY()
    FName DungeonID;

    UPROPERTY()
    int32 CurrentPhase;

    UPROPERTY()
    float TimeElapsed;

    UPROPERTY()
    TArray<bool> CompletedObjectives;

    UPROPERTY()
    TArray<FName> DefeatedBosses;

    UPROPERTY()
    bool bTimelineChallengeActive;

    UPROPERTY()
    int32 RealityTearCount;
};

/**
 * Manages legendary dungeons, their mechanics, and rewards
 */
UCLASS()
class SHADOWECHOES_API USEDungeonManager : public UObject
{
    GENERATED_BODY()

public:
    USEDungeonManager();

    /** Initialize the dungeon system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Dungeon management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    bool StartDungeon(const FName& DungeonID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void EndDungeon(bool bSuccess);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void UpdateDungeonProgress(const FDungeonProgress& Progress);

    /** Timeline mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void TriggerTimelineFlux();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void CreateRealityTear(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void TriggerTimeDilation(float DilationFactor);

    /** Challenge modes */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void ActivateWeeklyModifier(const FName& ModifierID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void StartTimeParadoxChallenge();

    /** Rewards */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Dungeons")
    void GrantLegendaryReward();

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Dungeons")
    bool HasCompletedDungeon(const FName& DungeonID) const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Dungeons|Events")
    FOnDungeonStarted OnDungeonStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Dungeons|Events")
    FOnDungeonCompleted OnDungeonCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Dungeons|Events")
    FOnLegendaryRewardGranted OnLegendaryRewardGranted;

protected:
    /** Dungeon settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Dungeons")
    float TimelineFluxInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Dungeons")
    int32 MaxRealityTears;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Dungeons")
    float TimeDilationRange;

private:
    /** Current state */
    UPROPERTY()
    FName CurrentDungeonID;

    UPROPERTY()
    FDungeonProgress CurrentProgress;

    UPROPERTY()
    TArray<FName> CompletedDungeons;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Validation */
    bool ValidateDungeonRequirements(const FName& DungeonID) const;
    bool CheckGroupRequirements(const FName& DungeonID) const;
    bool CheckTimelineMasteryRequirements(const FName& DungeonID) const;

    /** Mechanics */
    void ProcessTimelineFlux();
    void ManageRealityTears();
    void UpdateTimeDilation();
    void HandleTimeParadox();

    /** Rewards */
    void DetermineRewards() const;
    void ProcessLegendaryDrop() const;
    void ApplyWeeklyModifierBonus() const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnDungeonStarted(const FName& DungeonID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnDungeonCompleted(const FName& DungeonID, bool bSuccess);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnLegendaryRewardGranted(const FName& ItemID, const FName& DungeonID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnTimelineFlux();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnRealityTearCreated(const FVector& Location);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Dungeons|Events")
    void BP_OnTimeDilationChanged(float DilationFactor);
};
