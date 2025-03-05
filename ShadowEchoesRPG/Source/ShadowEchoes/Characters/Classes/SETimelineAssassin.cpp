// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Characters/Classes/SETimelineAssassin.h"
#include "Systems/TimelineManager.h"
#include "Combat/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USETimelineAssassin::USETimelineAssassin()
{
    // Base class settings
    BaseStats.Health = 800.0f;         // Lower base health
    BaseStats.TimelineEnergy = 150.0f; // Higher timeline energy
    BaseStats.StaminaRegen = 15.0f;    // Faster stamina regen
    BaseStats.TimelinePower = 60.0f;   // Higher timeline power
    BaseStats.CriticalChance = 0.15f;  // High base crit
    BaseStats.DodgeChance = 0.10f;     // High base dodge

    // Timeline affinity
    TimelineAffinity.PreferredTimeline = ETimelineState::DarkWorld;
    TimelineAffinity.TimelineEnergyEfficiency = 1.2f;
    TimelineAffinity.TimelineDamageBonus = 1.3f;
    TimelineAffinity.TimelineResistance = 0.8f;  // Lower resistance

    // Stealth settings
    BaseStealthDuration = 30.0f;
    StealthDetectionRadius = 300.0f;
    StealthMovementPenalty = 0.7f;

    // Assassination settings
    AssassinationWindow.Duration = 2.0f;
    AssassinationWindow.DamageMultiplier = 5.0f;
    AssassinationWindow.bIgnoreArmor = true;
    AssassinationWindow.bGuaranteedCritical = true;
    AssassinationRange = 200.0f;
    BackstabAngle = 60.0f;

    // Timeline abilities
    PhaseShiftSettings.Duration = 5.0f;
    PhaseShiftSettings.StealthBonus = 0.5f;
    PhaseShiftSettings.MovementSpeedBonus = 1.3f;
    PhaseShiftSettings.bCanPhaseShift = true;
    TimelineShadowDuration = 8.0f;

    // State initialization
    bIsStealthed = false;
    bIsPhaseShifted = false;
    CurrentStealthTime = 0.0f;
    CurrentPhaseTime = 0.0f;
}

bool USETimelineAssassin::AttemptStealth()
{
    // FromSoftware-style: Stealth requires careful timing and positioning
    if (bIsStealthed || !AbilityComponent)
    {
        return false;
    }

    // Check timeline energy cost
    float EnergyCost = 30.0f * GetTimelineEnergyModifier();
    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    // Apply stealth
    bIsStealthed = true;
    CurrentStealthTime = BaseStealthDuration * (1.0f + CalculateStealthBonus());
    
    // Apply movement penalties
    if (AbilityComponent)
    {
        AbilityComponent->SetMovementSpeedModifier(StealthMovementPenalty);
    }

    // Consume timeline energy
    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify stealth enter
    BP_OnStealthEnter();

    return true;
}

void USETimelineAssassin::BreakStealth()
{
    if (!bIsStealthed)
    {
        return;
    }

    bIsStealthed = false;
    CurrentStealthTime = 0.0f;

    // Reset movement speed
    if (AbilityComponent)
    {
        AbilityComponent->ResetMovementSpeedModifier();
    }

    // Notify stealth exit
    BP_OnStealthExit();
}

bool USETimelineAssassin::CanAssassinate(AActor* Target) const
{
    if (!Target || !bIsStealthed || !AbilityComponent)
    {
        return false;
    }

    // FromSoftware-style: Strict assassination requirements
    if (!ValidateAssassinationTarget(Target))
    {
        return false;
    }

    // Check distance
    float Distance = FVector::Distance(AbilityComponent->GetOwner()->GetActorLocation(), Target->GetActorLocation());
    if (Distance > AssassinationRange)
    {
        return false;
    }

    // Check angle (backstab)
    FVector ToTarget = Target->GetActorLocation() - AbilityComponent->GetOwner()->GetActorLocation();
    ToTarget.Normalize();
    float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToTarget, Target->GetActorForwardVector())));
    
    return Angle <= BackstabAngle;
}

float USETimelineAssassin::GetAssassinationDamage(float BaseDamage) const
{
    // FromSoftware-style: High risk, high reward damage calculation
    float FinalDamage = BaseDamage * AssassinationWindow.DamageMultiplier;

    // Timeline bonuses
    if (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::DarkWorld)
    {
        FinalDamage *= 1.5f;  // Additional damage in dark timeline
    }

    // Phase shift bonus
    if (bIsPhaseShifted)
    {
        FinalDamage *= 1.3f;
    }

    return FinalDamage;
}

bool USETimelineAssassin::AttemptPhaseShift()
{
    // FromSoftware-style: Timeline abilities require mastery
    if (bIsPhaseShifted || !AbilityComponent || !PhaseShiftSettings.bCanPhaseShift)
    {
        return false;
    }

    // Check timeline energy
    float EnergyCost = 50.0f * GetTimelineEnergyModifier();
    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    // Apply phase shift
    bIsPhaseShifted = true;
    CurrentPhaseTime = PhaseShiftSettings.Duration;

    // Apply bonuses
    if (AbilityComponent)
    {
        AbilityComponent->SetMovementSpeedModifier(PhaseShiftSettings.MovementSpeedBonus);
    }

    // Consume timeline energy
    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify phase shift start
    BP_OnPhaseShiftStart();

    return true;
}

