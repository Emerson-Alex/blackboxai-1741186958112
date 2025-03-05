// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SERaidManager.generated.h"

class USEGameInstance;
class UTimelineManager;

UENUM(BlueprintType)
enum class ERaidPhase : uint8
{
    None            UMETA(DisplayName = "None"),
    Preparation     UMETA(DisplayName = "Preparation"),
    Combat         UMETA(DisplayName = "Combat"),
    Transition     UMETA(DisplayName = "Timeline Transition"),
    Final          UMETA(DisplayName = "Final Phase")
};

USTRUCT(BlueprintType)
struct FRaidEncounter
{
    GENERATED_BODY()

    UPROPERTY()
    FString EncounterID;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    TArray<ERaidPhase> Phases;

    UPROPERTY()
    TArray<ETimelineState> RequiredTimelines;

    UPROPERTY()
    int32 RequiredPlayers;

    UPROPERTY()
    float DurationLimit;

    UPROPERTY()
    TArray<FName> MechanicIDs;

    UPROPERTY()
    TArray<FName> RewardIDs;
};

USTRUCT(BlueprintType)
struct FRaidMechanic
{
    GENERATED_BODY()

    UPROPERTY()
    FName MechanicID;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    ERaidPhase Phase;

    UPROPERTY()
    ETimelineState Timeline;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    float DamageMultiplier;

    UPROPERTY()
    TArray<FName> RequiredAbilities;
};

USTRUCT(BlueprintType)
struct FRaidProgress
{
    GENERATED_BODY()

    UPROPERTY()
    FString RaidID;

    UPROPERTY()
    ERaidPhase CurrentPhase;

    UPROPERTY()
    float TimeElapsed;

    UPROPERTY()
    TArray<FString> ParticipantIDs;

    UPROPERTY()
    TArray<FName> CompletedMechanics;

    UPROPERTY()
    int32 DeathCounter;

    UPROPERTY()
    TArray<FName> CollectedRewards;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaidStarted, const FRaidEncounter&, Encounter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaidPhaseChanged, ERaidPhase, NewPhase, const TArray<FName>&, Mechanics);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaidCompleted, const FString&, RaidID, bool, bSuccess);

/**
 * Manages raid encounters with FromSoftware-style mechanics and timeline challenges
 */
UCLASS()
class SHADOWECHOES_API USERaidManager : public UObject
{
    GENERATED_BODY()

public:
    USERaidManager();

    /** Initialize the raid system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Raid management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    bool StartRaid(const FString& RaidID, const TArray<FString>& ParticipantIDs);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void EndRaid(const FString& RaidID, bool bSuccess);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void UpdateRaidProgress(const FRaidProgress& Progress);

    /** Phase management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void TransitionToPhase(const FString& RaidID, ERaidPhase NewPhase);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void TriggerMechanic(const FString& RaidID, const FName& MechanicID);

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Reward system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Raid")
    void GrantRaidRewards(const FString& RaidID);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Raid")
    bool HasCompletedRaid(const FString& PlayerID, const FString& RaidID) const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Raid|Events")
    FOnRaidStarted OnRaidStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Raid|Events")
    FOnRaidPhaseChanged OnRaidPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Raid|Events")
    FOnRaidCompleted OnRaidCompleted;

protected:
    /** Raid settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Raid")
    float MechanicCheckInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Raid")
    int32 MaxSimultaneousRaids;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Raid")
    float RaidLockoutDuration;

private:
    /** Current state */
    UPROPERTY()
    TMap<FString, FRaidProgress> ActiveRaids;

    UPROPERTY()
    TMap<FString, TArray<FString>> RaidLockouts;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    bool ValidateRaidRequirements(const FString& RaidID, const TArray<FString>& ParticipantIDs) const;
    void ProcessRaidMechanics(const FString& RaidID);
    void UpdateRaidState(const FString& RaidID);
    void ApplyMechanicEffects(const FRaidMechanic& Mechanic);
    void CheckTimelineCompatibility(const FString& RaidID, ETimelineState NewState);
    bool ValidatePhaseTransition(const FString& RaidID, ERaidPhase NewPhase) const;
    void CalculateRewards(const FString& RaidID, bool bSuccess);
    float GetMechanicDifficulty(const FRaidMechanic& Mechanic) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Raid|Events")
    void BP_OnRaidStarted(const FRaidEncounter& Encounter);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Raid|Events")
    void BP_OnRaidPhaseChanged(ERaidPhase NewPhase, const TArray<FName>& Mechanics);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Raid|Events")
    void BP_OnRaidCompleted(const FString& RaidID, bool bSuccess);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Raid|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
