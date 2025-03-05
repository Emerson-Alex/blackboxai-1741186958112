// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Classes/SECharacterClassBase.h"
#include "SETimelineAssassin.generated.h"

USTRUCT(BlueprintType)
struct FAssassinationWindow
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Assassination")
    float Duration;

    UPROPERTY(EditDefaultsOnly, Category = "Assassination")
    float DamageMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Assassination")
    bool bIgnoreArmor;

    UPROPERTY(EditDefaultsOnly, Category = "Assassination")
    bool bGuaranteedCritical;
};

USTRUCT(BlueprintType)
struct FTimelinePhase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float Duration;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float StealthBonus;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float MovementSpeedBonus;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    bool bCanPhaseShift;
};

/**
 * Timeline Assassin class with stealth and assassination mechanics
 */
UCLASS()
class SHADOWECHOES_API USETimelineAssassin : public USECharacterClassBase
{
    GENERATED_BODY()

public:
    USETimelineAssassin();

    /** Stealth mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    bool AttemptStealth();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    void BreakStealth();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    bool IsInStealth() const { return bIsStealthed; }

    /** Assassination mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    bool CanAssassinate(AActor* Target) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    float GetAssassinationDamage(float BaseDamage) const;

    /** Timeline abilities */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    bool AttemptPhaseShift();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Assassin")
    void EndPhaseShift();

    /** Overridden base class functions */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;
    virtual float ModifyDamage(float BaseDamage, ETimelineState DamageTimeline) override;
    virtual float ModifyDefense(float BaseDefense, ETimelineState AttackTimeline) override;

protected:
    /** Stealth settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float BaseStealthDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float StealthDetectionRadius;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float StealthMovementPenalty;

    /** Assassination settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    FAssassinationWindow AssassinationWindow;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float AssassinationRange;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float BackstabAngle;

    /** Timeline abilities */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    FTimelinePhase PhaseShiftSettings;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Assassin")
    float TimelineShadowDuration;

    /** State */
    UPROPERTY()
    bool bIsStealthed;

    UPROPERTY()
    bool bIsPhaseShifted;

    UPROPERTY()
    float CurrentStealthTime;

    UPROPERTY()
    float CurrentPhaseTime;

    /** Internal functionality */
    virtual void UpdateStealthState();
    virtual void UpdatePhaseState();
    virtual bool ValidateAssassinationTarget(AActor* Target) const;
    virtual float CalculateStealthBonus() const;
    virtual void ApplyTimelineShadowEffects();

    /** Timeline-specific mechanics */
    float GetTimelineStealthModifier() const;
    bool CanUseTimelineShadow() const;
    void UpdateTimelineShadowEffects();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Assassin")
    void BP_OnStealthEnter();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Assassin")
    void BP_OnStealthExit();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Assassin")
    void BP_OnAssassination(AActor* Target);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Assassin")
    void BP_OnPhaseShiftStart();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Assassin")
    void BP_OnPhaseShiftEnd();
};
