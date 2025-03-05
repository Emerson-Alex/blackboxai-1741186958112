#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETimelineTypes.h"
#include "Combat/SECombatTypes.h"
#include "AbilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityActivated, const FName&, AbilityName, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityLearned, const FAbilityData&, AbilityData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownUpdated, const FName&, AbilityName, float, RemainingTime);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API UAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Ability Management
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    bool ActivateAbility(const FName& AbilityName);

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void LearnAbility(const FAbilityData& AbilityData);

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ForgetAbility(const FName& AbilityName);

    UFUNCTION(BlueprintPure, Category = "Abilities")
    bool HasAbility(const FName& AbilityName) const;

    UFUNCTION(BlueprintPure, Category = "Abilities")
    const FAbilityData* GetAbilityData(const FName& AbilityName) const;

    // Timeline Integration
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState);

    UFUNCTION(BlueprintPure, Category = "Abilities")
    TArray<FName> GetAvailableAbilities(ETimelineState State) const;

    // Cooldown Management
    UFUNCTION(BlueprintPure, Category = "Abilities")
    float GetRemainingCooldown(const FName& AbilityName) const;

    UFUNCTION(BlueprintPure, Category = "Abilities")
    bool IsAbilityReady(const FName& AbilityName) const;

    // Resource Management
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    bool HasEnoughResources(const FAbilityData& AbilityData) const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ConsumeResources(const FAbilityData& AbilityData);

    // Combo System Integration
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    bool CanUseInCombo(const FName& AbilityName, const TArray<FName>& CurrentCombo) const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    float GetComboBonus(const FName& AbilityName, const TArray<FName>& CurrentCombo) const;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Abilities")
    FOnAbilityActivated OnAbilityActivated;

    UPROPERTY(BlueprintAssignable, Category = "Abilities")
    FOnAbilityLearned OnAbilityLearned;

    UPROPERTY(BlueprintAssignable, Category = "Abilities")
    FOnCooldownUpdated OnCooldownUpdated;

protected:
    // Ability Storage
    UPROPERTY()
    TMap<FName, FAbilityData> LearnedAbilities;

    UPROPERTY()
    TMap<FName, float> AbilityCooldowns;

    // Timeline State
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    // Ability Execution
    bool ExecuteAbility(const FAbilityData& AbilityData);
    bool ValidateAbilityUse(const FAbilityData& AbilityData) const;
    void ApplyAbilityEffects(const FAbilityData& AbilityData);
    void StartCooldown(const FName& AbilityName, float Duration);

    // Timeline Effects
    void ApplyTimelineModifiers(FAbilityData& AbilityData) const;
    float CalculateTimelineBonus(const FAbilityData& AbilityData) const;

    // Resource Management
    bool CheckResourceCosts(const FAbilityData& AbilityData) const;
    void HandleResourceConsumption(const FAbilityData& AbilityData);

    // Visual Effects
    void PlayAbilityEffects(const FAbilityData& AbilityData);
    void HandleAbilityFailure(const FName& AbilityName, const FString& Reason);

private:
    // Cooldown Management
    void UpdateCooldowns(float DeltaTime);
    void ClearCooldown(const FName& AbilityName);

    // Combo System
    TArray<FName> CurrentComboChain;
    float LastComboTime;
    static const float ComboWindowDuration;

    // Cache
    TMap<FName, float> DamageCache;
    void UpdateDamageCache();

    // Validation
    bool IsValidAbilityForCurrentState(const FAbilityData& AbilityData) const;
    bool AreResourcesAvailable(const FAbilityData& AbilityData) const;

    // Performance Optimization
    void CleanupExpiredEffects();
    void ManageAbilityCache();

    // Error Handling
    void LogAbilityError(const FString& ErrorMessage) const;
    bool ValidateAbilityData(const FAbilityData& AbilityData) const;
};
