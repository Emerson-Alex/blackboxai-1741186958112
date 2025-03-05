// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Characters/Classes/SEVoidMage.h"
#include "Systems/TimelineManager.h"
#include "Combat/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USEVoidMage::USEVoidMage()
{
    // Base class settings
    BaseStats.Health = 700.0f;         // Lower health due to ranged focus
    BaseStats.TimelineEnergy = 200.0f; // High timeline energy
    BaseStats.StaminaRegen = 8.0f;     // Lower stamina regen
    BaseStats.TimelinePower = 80.0f;   // Very high timeline power
    BaseStats.CriticalChance = 0.08f;  // Moderate crit
    BaseStats.DodgeChance = 0.05f;     // Low dodge

    // Timeline affinity
    TimelineAffinity.PreferredTimeline = ETimelineState::DarkWorld;
    TimelineAffinity.TimelineEnergyEfficiency = 1.5f;
    TimelineAffinity.TimelineDamageBonus = 1.4f;
    TimelineAffinity.TimelineResistance = 0.7f;  // Very vulnerable

    // Void settings
    VoidState.VoidPower = 0.0f;
    VoidState.VoidCorruption = 0.0f;
    VoidState.VoidRegenRate = 5.0f;
    VoidState.CorruptionThreshold = 100.0f;
    VoidState.MaxVoidPower = 200.0f;

    VoidChannelRate = 10.0f;
    CorruptionRate = 5.0f;

    // Portal settings
    PortalDuration = 15.0f;
    PortalRange = 1000.0f;
    PortalVoidCost = 50.0f;

    // Initialize void abilities
    // FromSoftware-style: High risk, high reward abilities
    FVoidAbility VoidBlast;
    VoidBlast.VoidCost = 30.0f;
    VoidBlast.CorruptionCost = 10.0f;
    VoidBlast.Cooldown = 3.0f;
    VoidBlast.DamageMultiplier = 2.0f;
    VoidBlast.bRequiresDarkTimeline = false;
    VoidAbilities.Add("VoidBlast", VoidBlast);

    FVoidAbility VoidEruption;
    VoidEruption.VoidCost = 80.0f;
    VoidEruption.CorruptionCost = 30.0f;
    VoidEruption.Cooldown = 15.0f;
    VoidEruption.DamageMultiplier = 4.0f;
    VoidEruption.bRequiresDarkTimeline = true;
    VoidAbilities.Add("VoidEruption", VoidEruption);

    // State initialization
    bIsChannelingVoid = false;
    bHasActivePortal = false;
    CurrentPortalTime = 0.0f;
}

bool USEVoidMage::ChannelVoidPower()
{
    // FromSoftware-style: Risk/reward channeling
    if (bIsChannelingVoid || !AbilityComponent)
    {
        return false;
    }

    // Check timeline energy
    float EnergyCost = 20.0f * GetTimelineEnergyModifier();
    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    bIsChannelingVoid = true;
    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify channel start
    BP_OnVoidChannelStart();

    return true;
}

void USEVoidMage::StopChannelingVoid()
{
    if (!bIsChannelingVoid)
    {
        return;
    }

    bIsChannelingVoid = false;

    // Notify channel end
    BP_OnVoidChannelEnd();
}

bool USEVoidMage::CastVoidAbility(const FName& AbilityID)
{
    // FromSoftware-style: Strict casting requirements
    if (!ValidateVoidAbility(AbilityID))
    {
        return false;
    }

    const FVoidAbility& Ability = VoidAbilities[AbilityID];

    // Check void power
    if (VoidState.VoidPower < Ability.VoidCost)
    {
        return false;
    }

    // Check timeline state
    if (Ability.bRequiresDarkTimeline)
    {
        if (!TimelineManager || TimelineManager->GetCurrentState() != ETimelineState::DarkWorld)
        {
            return false;
        }
    }

    // Apply costs
    VoidState.VoidPower -= Ability.VoidCost;
    VoidState.VoidCorruption += Ability.CorruptionCost;

    // Check corruption threshold
    if (IsCorrupted())
    {
        ApplyCorruptionEffects();
    }

    // Notify ability cast
    BP_OnVoidAbilityCast(AbilityID);

    return true;
}

bool USEVoidMage::CreateVoidPortal(const FVector& Location)
{
    // FromSoftware-style: Strategic portal placement
    if (bHasActivePortal || !AbilityComponent)
    {
        return false;
    }

    // Check void power
    if (VoidState.VoidPower < PortalVoidCost)
    {
        return false;
    }

    // Check range
    float Distance = FVector::Distance(AbilityComponent->GetOwner()->GetActorLocation(), Location);
    if (Distance > PortalRange)
    {
        return false;
    }

    // Create portal
    bHasActivePortal = true;
    PortalLocation = Location;
    CurrentPortalTime = PortalDuration;
    VoidState.VoidPower -= PortalVoidCost;

    // Notify portal creation
    BP_OnPortalCreated(Location);

    return true;
}

void USEVoidMage::CloseVoidPortal()
{
    if (!bHasActivePortal)
    {
        return;
    }

    bHasActivePortal = false;
    CurrentPortalTime = 0.0f;

    // Notify portal closure
    BP_OnPortalClosed();
}

