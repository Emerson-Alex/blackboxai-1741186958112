// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Combat/SECombatTypes.h"
#include "SEBossManager.generated.h"

class USEGameInstance;
class USEAbilityManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPhaseChanged, const FBossPhaseInfo&, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossDefeated, const FName&, BossID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossAbilityAvailable, const FName&, BossID, const FAbilityInfo&, Ability);

/**
 * Boss fight info
 */
USTRUCT(BlueprintType)
struct FBossInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FName BossID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FBossPhaseInfo> Phases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FName> UnlockableAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    ETimelineState PreferredTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FCombatStats BaseStats;

    FBossInfo()
        : BossID(NAME_None)
        , RequiredLevel(1)
        , PreferredTimeline(ETimelineState::Any)
    {
    }
};

/**
 * Manages boss fights and ability unlocks
 */
UCLASS()
class SHADOWECHOES_API USEBossManager : public UObject
{
    GENERATED_BODY()

public:
    USEBossManager();

    /** Initialize the boss system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Boss management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    bool StartBossFight(const FName& BossID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    void EndBossFight(bool bWasVictorious);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Bosses")
    bool IsBossFightActive() const { return bIsBossFightActive; }

    /** Boss info */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Bosses")
    const FBossInfo* GetBossInfo(const FName& BossID) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Bosses")
    const FBossPhaseInfo* GetCurrentPhase() const;

    /** Boss phases */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    void UpdateBossHealth(float NewHealthPercent);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    void AdvanceToNextPhase();

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Ability unlocks */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    TArray<FAbilityInfo> GetUnlockableAbilities() const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Bosses")
    bool UnlockBossAbility(const FName& AbilityID);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Bosses|Events")
    FOnBossPhaseChanged OnBossPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Bosses|Events")
    FOnBossDefeated OnBossDefeated;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Bosses|Events")
    FOnBossAbilityAvailable OnBossAbilityAvailable;

protected:
    /** Boss settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Bosses")
    float PhaseTransitionDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Bosses")
    float TimelinePreferenceBonus;

private:
    /** Current boss state */
    bool bIsBossFightActive;
    FName CurrentBossID;
    int32 CurrentPhaseIndex;
    float CurrentHealthPercent;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    /** Ability manager reference */
    UPROPERTY()
    USEAbilityManager* AbilityManager;

    /** Boss database */
    UPROPERTY()
    TMap<FName, FBossInfo> BossDatabase;

    /** Boss validation */
    bool ValidateBoss(const FName& BossID) const;
    bool CheckBossRequirements(const FBossInfo& Boss) const;

    /** Phase management */
    void InitializePhases();
    void CheckPhaseTransition();
    void TransitionToPhase(int32 PhaseIndex);
    void ApplyPhaseModifiers(const FBossPhaseInfo& Phase);

    /** Timeline helpers */
    bool IsInPreferredTimeline() const;
    float CalculateTimelineBonus() const;

    /** Ability helpers */
    void CheckAbilityUnlocks();
    bool ValidateAbilityUnlock(const FName& AbilityID) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Bosses|Events")
    void BP_OnBossPhaseChanged(const FBossPhaseInfo& NewPhase);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Bosses|Events")
    void BP_OnBossDefeated(const FName& BossID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Bosses|Events")
    void BP_OnBossAbilityAvailable(const FName& BossID, const FAbilityInfo& Ability);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Bosses|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
