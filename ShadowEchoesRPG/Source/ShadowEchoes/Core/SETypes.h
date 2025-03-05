// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SETypes.generated.h"

/** Timeline states */
UENUM(BlueprintType)
enum class ETimelineState : uint8
{
    BrightWorld UMETA(DisplayName = "Bright World"),
    DarkWorld   UMETA(DisplayName = "Dark World"),
    Any         UMETA(DisplayName = "Any Timeline")
};

/** Combat states */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
    OutOfCombat UMETA(DisplayName = "Out of Combat"),
    InCombat    UMETA(DisplayName = "In Combat"),
    Evading     UMETA(DisplayName = "Evading"),
    Stunned     UMETA(DisplayName = "Stunned")
};

/** Ability types */
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
    Active      UMETA(DisplayName = "Active"),
    Passive     UMETA(DisplayName = "Passive"),
    Ultimate    UMETA(DisplayName = "Ultimate")
};

/** Damage types */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical    UMETA(DisplayName = "Physical"),
    Timeline    UMETA(DisplayName = "Timeline"),
    Void        UMETA(DisplayName = "Void")
};

/** Quest types */
UENUM(BlueprintType)
enum class EQuestType : uint8
{
    MainQuest   UMETA(DisplayName = "Main Quest"),
    SideQuest   UMETA(DisplayName = "Side Quest"),
    BossQuest   UMETA(DisplayName = "Boss Quest")
};

/** Quest states */
UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted  UMETA(DisplayName = "Not Started"),
    InProgress  UMETA(DisplayName = "In Progress"),
    Completed   UMETA(DisplayName = "Completed"),
    Failed      UMETA(DisplayName = "Failed")
};

/** Ability information */
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UTexture2D* Icon;

    /** Type and requirements */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    EAbilityType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    ETimelineState RequiredTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    int32 RequiredLevel;

    /** Gameplay stats */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float EnergyCost;

    /** Effects */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UParticleSystem* CastEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UParticleSystem* ImpactEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    USoundBase* CastSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    USoundBase* ImpactSound;

    /** Animation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    UAnimMontage* CastMontage;

    FAbilityInfo()
        : AbilityID(NAME_None)
        , Icon(nullptr)
        , Type(EAbilityType::Active)
        , RequiredTimeline(ETimelineState::Any)
        , RequiredLevel(1)
        , Damage(0.0f)
        , Duration(0.0f)
        , Cooldown(0.0f)
        , EnergyCost(0.0f)
        , CastEffect(nullptr)
        , ImpactEffect(nullptr)
        , CastSound(nullptr)
        , ImpactSound(nullptr)
        , CastMontage(nullptr)
    {
    }
};

/** Quest objective information */
USTRUCT(BlueprintType)
struct FQuestObjectiveInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName ObjectiveID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsOptional;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    ETimelineState RequiredTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardCurrency;

    FQuestObjectiveInfo()
        : ObjectiveID(NAME_None)
        , bIsOptional(false)
        , RequiredTimeline(ETimelineState::Any)
        , RequiredLevel(1)
        , RewardXP(0)
        , RewardCurrency(0)
    {
    }
};

/** Quest information */
USTRUCT(BlueprintType)
struct FQuestInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EQuestType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    ETimelineState RequiredTimeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> PrerequisiteQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> ObjectiveIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardXP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardCurrency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> RewardItems;

    FQuestInfo()
        : QuestID(NAME_None)
        , Type(EQuestType::MainQuest)
        , RequiredTimeline(ETimelineState::Any)
        , RequiredLevel(1)
        , RewardXP(0)
        , RewardCurrency(0)
    {
    }
};

/** Boss fight information */
USTRUCT(BlueprintType)
struct FBossFightPhase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FName Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FName> Abilities;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    float HealthThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FString> Mechanics;

    FBossFightPhase()
        : Name(NAME_None)
        , HealthThreshold(1.0f)
    {
    }
};

USTRUCT(BlueprintType)
struct FBossFightInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FName BossID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    ETimelineState Timeline;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    TArray<FBossFightPhase> Phases;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
    FQuestInfo RewardQuest;

    FBossFightInfo()
        : BossID(NAME_None)
        , RequiredLevel(1)
        , Timeline(ETimelineState::Any)
    {
    }
};