void USEVoidMage::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // FromSoftware-style: Timeline affects void power
    UpdateVoidEffects();

    // Check corruption effects
    if (IsCorrupted() && NewState == ETimelineState::DarkWorld)
    {
        ApplyCorruptionEffects();
    }
}

float USEVoidMage::ModifyDamage(float BaseDamage, ETimelineState DamageTimeline)
{
    float FinalDamage = Super::ModifyDamage(BaseDamage, DamageTimeline);

    // FromSoftware-style: Corruption increases damage but at a cost
    if (IsCorrupted())
    {
        FinalDamage *= 1.5f;
    }

    // Void power bonus
    float VoidBonus = CalculateVoidPowerBonus();
    FinalDamage *= (1.0f + VoidBonus);

    return FinalDamage;
}

float USEVoidMage::ModifyDefense(float BaseDefense, ETimelineState AttackTimeline)
{
    float FinalDefense = Super::ModifyDefense(BaseDefense, AttackTimeline);

    // FromSoftware-style: Corruption decreases defense
    if (IsCorrupted())
    {
        FinalDefense *= 0.7f;
    }

    // More vulnerable while channeling
    if (bIsChannelingVoid)
    {
        FinalDefense *= 0.8f;
    }

    return FinalDefense;
}

void USEVoidMage::UpdateVoidState()
{
    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

    // Update void power
    if (bIsChannelingVoid)
    {
        VoidState.VoidPower = FMath::Min(VoidState.VoidPower + VoidChannelRate * DeltaTime, VoidState.MaxVoidPower);
        VoidState.VoidCorruption += CorruptionRate * DeltaTime;
    }
    else
    {
        VoidState.VoidPower = FMath::Max(0.0f, VoidState.VoidPower - VoidState.VoidRegenRate * DeltaTime);
    }

    // Process corruption
    ProcessCorruption();

    // Update portal
    UpdatePortalState();
}

void USEVoidMage::ProcessCorruption()
{
    // FromSoftware-style: Corruption management
    if (IsCorrupted())
    {
        // Notify first time corruption threshold is reached
        static bool bHasNotifiedCorruption = false;
        if (!bHasNotifiedCorruption)
        {
            BP_OnCorruptionThresholdReached();
            bHasNotifiedCorruption = true;
        }

        // Apply corruption damage over time
        if (AbilityComponent)
        {
            float CorruptionDamage = VoidState.VoidCorruption * 0.1f * UGameplayStatics::GetWorldDeltaSeconds(this);
            AbilityComponent->ApplyDamage(CorruptionDamage, ETimelineState::DarkWorld);
        }
    }
}

void USEVoidMage::UpdatePortalState()
{
    if (!bHasActivePortal)
    {
        return;
    }

    CurrentPortalTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (CurrentPortalTime <= 0.0f)
    {
        CloseVoidPortal();
    }
}

bool USEVoidMage::ValidateVoidAbility(const FName& AbilityID) const
{
    if (!VoidAbilities.Contains(AbilityID))
    {
        return false;
    }

    const FVoidAbility& Ability = VoidAbilities[AbilityID];

    // Check timeline requirements
    if (Ability.bRequiresDarkTimeline)
    {
        if (!TimelineManager || TimelineManager->GetCurrentState() != ETimelineState::DarkWorld)
        {
            return false;
        }
    }

    return true;
}

float USEVoidMage::CalculateVoidPowerBonus() const
{
    // FromSoftware-style: Power scaling with risk
    float Bonus = VoidState.VoidPower / VoidState.MaxVoidPower;
    
    // Corruption bonus
    if (IsCorrupted())
    {
        Bonus *= 1.5f;
    }

    // Timeline bonus
    if (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::DarkWorld)
    {
        Bonus *= 1.3f;
    }

    return Bonus;
}

void USEVoidMage::ApplyCorruptionEffects()
{
    // FromSoftware-style: Corruption has severe consequences
    if (!IsCorrupted())
    {
        return;
    }

    // Increase void power but at a cost
    VoidState.MaxVoidPower *= 1.2f;
    BaseStats.Health *= 0.8f;

    // Apply visual effects
    // This would be handled in blueprints
}

float USEVoidMage::GetTimelineVoidModifier() const
{
    if (!TimelineManager)
    {
        return 1.0f;
    }

    switch (TimelineManager->GetCurrentState())
    {
        case ETimelineState::DarkWorld:
            return 1.5f;  // Enhanced void power in dark timeline

        case ETimelineState::BrightWorld:
            return 0.7f;  // Reduced void power in light timeline

        default:
            return 1.0f;
    }
}

bool USEVoidMage::CanUseVoidPower() const
{
    return !IsCorrupted() || (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::DarkWorld);
}

void USEVoidMage::UpdateVoidEffects()
{
    // FromSoftware-style: Dynamic ability adjustments
    if (!TimelineManager)
    {
        return;
    }

    // Adjust abilities based on timeline
    switch (TimelineManager->GetCurrentState())
    {
        case ETimelineState::DarkWorld:
            VoidChannelRate *= 1.5f;
            CorruptionRate *= 0.7f;
            break;

        case ETimelineState::BrightWorld:
            VoidChannelRate *= 0.7f;
            CorruptionRate *= 1.5f;
            break;

        default:
            VoidChannelRate = 10.0f;
            CorruptionRate = 5.0f;
            break;
    }
}
