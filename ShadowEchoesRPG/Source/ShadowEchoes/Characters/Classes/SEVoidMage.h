// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Classes/SECharacterClassBase.h"
#include "SEVoidMage.generated.h"

USTRUCT(BlueprintType)
struct FVoidState
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float VoidPower;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float VoidCorruption;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float VoidRegenRate;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float CorruptionThreshold;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float MaxVoidPower;
};

USTRUCT(BlueprintType)
struct FVoidAbility
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float VoidCost;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float CorruptionCost;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float Cooldown;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    float DamageMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Void")
    bool bRequiresDarkTimeline;
};

/**
 * Void Mage class specializing in dark timeline manipulation and void magic
 */
UCLASS()
class SHADOWECHOES_API USEVoidMage : public USECharacterClassBase
{
    GENERATED_BODY()

public:
    USEVoidMage();

    /** Void manipulation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    bool ChannelVoidPower();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    void StopChannelingVoid();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    bool CastVoidAbility(const FName& AbilityID);

    /** Corruption mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    float GetCurrentCorruption() const { return VoidState.VoidCorruption; }

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    bool IsCorrupted() const { return VoidState.VoidCorruption >= VoidState.CorruptionThreshold; }

    /** Timeline abilities */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    bool CreateVoidPortal(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Void Mage")
    void CloseVoidPortal();

    /** Overridden base class functions */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;
    virtual float ModifyDamage(float BaseDamage, ETimelineState DamageTimeline) override;
    virtual float ModifyDefense(float BaseDefense, ETimelineState AttackTimeline) override;

protected:
    /** Void settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    FVoidState VoidState;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    TMap<FName, FVoidAbility> VoidAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    float VoidChannelRate;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    float CorruptionRate;

    /** Portal settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    float PortalDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    float PortalRange;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Void Mage")
    float PortalVoidCost;

    /** State */
    UPROPERTY()
    bool bIsChannelingVoid;

    UPROPERTY()
    bool bHasActivePortal;

    UPROPERTY()
    FVector PortalLocation;

    UPROPERTY()
    float CurrentPortalTime;

    /** Internal functionality */
    virtual void UpdateVoidState();
    virtual void ProcessCorruption();
    virtual void UpdatePortalState();
    virtual bool ValidateVoidAbility(const FName& AbilityID) const;
    virtual float CalculateVoidPowerBonus() const;
    virtual void ApplyCorruptionEffects();

    /** Timeline-specific mechanics */
    float GetTimelineVoidModifier() const;
    bool CanUseVoidPower() const;
    void UpdateVoidEffects();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnVoidChannelStart();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnVoidChannelEnd();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnVoidAbilityCast(const FName& AbilityID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnCorruptionThresholdReached();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnPortalCreated(const FVector& Location);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Void Mage")
    void BP_OnPortalClosed();
};
