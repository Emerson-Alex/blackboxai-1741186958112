#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETimelineTypes.h"
#include "Combat/SECombatTypes.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageDealt, float, Damage, AActor*, Target, bool, bWasCritical);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatStateChanged, ECombatState, NewState, ECombatState, OldState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Combat State Management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EnterCombat(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ExitCombat();

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsInCombat() const;

    // Stats Management
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void InitializeWithStats(const FCharacterStats& InitStats);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void UpdateStats(const FCharacterStats& NewStats);

    // Combat Actions
    UFUNCTION(BlueprintCallable, Category = "Combat")
    float CalculateDamage(const FAttackData& AttackData, AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ExecuteAttack(const FAttackData& AttackData, AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyStatusEffect(const FStatusEffect& Effect, AActor* Target);

    // Timeline-Specific Combat
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    float GetTimelineAttackModifier() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    float GetTimelineDefenseModifier() const;

    // Combat Stats Queries
    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetCurrentAttackPower() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetCurrentDefense() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetCriticalChance() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetCriticalMultiplier() const;

    // Combo System
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartCombo();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ContinueCombo(const FName& AbilityName);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndCombo();

    UFUNCTION(BlueprintPure, Category = "Combat")
    int32 GetCurrentComboCount() const;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnDamageDealt OnDamageDealt;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCombatStateChanged OnCombatStateChanged;

protected:
    // Combat Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FCharacterStats BaseStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FCharacterStats CurrentStats;

    // Combat State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    ECombatState CurrentCombatState;

    UPROPERTY()
    AActor* CurrentTarget;

    // Combo System
    UPROPERTY()
    int32 ComboCount;

    UPROPERTY()
    float LastComboTime;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float ComboWindowDuration;

    UPROPERTY()
    TArray<FName> ComboChain;

    // Timeline Combat
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float TimelineBonusMultiplier;

    // Status Effects
    UPROPERTY()
    TArray<FStatusEffect> ActiveStatusEffects;

    // Internal Methods
    void UpdateStatusEffects(float DeltaTime);
    void ProcessComboTimeout(float DeltaTime);
    float CalculateCriticalDamage(float BaseDamage) const;
    bool ValidateComboChain(const FName& AbilityName) const;
    void ApplyCombatModifiers(FCharacterStats& Stats) const;
    void HandleStatusEffectExpiration(const FStatusEffect& Effect);

private:
    // Combat Calculations
    float CalculateTimelineBonus(float BaseValue, ETimelineState State) const;
    float GetStatusEffectModifier(EStatType StatType) const;
    bool ShouldTriggerCritical() const;

    // State Management
    void SetCombatState(ECombatState NewState);
    void CleanupCombatState();

    // Effect Management
    void ApplyTimelineEffects();
    void RemoveExpiredEffects();

    // Combo Management
    void ResetCombo();
    bool IsComboWindowOpen() const;
    float GetComboDamageMultiplier() const;

    // Cache
    TMap<FName, float> AbilityDamageCache;
    void UpdateDamageCache();
};
