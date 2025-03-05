// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SECharacterClassBase.generated.h"

class UTimelineManager;
class UAbilityComponent;

UENUM(BlueprintType)
enum class EClassSpecialization : uint8
{
    None            UMETA(DisplayName = "None"),
    TimelineWeaver  UMETA(DisplayName = "Timeline Weaver"),
    VoidWalker      UMETA(DisplayName = "Void Walker"),
    LightBringer    UMETA(DisplayName = "Light Bringer"),
    ChronoMaster    UMETA(DisplayName = "Chrono Master")
};

USTRUCT(BlueprintType)
struct FClassStats
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float Health;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float TimelineEnergy;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float StaminaRegen;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float TimelinePower;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float CriticalChance;

    UPROPERTY(EditDefaultsOnly, Category = "Stats")
    float DodgeChance;
};

USTRUCT(BlueprintType)
struct FTimelineAffinity
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    ETimelineState PreferredTimeline;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float TimelineEnergyEfficiency;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float TimelineDamageBonus;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline")
    float TimelineResistance;
};

/**
 * Base class for character classes with timeline mechanics
 */
UCLASS(Abstract, Blueprintable)
class SHADOWECHOES_API USECharacterClassBase : public UObject
{
    GENERATED_BODY()

public:
    USECharacterClassBase();

    /** Class initialization */
    virtual void Initialize(UTimelineManager* InTimelineManager, UAbilityComponent* InAbilityComponent);

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual void OnTimelineStateChanged(ETimelineState NewState);

    /** Stats and progression */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual void LevelUp();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual void UnlockSpecialization(EClassSpecialization Specialization);

    /** Combat mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual float ModifyDamage(float BaseDamage, ETimelineState DamageTimeline);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual float ModifyDefense(float BaseDefense, ETimelineState AttackTimeline);

    /** Timeline mechanics */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual float GetTimelineEnergyModifier() const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Class")
    virtual bool CanUseTimelineAbility(ETimelineState RequiredTimeline) const;

protected:
    /** Base stats */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Class")
    FClassStats BaseStats;

    /** Timeline affinity */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Class")
    FTimelineAffinity TimelineAffinity;

    /** Specialization */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Class")
    TArray<EClassSpecialization> AvailableSpecializations;

    UPROPERTY()
    EClassSpecialization CurrentSpecialization;

    /** References */
    UPROPERTY()
    UTimelineManager* TimelineManager;

    UPROPERTY()
    UAbilityComponent* AbilityComponent;

    /** Internal functionality */
    virtual void ApplyTimelineEffects(ETimelineState State);
    virtual void UpdateStats();
    virtual void ApplySpecializationBonuses();
    virtual float CalculateTimelineSynergy() const;

    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Class")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Class")
    void BP_OnLevelUp();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Class")
    void BP_OnSpecializationUnlocked(EClassSpecialization Specialization);
};
