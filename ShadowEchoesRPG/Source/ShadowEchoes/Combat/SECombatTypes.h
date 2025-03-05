// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/SETypes.h"
#include "SECombatTypes.generated.h"

/** Combat stats */
USTRUCT(BlueprintType)
struct FCombatStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Attack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Defense;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CriticalChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CriticalDamage;

    FCombatStats()
        : Health(100.0f)
        , MaxHealth(100.0f)
        , Attack(10.0f)
        , Defense(5.0f)
        , CriticalChance(0.05f)
        , CriticalDamage(1.5f)
    {
    }
};

/** Equipment slots */
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    Weapon      UMETA(DisplayName = "Weapon"),
    Armor       UMETA(DisplayName = "Armor"),
    Accessory1  UMETA(DisplayName = "Accessory 1"),
    Accessory2  UMETA(DisplayName = "Accessory 2")
};

/** Equipment rarity */
UENUM(BlueprintType)
enum class EEquipmentRarity : uint8
{
    Common      UMETA(DisplayName = "Common"),
    Uncommon    UMETA(DisplayName = "Uncommon"),
    Rare        UMETA(DisplayName = "Rare"),
    Epic        UMETA(DisplayName = "Epic"),
    Legendary   UMETA(DisplayName = "Legendary")
};

/** Equipment info */
USTRUCT(BlueprintType)
struct FEquipmentInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FName EquipmentID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    EEquipmentSlot Slot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    EEquipmentRarity Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    FCombatStats Stats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
    USkeletalMesh* Mesh;

    FEquipmentInfo()
        : EquipmentID(NAME_None)
        , Slot(EEquipmentSlot::Weapon)
        , Rarity(EEquipmentRarity::Common)
        , Level(1)
        , Icon(nullptr)
        , Mesh(nullptr)
    {
    }
};

/** Ability types */
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
    Attack      UMETA(DisplayName = "Attack"),
    Skill       UMETA(DisplayName = "Skill"),
    Ultimate    UMETA(DisplayName = "Ultimate"),
    Passive     UMETA(DisplayName = "Passive")
};

/** Ability targeting types */
UENUM(BlueprintType)
enum class EAbilityTargetType : uint8
{
    Self        UMETA(DisplayName = "Self"),
    SingleTarget UMETA(DisplayName = "Single Target"),
    AOE         UMETA(DisplayName = "Area of Effect"),
    Projectile  UMETA(DisplayName = "Projectile")
};

/** Combo requirement */
USTRUCT(BlueprintType)
struct FComboRequirement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TArray<FName> RequiredAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float TimeWindow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 RequiredHits;

    FComboRequirement()
        : TimeWindow(2.0f)
        , RequiredHits(1)
    {
    }
};

/** Ability info */
USTRUCT(BlueprintType)
struct FAbilityInfo
{
    GENERATED_BODY()

    /** Basic info */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName AbilityID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FText Description;

    /** Type and targeting */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    EAbilityType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    EAbilityTargetType TargetType;

    /** Requirements */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    ETimelineState RequiredTimeline;

    /** Combat stats */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float EnergyCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cooldown;

    /** Combo system */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bCanStartCombo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FComboRequirement ComboRequirement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float ComboDamageMultiplier;

    /** Visual effects */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UAnimMontage* AnimMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UParticleSystem* VFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    USoundBase* SFX;

    FAbilityInfo()
        : AbilityID(NAME_None)
        , Type(EAbilityType::Attack)
        , TargetType(EAbilityTargetType::SingleTarget)
        , RequiredLevel(1)
        , RequiredTimeline(ETimelineState::Any)
        , Damage(10.0f)
        , EnergyCost(0.0f)
        , Cooldown(0.0f)
        , bCanStartCombo(false)
        , ComboDamageMultiplier(1.5f)
        , Icon(nullptr)
        , AnimMontage(nullptr)
        , VFX(nullptr)
        , SFX(nullptr)
    {
    }
};

/** Boss phase info */
USTRUCT(BlueprintType)
struct FBossPhaseInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText PhaseName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    float HealthThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FName> PhaseAbilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FCombatStats PhaseStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    ETimelineState PreferredTimeline;

    FBossPhaseInfo()
        : HealthThreshold(0.75f)
        , PreferredTimeline(ETimelineState::Any)
    {
    }
};

/** Upgrade info */
USTRUCT(BlueprintType)
struct FUpgradeInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FName UpgradeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    int32 MaxLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    int32 CostPerLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FCombatStats StatsPerLevel;

    FUpgradeInfo()
        : UpgradeID(NAME_None)
        , Level(0)
        , MaxLevel(10)
        , CostPerLevel(100)
    {
    }
};
