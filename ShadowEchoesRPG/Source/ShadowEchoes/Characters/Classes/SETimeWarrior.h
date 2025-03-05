// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Classes/SECharacterClassBase.h"
#include "SETimeWarrior.generated.h"

USTRUCT(BlueprintType)
struct FTimelineStance
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float DamageMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float DefenseMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float SpeedMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float TimelineEnergyRate;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    bool bCanTimeShift;
};

USTRUCT(BlueprintType)
struct FTimelineCombo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TArray<ETimelineState> ComboSequence;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float DamageMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float TimelineEnergyCost;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float CooldownDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    bool bRequiresTimeShift;
};

/**
 * Time Warrior class specializing in timeline combat and stance-based fighting
 */
UCLASS()
class SHADOWECHOES_API USETimeWarrior : public USECharacterClassBase
{
    GENERATED_BODY()

public:
    USETimeWarrior();

    /** Stance management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    bool SwitchStance(ETimelineState NewStance);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    ETimelineState GetCurrentStance() const { return CurrentStance; }

    /** Combat mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    bool ExecuteCombo(const FName& ComboID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    void ResetCombo();

    /** Timeline abilities */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    bool TimeShift(ETimelineState TargetTimeline);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    void EndTimeShift();

    /** Stance-specific abilities */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    bool ExecuteStanceAbility();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Time Warrior")
    float GetStanceBonus() const;

    /** Overridden base class functions */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;
    virtual float ModifyDamage(float BaseDamage, ETimelineState DamageTimeline) override;
    virtual float ModifyDefense(float BaseDefense, ETimelineState AttackTimeline) override;

protected:
    /** Stance settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    TMap<ETimelineState, FTimelineStance> Stances;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    float StanceSwitchCooldown;

    /** Combat settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    TMap<FName, FTimelineCombo> Combos;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    float ComboWindowDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    int32 MaxComboLength;

    /** Timeline abilities */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    float TimeShiftDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    float TimeShiftCooldown;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Time Warrior")
    float TimeShiftEnergyCost;

    /** State */
    UPROPERTY()
    ETimelineState CurrentStance;

    UPROPERTY()
    TArray<ETimelineState> CurrentCombo;

    UPROPERTY()
    float ComboTimeRemaining;

    UPROPERTY()
    bool bIsTimeShifted;

    UPROPERTY()
    float CurrentTimeShiftTime;

    UPROPERTY()
    float LastStanceSwitchTime;

    /** Internal functionality */
    virtual void UpdateStanceEffects();
    virtual void ProcessComboWindow();
    virtual void UpdateTimeShift();
    virtual bool ValidateCombo(const FName& ComboID) const;
    virtual float CalculateComboBonus() const;
    virtual void ApplyStanceEffects();

    /** Timeline-specific mechanics */
    float GetTimelineStanceModifier() const;
    bool CanUseTimeShift() const;
    void UpdateTimelineEffects();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnStanceChanged(ETimelineState NewStance);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnComboExecuted(const FName& ComboID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnComboReset();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnTimeShiftStart(ETimelineState TargetTimeline);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnTimeShiftEnd();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Time Warrior")
    void BP_OnStanceAbilityUsed();
};