void USETimelineAssassin::EndPhaseShift()
{
    if (!bIsPhaseShifted)
    {
        return;
    }

    bIsPhaseShifted = false;
    CurrentPhaseTime = 0.0f;

    // Reset movement speed
    if (AbilityComponent)
    {
        AbilityComponent->ResetMovementSpeedModifier();
    }

    // Notify phase shift end
    BP_OnPhaseShiftEnd();
}

void USETimelineAssassin::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // FromSoftware-style: Timeline changes affect abilities
    UpdateStealthState();
    UpdatePhaseState();
    UpdateTimelineShadowEffects();
}

float USETimelineAssassin::ModifyDamage(float BaseDamage, ETimelineState DamageTimeline)
{
    float FinalDamage = Super::ModifyDamage(BaseDamage, DamageTimeline);

    // FromSoftware-style: Complex damage calculations
    if (bIsStealthed)
    {
        FinalDamage *= 2.0f;  // Stealth damage bonus
    }

    if (bIsPhaseShifted)
    {
        FinalDamage *= 1.5f;  // Phase shift damage bonus
    }

    // Timeline synergy
    if (TimelineManager && TimelineManager->GetCurrentState() == ETimelineState::DarkWorld)
    {
        if (DamageTimeline == ETimelineState::DarkWorld)
        {
            FinalDamage *= 1.3f;  // Dark timeline synergy
        }
    }

    return FinalDamage;
}

float USETimelineAssassin::ModifyDefense(float BaseDefense, ETimelineState AttackTimeline)
{
    float FinalDefense = Super::ModifyDefense(BaseDefense, AttackTimeline);

    // FromSoftware-style: High risk, low defense
    if (bIsStealthed)
    {
        FinalDefense *= 0.7f;  // More vulnerable in stealth
    }

    if (bIsPhaseShifted)
    {
        FinalDefense *= 0.8f;  // More vulnerable during phase shift
    }

    return FinalDefense;
}

void USETimelineAssassin::UpdateStealthState()
{
    if (!bIsStealthed)
    {
        return;
    }

    // Update stealth duration
    CurrentStealthTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (CurrentStealthTime <= 0.0f)
    {
        BreakStealth();
        return;
    }

    // Check for nearby enemies
    // This would be implemented with actual enemy detection logic
}

void USETimelineAssassin::UpdatePhaseState()
{
    if (!bIsPhaseShifted)
    {
        return;
    }

    // Update phase shift duration
    CurrentPhaseTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (CurrentPhaseTime <= 0.0f)
    {
        EndPhaseShift();
    }
}

bool USETimelineAssassin::ValidateAssassinationTarget(AActor* Target) const
{
    if (!Target)
    {
        return false;
    }

    // FromSoftware-style: Not all enemies can be assassinated
    // This would check for boss status, size, etc.

    return true;
}

float USETimelineAssassin::CalculateStealthBonus() const
{
    float Bonus = 0.0f;

    // Timeline bonus
    if (TimelineManager)
    {
        if (TimelineManager->GetCurrentState() == ETimelineState::DarkWorld)
        {
            Bonus += 0.3f;  // Better stealth in dark timeline
        }
    }

    // Phase shift bonus
    if (bIsPhaseShifted)
    {
        Bonus += PhaseShiftSettings.StealthBonus;
    }

    return Bonus;
}

void USETimelineAssassin::ApplyTimelineShadowEffects()
{
    // FromSoftware-style: Timeline-specific abilities
    if (!TimelineManager || !CanUseTimelineShadow())
    {
        return;
    }

    // Apply shadow effects based on timeline
    if (TimelineManager->GetCurrentState() == ETimelineState::DarkWorld)
    {
        // Enhanced stealth capabilities
        BaseStealthDuration *= 1.5f;
        StealthDetectionRadius *= 0.7f;
    }
}

float USETimelineAssassin::GetTimelineStealthModifier() const
{
    if (!TimelineManager)
    {
        return 1.0f;
    }

    switch (TimelineManager->GetCurrentState())
    {
        case ETimelineState::DarkWorld:
            return 1.5f;  // Enhanced stealth in dark timeline

        case ETimelineState::BrightWorld:
            return 0.7f;  // Reduced stealth in light timeline

        default:
            return 1.0f;
    }
}

bool USETimelineAssassin::CanUseTimelineShadow() const
{
    if (!TimelineManager || !AbilityComponent)
    {
        return false;
    }

    // Check timeline energy
    float EnergyCost = 40.0f * GetTimelineEnergyModifier();
    return AbilityComponent->HasEnoughTimelineEnergy(EnergyCost);
}

void USETimelineAssassin::UpdateTimelineShadowEffects()
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
            AssassinationWindow.DamageMultiplier = 6.0f;  // Enhanced assassination damage
            PhaseShiftSettings.Duration = 7.0f;           // Longer phase shift
            break;

        case ETimelineState::BrightWorld:
            AssassinationWindow.DamageMultiplier = 4.0f;  // Reduced assassination damage
            PhaseShiftSettings.Duration = 4.0f;           // Shorter phase shift
            break;

        default:
            AssassinationWindow.DamageMultiplier = 5.0f;  // Default values
            PhaseShiftSettings.Duration = 5.0f;
            break;
    }
}
