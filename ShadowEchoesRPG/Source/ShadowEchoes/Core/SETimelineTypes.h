#pragma once

#include "CoreMinimal.h"
#include "SETimelineTypes.generated.h"

UENUM(BlueprintType)
enum class ETimelineState : uint8
{
    None        UMETA(DisplayName = "None"),
    Light       UMETA(DisplayName = "Light Timeline"),
    Dark        UMETA(DisplayName = "Dark Timeline"),
    Transition  UMETA(DisplayName = "Timeline Transition")
};

USTRUCT(BlueprintType)
struct FTimelineStats
{
    GENERATED_BODY()

    // Core stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float Energy = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float MaxEnergy = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float EnergyRegenRate = 1.0f;

    // Mastery levels
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float LightMastery = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float DarkMastery = 0.0f;

    // Timeline-specific bonuses
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float LightDamageBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float DarkDamageBonus = 0.0f;

    // Transition costs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float TransitionEnergyCost = 25.0f;
};

USTRUCT(BlueprintType)
struct FTimelineEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    FString EffectName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    float Power;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    ETimelineState RequiredState;

    // Visual effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    class UParticleSystem* VisualEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
    class USoundBase* SoundEffect;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimelineStateChanged, ETimelineState, NewState, ETimelineState, OldState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineEnergyChanged, float, NewEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineMasteryGained, float, NewMastery);
