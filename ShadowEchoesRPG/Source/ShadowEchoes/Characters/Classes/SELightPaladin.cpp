// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Characters/Classes/SELightPaladin.h"
#include "Systems/TimelineManager.h"
#include "Combat/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USELightPaladin::USELightPaladin()
{
    // Base class settings
    BaseStats.Health = 1200.0f;        // High health for tanking
    BaseStats.TimelineEnergy = 150.0f; // Moderate timeline energy
    BaseStats.StaminaRegen = 12.0f;    // High stamina regen
    BaseStats.TimelinePower = 70.0f;   // High timeline power
    BaseStats.CriticalChance = 0.05f;  // Low crit
    BaseStats.DodgeChance = 0.08f;     // Moderate dodge

    // Timeline affinity
    TimelineAffinity.PreferredTimeline = ETimelineState::BrightWorld;
    TimelineAffinity.TimelineEnergyEfficiency = 1.3f;
    TimelineAffinity.TimelineDamageBonus = 1.2f;
    TimelineAffinity.TimelineResistance = 1.4f;  // High resistance

    // Light settings
    LightState.LightPower = 0.0f;
    LightState.LightResonance = 0.0f;
    LightState.ResonanceThreshold = 100.0f;
    LightState.MaxLightPower = 200.0f;
    LightState.ResonanceDecayRate = 5.0f;

    LightChannelRate = 10.0f;
    ResonanceGainRate = 5.0f;

    // Barrier settings
    BarrierDuration = 20.0f;
    BarrierRange = 800.0f;
    BarrierLightCost = 50.0f;

    // Healing settings
    BaseHealingPower = 50.0f;
    HealingRange = 500.0f;
    HealingLightCost = 30.0f;

    // Initialize light abilities
    // FromSoftware-style: Strategic healing and protection abilities
    FLightAbility LightBlessing;
    LightBlessing.LightCost = 30.0f;
    LightBlessing.ResonanceGain = 10.0f;
    LightBlessing.Cooldown = 5.0f;
    LightBlessing.HealingMultiplier = 1.5f;
    LightBlessing.bRequiresBrightTimeline = false;
    LightAbilities.Add("LightBlessing", LightBlessing);

    FLightAbility DivineProtection;
    DivineProtection.LightCost = 80.0f;
    DivineProtection.ResonanceGain = 30.0f;
    DivineProtection.Cooldown = 20.0f;
    DivineProtection.HealingMultiplier = 2.0f;
    DivineProtection.bRequiresBrightTimeline = true;
    LightAbilities.Add("DivineProtection", DivineProtection);

    // State initialization
    bIsChannelingLight = false;
    bHasActiveBarrier = false;
    CurrentBarrierTime = 0.0f;
}

bool USELightPaladin::ChannelLightPower()
{
    // FromSoftware-style: Strategic resource management
    if (bIsChannelingLight || !AbilityComponent)
    {
        return false;
    }

    // Check timeline energy
    float EnergyCost = 20.0f * GetTimelineEnergyModifier();
    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    bIsChannelingLight = true;
    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify channel start
    BP_OnLightChannelStart();

    return true;
}

void USELightPaladin::StopChannelingLight()
{
    if (!bIsChannelingLight)
    {
        return;
    }

    bIsChannelingLight = false;

    // Notify channel end
    BP_OnLightChannelEnd();
}

bool USELightPaladin::CastLightAbility(const FName& AbilityID)
{
    // FromSoftware-style: Precise timing and positioning
    if (!ValidateLightAbility(AbilityID))
    {
        return false;
    }

    const FLightAbility& Ability = LightAbilities[AbilityID];

    // Check light power
    if (LightState.LightPower < Ability.LightCost)
    {
        return false;
    }

    // Check timeline state
    if (Ability.bRequiresBrightTimeline)
    {
        if (!TimelineManager || TimelineManager->GetCurrentState() != ETimelineState::BrightWorld)
        {
            return false;
        }
    }

    // Apply costs and gains
    LightState.LightPower -= Ability.LightCost;
    LightState.LightResonance += Ability.ResonanceGain;

    // Check resonance threshold
    if (IsResonating())
    {
        ApplyResonanceEffects();
    }

    // Notify ability cast
    BP_OnLightAbilityCast(AbilityID);

    return true;
}

