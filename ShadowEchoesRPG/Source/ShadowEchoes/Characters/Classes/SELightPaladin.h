// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Classes/SECharacterClassBase.h"
#include "SELightPaladin.generated.h"

USTRUCT(BlueprintType)
struct FLightState
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float LightPower;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float LightResonance;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float ResonanceThreshold;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float MaxLightPower;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float ResonanceDecayRate;
};

USTRUCT(BlueprintType)
struct FLightAbility
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float LightCost;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float ResonanceGain;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float Cooldown;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    float HealingMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Light")
    bool bRequiresBrightTimeline;
};

/**
 * Light Paladin class specializing in light timeline manipulation and holy magic
 */
UCLASS()
class SHADOWECHOES_API USELightPaladin : public USECharacterClassBase
{
    GENERATED_BODY()

public:
    USELightPaladin();

    /** Light manipulation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    bool ChannelLightPower();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    void StopChannelingLight();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    bool CastLightAbility(const FName& AbilityID);

    /** Resonance mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    float GetCurrentResonance() const { return LightState.LightResonance; }

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    bool IsResonating() const { return LightState.LightResonance >= LightState.ResonanceThreshold; }

    /** Timeline abilities */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    bool CreateLightBarrier(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    void DismissLightBarrier();

    /** Healing mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    float GetHealingPower() const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Light Paladin")
    bool AttemptHealing(AActor* Target);

    /** Overridden base class functions */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;
    virtual float ModifyDamage(float BaseDamage, ETimelineState DamageTimeline) override;
    virtual float ModifyDefense(float BaseDefense, ETimelineState AttackTimeline) override;

protected:
    /** Light settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    FLightState LightState;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    TMap<FName, FLightAbility> LightAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float LightChannelRate;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float ResonanceGainRate;

    /** Barrier settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float BarrierDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float BarrierRange;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float BarrierLightCost;

    /** Healing settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float BaseHealingPower;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float HealingRange;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Light Paladin")
    float HealingLightCost;

    /** State */
    UPROPERTY()
    bool bIsChannelingLight;

    UPROPERTY()
    bool bHasActiveBarrier;

    UPROPERTY()
    FVector BarrierLocation;

    UPROPERTY()
    float CurrentBarrierTime;

    /** Internal functionality */
    virtual void UpdateLightState();
    virtual void ProcessResonance();
    virtual void UpdateBarrierState();
    virtual bool ValidateLightAbility(const FName& AbilityID) const;
    virtual float CalculateLightPowerBonus() const;
    virtual void ApplyResonanceEffects();

    /** Timeline-specific mechanics */
    float GetTimelineLightModifier() const;
    bool CanUseLightPower() const;
    void UpdateLightEffects();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnLightChannelStart();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnLightChannelEnd();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnLightAbilityCast(const FName& AbilityID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnResonanceThresholdReached();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnBarrierCreated(const FVector& Location);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnBarrierDismissed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Light Paladin")
    void BP_OnHealingPerformed(AActor* Target, float Amount);
};
