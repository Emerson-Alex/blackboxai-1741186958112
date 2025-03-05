// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SEPvPManager.generated.h"

class USEGameInstance;
class UTimelineManager;

UENUM(BlueprintType)
enum class EPvPMode : uint8
{
    None            UMETA(DisplayName = "None"),
    TimelineInvasion UMETA(DisplayName = "Timeline Invasion"),
    ArenaMatch      UMETA(DisplayName = "Arena Match"),
    TimelineWar     UMETA(DisplayName = "Timeline War")
};

USTRUCT(BlueprintType)
struct FPvPMatchData
{
    GENERATED_BODY()

    UPROPERTY()
    FString MatchID;

    UPROPERTY()
    EPvPMode Mode;

    UPROPERTY()
    TArray<FString> PlayerIDs;

    UPROPERTY()
    ETimelineState Timeline;

    UPROPERTY()
    FString AreaID;

    UPROPERTY()
    bool bIsRanked;

    UPROPERTY()
    int32 RankThreshold;
};

USTRUCT(BlueprintType)
struct FInvasionData
{
    GENERATED_BODY()

    UPROPERTY()
    FString InvaderID;

    UPROPERTY()
    FString TargetID;

    UPROPERTY()
    ETimelineState SourceTimeline;

    UPROPERTY()
    ETimelineState TargetTimeline;

    UPROPERTY()
    FString AreaID;

    UPROPERTY()
    float TimeLimit;

    UPROPERTY()
    TArray<FName> RestrictedAbilities;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvasionStarted, const FInvasionData&, InvasionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStarted, const FPvPMatchData&, MatchData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPvPReward, const FString&, WinnerID, const TArray<FName>&, Rewards);

/**
 * Manages PvP systems including timeline invasions, arena matches, and timeline wars
 */
UCLASS()
class SHADOWECHOES_API USEPvPManager : public UObject
{
    GENERATED_BODY()

public:
    USEPvPManager();

    /** Initialize the PvP system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Invasion system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    bool StartInvasion(const FString& InvaderID, const FString& TargetID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    void EndInvasion(const FString& InvasionID, const FString& WinnerID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    bool CanInvade(const FString& PlayerID) const;

    /** Arena system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    bool QueueForArena(const FString& PlayerID, bool bRanked = false);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    void StartArenaMatch(const FPvPMatchData& MatchData);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    void EndArenaMatch(const FString& MatchID, const FString& WinnerID);

    /** Timeline War system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    bool JoinTimelineWar(const FString& PlayerID, ETimelineState Timeline);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    void UpdateWarProgress(ETimelineState Timeline, int32 Points);

    /** Ranking system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    int32 GetPlayerRank(const FString& PlayerID) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|PvP")
    void UpdatePlayerRank(const FString& PlayerID, int32 RankChange);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|PvP|Events")
    FOnInvasionStarted OnInvasionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|PvP|Events")
    FOnMatchStarted OnMatchStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|PvP|Events")
    FOnPvPReward OnPvPReward;

protected:
    /** PvP settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|PvP")
    float InvasionCooldown;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|PvP")
    int32 MaxSimultaneousInvasions;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|PvP")
    float ArenaMatchDuration;

private:
    /** Current state */
    UPROPERTY()
    TMap<FString, FInvasionData> ActiveInvasions;

    UPROPERTY()
    TMap<FString, FPvPMatchData> ActiveMatches;

    UPROPERTY()
    TMap<ETimelineState, int32> TimelineWarScores;

    UPROPERTY()
    TMap<FString, float> InvasionCooldowns;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    bool ValidateInvasion(const FString& InvaderID, const FString& TargetID) const;
    bool ValidateArenaMatch(const FPvPMatchData& MatchData) const;
    void ProcessMatchmaking();
    void UpdateTimelineWar();
    void CalculateRewards(const FString& WinnerID, EPvPMode Mode);
    bool CheckTimelineCompatibility(ETimelineState Source, ETimelineState Target) const;
    void ApplyInvasionEffects(const FInvasionData& Invasion);
    int32 CalculateRankChange(int32 WinnerRank, int32 LoserRank) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|PvP|Events")
    void BP_OnInvasionStarted(const FInvasionData& InvasionData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|PvP|Events")
    void BP_OnMatchStarted(const FPvPMatchData& MatchData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|PvP|Events")
    void BP_OnPvPReward(const FString& WinnerID, const TArray<FName>& Rewards);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|PvP|Events")
    void BP_OnTimelineWarUpdate(ETimelineState Timeline, int32 Points);
};
