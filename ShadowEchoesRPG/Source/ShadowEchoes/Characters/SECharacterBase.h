#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Core/SETimelineTypes.h"
#include "Combat/SECombatTypes.h"
#include "SECharacterBase.generated.h"

class USETimelineStateManager;
class UCombatComponent;
class UAbilityComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterStateChanged, ETimelineState, NewState, ETimelineState, OldState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

UCLASS()
class SHADOWECHOES_API ASECharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    ASECharacterBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // Timeline State Management
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool RequestTimelineTransition(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category = "Timeline")
    ETimelineState GetCurrentTimelineState() const;

    UFUNCTION(BlueprintPure, Category = "Timeline")
    float GetTimelineEnergy() const;

    // Combat
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void InitializeStats(const FCharacterStats& InitStats);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ApplyDamage(float DamageAmount, AActor* DamageCauser);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void ApplyHealing(float HealAmount);

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetHealthPercent() const;

    // Abilities
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    bool ActivateAbility(const FName& AbilityName);

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void LearnAbility(const FAbilityData& AbilityData);

    // Timeline Effects
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool ApplyTimelineEffect(const FTimelineEffect& Effect);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void RemoveTimelineEffect(const FString& EffectName);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline")
    FOnCharacterStateChanged OnTimelineStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Combat")
    FOnCharacterDeath OnCharacterDeath;

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline")
    USETimelineStateManager* TimelineManager;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UCombatComponent* CombatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilityComponent* AbilityComponent;

    // Character Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FCharacterStats BaseStats;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float CurrentHealth;

    UPROPERTY(Replicated)
    float MaxHealth;

    // Visual Effects
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UParticleSystem* TimelineTransitionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    USoundBase* TimelineTransitionSound;

    // Internal methods
    virtual void HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState);
    virtual void UpdateCharacterVisuals();
    virtual void HandleDeath();

    UFUNCTION()
    virtual void OnRep_CurrentHealth();

    // Network
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // Timeline state cache
    ETimelineState LastTimelineState;

    // Effect management
    void ProcessTimelineEffects(float DeltaTime);
    void UpdateStatModifiers();

    // Combat helpers
    float CalculateDamageModifier() const;
    float CalculateHealingModifier() const;
    bool IsVulnerableToState(ETimelineState State) const;

    // Visual effect helpers
    void PlayTimelineTransitionEffects();
    void UpdateTimelineVisuals();

    // State validation
    bool ValidateTimelineTransition(ETimelineState NewState) const;
    bool HasRequiredEnergy(ETimelineState NewState) const;

    // Stat calculation
    void RecalculateStats();
    float GetTotalStatModifier(EStatType StatType) const;

    // Effect tracking
    TArray<FActiveEffect> ActiveEffects;
    void CleanupExpiredEffects();
    void ApplyEffectModifiers(const FActiveEffect& Effect);
};