bool USELightPaladin::CreateLightBarrier(const FVector& Location)
{
    // FromSoftware-style: Strategic barrier placement
    if (bHasActiveBarrier || !AbilityComponent)
    {
        return false;
    }

    // Check light power
    if (LightState.LightPower < BarrierLightCost)
    {
        return false;
    }

    // Check range
    float Distance = FVector::Distance(AbilityComponent->GetOwner()->GetActorLocation(), Location);
    if (Distance > BarrierRange)
    {
        return false;
    }

    // Create barrier
    bHasActiveBarrier = true;
    BarrierLocation = Location;
    CurrentBarrierTime = BarrierDuration;
    LightState.LightPower -= BarrierLightCost;

    // Notify barrier creation
    BP_OnBarrierCreated(Location);

    return true;
}

void USELightPaladin::DismissLightBarrier()
{
    if (!bHasActiveBarrier)
    {
        return;
    }

    bHasActiveBarrier = false;
    CurrentBarrierTime = 0.0f;

    // Notify barrier dismissal
    BP_OnBarrierDismissed();
}

float USELightPaladin::GetHealingPower() const
{
    // FromSoftware-style: Complex healing calculations
    float HealingPower = BaseHealingPower;

    // Light power bonus
    float LightBonus = CalculateLightPowerBonus();
    HealingPower *= (1.0f + LightBonus);

    // Resonance bonus
    if (IsResonating())
    {
        HealingPower *= 1.5f;
    }

    // Timeline bonus
    if (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::BrightWorld)
    {
        HealingPower *= 1.3f;
    }

    return HealingPower;
}

bool USELightPaladin::AttemptHealing(AActor* Target)
{
    if (!Target || !AbilityComponent)
    {
        return false;
    }

    // Check light power
    if (LightState.LightPower < HealingLightCost)
    {
        return false;
    }

    // Check range
    float Distance = FVector::Distance(AbilityComponent->GetOwner()->GetActorLocation(), Target->GetActorLocation());
    if (Distance > HealingRange)
    {
        return false;
    }

    // Apply healing
    float HealAmount = GetHealingPower();
    // This would be handled by the target's health component
    
    // Consume light power
    LightState.LightPower -= HealingLightCost;

    // Notify healing
    BP_OnHealingPerformed(Target, HealAmount);

    return true;
}

void USELightPaladin::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // FromSoftware-style: Timeline affects abilities
    UpdateLightEffects();

    // Check resonance effects
    if (IsResonating() && NewState == ETimelineState::BrightWorld)
    {
        ApplyResonanceEffects();
    }
}

float USELightPaladin::ModifyDamage(float BaseDamage, ETimelineState DamageTimeline)
{
    float FinalDamage = Super::ModifyDamage(BaseDamage, DamageTimeline);

    // FromSoftware-style: Light empowers damage against darkness
    if (DamageTimeline == ETimelineState::DarkWorld)
    {
        if (IsResonating())
        {
            FinalDamage *= 1.5f;
        }
    }

    // Light power bonus
    float LightBonus = CalculateLightPowerBonus();
    FinalDamage *= (1.0f + LightBonus);

    return FinalDamage;
}

float USELightPaladin::ModifyDefense(float BaseDefense, ETimelineState AttackTimeline)
{
    float FinalDefense = Super::ModifyDefense(BaseDefense, AttackTimeline);

    // FromSoftware-style: Strong defense against dark attacks
    if (AttackTimeline == ETimelineState::DarkWorld)
    {
        if (IsResonating())
        {
            FinalDefense *= 1.5f;
        }
    }

    // Active barrier bonus
    if (bHasActiveBarrier)
    {
        FinalDefense *= 1.3f;
    }

    return FinalDefense;
}

