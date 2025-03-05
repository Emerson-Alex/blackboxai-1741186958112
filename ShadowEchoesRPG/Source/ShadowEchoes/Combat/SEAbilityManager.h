// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Combat/SECombatTypes.h"
#include "SEAbilityManager.generated.h"

class USEGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityUnlocked, const FAbilityInfo&, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboStarted, const FAbilityInfo&, ComboAbility);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboCompleted, const FAbilityInfo&, ComboAbility);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboFailed);

/**
 * Manages abilities, combos, and skill unlocks
 */
UCLASS()
class SHADOWECHOES_API USEAbilityManager : public UObject
{
    GENERATED_BODY()

public:
    USEAbilityManager();

    /** Initialize the ability system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Ability management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool UnlockAbility(const FAbilityInfo& Ability);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool HasAbility(const FName& AbilityID) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Abilities")
    const FAbilityInfo* GetAbilityInfo(const FName& AbilityID) const;

    /** Ability execution */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool CanUseAbility(const FName& AbilityID) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool UseAbility(const FName& AbilityID);

    /** Combo system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool StartCombo(const FName& AbilityID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool ContinueCombo(const FName& AbilityID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    void EndCombo();

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Boss ability unlocks */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    TArray<FAbilityInfo> GetUnlockableAbilities(const FName& BossID) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Abilities")
    bool CanUnlockBossAbility(const FName& AbilityID) const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Abilities|Events")
    FOnAbilityUnlocked OnAbilityUnlocked;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Abilities|Events")
    FOnComboStarted OnComboStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Abilities|Events")
    FOnComboCompleted OnComboCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Abilities|Events")
    FOnComboFailed OnComboFailed;

protected:
    /** Ability settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Abilities")
    float ComboTimeoutDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Abilities")
    int32 MaxComboLength;

private:
    /** Unlocked abilities */
    UPROPERTY()
    TMap<FName, FAbilityInfo> UnlockedAbilities;

    /** Active abilities */
    UPROPERTY()
    TMap<FName, float> AbilityCooldowns;

    /** Combo state */
    bool bIsInCombo;
    FName CurrentComboAbilityID;
    TArray<FName> CurrentComboChain;
    float LastComboTime;
    int32 CurrentComboHits;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    /** Current timeline state */
    ETimelineState CurrentTimelineState;

    /** Ability validation */
    bool ValidateAbility(const FAbilityInfo& Ability) const;
    bool CheckAbilityRequirements(const FAbilityInfo& Ability) const;
    bool CheckTimelineRequirement(const FAbilityInfo& Ability) const;

    /** Combo helpers */
    bool ValidateCombo(const FName& AbilityID) const;
    bool CheckComboRequirements(const FAbilityInfo& Ability) const;
    void ProcessComboHit();
    void CheckComboTimeout();
    void ResetCombo();

    /** Cooldown management */
    void UpdateCooldowns(float DeltaTime);
    void StartCooldown(const FName& AbilityID);
    bool IsOnCooldown(const FName& AbilityID) const;
    float GetRemainingCooldown(const FName& AbilityID) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Abilities|Events")
    void BP_OnAbilityUnlocked(const FAbilityInfo& Ability);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Abilities|Events")
    void BP_OnComboStarted(const FAbilityInfo& ComboAbility);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Abilities|Events")
    void BP_OnComboCompleted(const FAbilityInfo& ComboAbility);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Abilities|Events")
    void BP_OnComboFailed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Abilities|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