void USELightPaladin::UpdateLightState()
{
    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

    // Update light power
    if (bIsChannelingLight)
    {
        LightState.LightPower = FMath::Min(LightState.LightPower + LightChannelRate * DeltaTime, LightState.MaxLightPower);
        LightState.LightResonance += ResonanceGainRate * DeltaTime;
    }
    else
    {
        // Natural decay
        LightState.LightResonance = FMath::Max(0.0f, LightState.LightResonance - LightState.ResonanceDecayRate * DeltaTime);
    }

    // Process resonance
    ProcessResonance();

    // Update barrier
    UpdateBarrierState();
}

void USELightPaladin::ProcessResonance()
{
    // FromSoftware-style: Resonance management
    if (IsResonating())
    {
        // Notify first time resonance threshold is reached
        static bool bHasNotifiedResonance = false;
        if (!bHasNotifiedResonance)
        {
            BP_OnResonanceThresholdReached();
            bHasNotifiedResonance = true;
        }

        // Apply resonance healing
        if (AbilityComponent)
        {
            float ResonanceHealing = GetHealingPower() * 0.1f * UGameplayStatics::GetWorldDeltaSeconds(this);
            // Apply healing to self
        }
    }
}

void USELightPaladin::UpdateBarrierState()
{
    if (!bHasActiveBarrier)
    {
        return;
    }

    CurrentBarrierTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (CurrentBarrierTime <= 0.0f)
    {
        DismissLightBarrier();
    }
}

bool USELightPaladin::ValidateLightAbility(const FName& AbilityID) const
{
    if (!LightAbilities.Contains(AbilityID))
    {
        return false;
    }

    const FLightAbility& Ability = LightAbilities[AbilityID];

    // Check timeline requirements
    if (Ability.bRequiresBrightTimeline)
    {
        if (!TimelineManager || TimelineManager->GetCurrentState() != ETimelineState::BrightWorld)
        {
            return false;
        }
    }

    return true;
}

float USELightPaladin::CalculateLightPowerBonus() const
{
    // FromSoftware-style: Power scaling with resonance
    float Bonus = LightState.LightPower / LightState.MaxLightPower;
    
    // Resonance bonus
    if (IsResonating())
    {
        Bonus *= 1.5f;
    }

    // Timeline bonus
    if (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::BrightWorld)
    {
        Bonus *= 1.3f;
    }

    return Bonus;
}

void USELightPaladin::ApplyResonanceEffects()
{
    // FromSoftware-style: Resonance enhances all abilities
    if (!IsResonating())
    {
        return;
    }

    // Enhance healing and protection
    BaseHealingPower *= 1.3f;
    if (bHasActiveBarrier)
    {
        CurrentBarrierTime += 5.0f;  // Extend barrier duration
    }

    // Apply visual effects
    // This would be handled in blueprints
}

float USELightPaladin::GetTimelineLightModifier() const
{
    if (!TimelineManager)
    {
        return 1.0f;
    }

    switch (TimelineManager->GetCurrentState())
    {
        case ETimelineState::BrightWorld:
            return 1.5f;  // Enhanced light power in bright timeline

        case ETimelineState::DarkWorld:
            return 0.7f;  // Reduced light power in dark timeline

        default:
            return 1.0f;
    }
}

bool USELightPaladin::CanUseLightPower() const
{
    return !IsResonating() || (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::BrightWorld);
}

void USELightPaladin::UpdateLightEffects()
{
    // FromSoftware-style: Dynamic ability adjustments
    if (!TimelineManager)
    {
        return;
    }

    // Adjust abilities based on timeline
    switch (TimelineManager->GetCurrentState())
    {
        case ETimelineState::BrightWorld:
            LightChannelRate *= 1.5f;
            BaseHealingPower *= 1.3f;
            break;

        case ETimelineState::DarkWorld:
            LightChannelRate *= 0.7f;
            BaseHealingPower *= 0.7f;
            break;

        default:
            LightChannelRate = 10.0f;
            BaseHealingPower = 50.0f;
            break;
    }
}
