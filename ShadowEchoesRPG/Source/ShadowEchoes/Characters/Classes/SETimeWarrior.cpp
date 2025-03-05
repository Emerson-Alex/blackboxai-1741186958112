// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Characters/Classes/SETimeWarrior.h"
#include "Systems/TimelineManager.h"
#include "Combat/AbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USETimeWarrior::USETimeWarrior()
{
    // Base class settings
    BaseStats.Health = 1000.0f;        // Balanced health
    BaseStats.TimelineEnergy = 180.0f; // High timeline energy
    BaseStats.StaminaRegen = 15.0f;    // High stamina regen
    BaseStats.TimelinePower = 75.0f;   // Very high timeline power
    BaseStats.CriticalChance = 0.10f;  // Moderate crit
    BaseStats.DodgeChance = 0.08f;     // Moderate dodge

    // Timeline affinity
    TimelineAffinity.PreferredTimeline = ETimelineState::Any;
    TimelineAffinity.TimelineEnergyEfficiency = 1.4f;
    TimelineAffinity.TimelineDamageBonus = 1.3f;
    TimelineAffinity.TimelineResistance = 1.2f;

    // Stance settings
    // FromSoftware-style: Each stance has distinct advantages
    FTimelineStance NeutralStance;
    NeutralStance.DamageMultiplier = 1.0f;
    NeutralStance.DefenseMultiplier = 1.0f;
    NeutralStance.SpeedMultiplier = 1.0f;
    NeutralStance.TimelineEnergyRate = 1.0f;
    NeutralStance.bCanTimeShift = true;
    Stances.Add(ETimelineState::Any, NeutralStance);

    FTimelineStance DarkStance;
    DarkStance.DamageMultiplier = 1.4f;
    DarkStance.DefenseMultiplier = 0.8f;
    DarkStance.SpeedMultiplier = 1.2f;
    DarkStance.TimelineEnergyRate = 1.2f;
    DarkStance.bCanTimeShift = true;
    Stances.Add(ETimelineState::DarkWorld, DarkStance);

    FTimelineStance LightStance;
    LightStance.DamageMultiplier = 1.2f;
    LightStance.DefenseMultiplier = 1.3f;
    LightStance.SpeedMultiplier = 0.9f;
    LightStance.TimelineEnergyRate = 1.3f;
    LightStance.bCanTimeShift = true;
    Stances.Add(ETimelineState::BrightWorld, LightStance);

    // Combat settings
    // FromSoftware-style: Complex combo system
    FTimelineCombo CrossTimelineSlash;
    CrossTimelineSlash.ComboSequence = {ETimelineState::DarkWorld, ETimelineState::BrightWorld};
    CrossTimelineSlash.DamageMultiplier = 2.0f;
    CrossTimelineSlash.TimelineEnergyCost = 40.0f;
    CrossTimelineSlash.CooldownDuration = 5.0f;
    CrossTimelineSlash.bRequiresTimeShift = true;
    Combos.Add("CrossTimelineSlash", CrossTimelineSlash);

    FTimelineCombo TimelineSurge;
    TimelineSurge.ComboSequence = {ETimelineState::Any, ETimelineState::Any, ETimelineState::Any};
    TimelineSurge.DamageMultiplier = 2.5f;
    TimelineSurge.TimelineEnergyCost = 60.0f;
    TimelineSurge.CooldownDuration = 8.0f;
    TimelineSurge.bRequiresTimeShift = false;
    Combos.Add("TimelineSurge", TimelineSurge);

    // Timeline ability settings
    TimeShiftDuration = 10.0f;
    TimeShiftCooldown = 15.0f;
    TimeShiftEnergyCost = 50.0f;

    // Combat window settings
    ComboWindowDuration = 3.0f;
    MaxComboLength = 4;
    StanceSwitchCooldown = 2.0f;

    // State initialization
    CurrentStance = ETimelineState::Any;
    bIsTimeShifted = false;
    CurrentTimeShiftTime = 0.0f;
    ComboTimeRemaining = 0.0f;
    LastStanceSwitchTime = -StanceSwitchCooldown;
}

bool USETimeWarrior::SwitchStance(ETimelineState NewStance)
{
    // FromSoftware-style: Strategic stance switching
    if (!Stances.Contains(NewStance))
    {
        return false;
    }

    float CurrentTime = UGameplayStatics::GetRealTimeSeconds(this);
    if (CurrentTime - LastStanceSwitchTime < StanceSwitchCooldown)
    {
        return false;
    }

    // Apply stance change
    CurrentStance = NewStance;
    LastStanceSwitchTime = CurrentTime;

    // Apply stance effects
    ApplyStanceEffects();

    // Notify stance change
    BP_OnStanceChanged(NewStance);

    return true;
}

bool USETimeWarrior::ExecuteCombo(const FName& ComboID)
{
    // FromSoftware-style: Precise combo execution
    if (!ValidateCombo(ComboID))
    {
        return false;
    }

    const FTimelineCombo& Combo = Combos[ComboID];

    // Check timeline energy
    if (!AbilityComponent || !AbilityComponent->HasEnoughTimelineEnergy(Combo.TimelineEnergyCost))
    {
        return false;
    }

    // Check combo requirements
    if (Combo.bRequiresTimeShift && !bIsTimeShifted)
    {
        return false;
    }

    // Execute combo
    AbilityComponent->ConsumeTimelineEnergy(Combo.TimelineEnergyCost);

    // Apply combo effects
    float ComboBonus = CalculateComboBonus();
    float FinalDamage = Combo.DamageMultiplier * ComboBonus;
    // Apply damage through combat system

    // Reset combo window
    ResetCombo();

    // Notify combo execution
    BP_OnComboExecuted(ComboID);

    return true;
}

void USETimeWarrior::ResetCombo()
{
    CurrentCombo.Empty();
    ComboTimeRemaining = 0.0f;

    // Notify combo reset
    BP_OnComboReset();
}

bool USETimeWarrior::TimeShift(ETimelineState TargetTimeline)
{
    // FromSoftware-style: Strategic timeline manipulation
    if (bIsTimeShifted || !AbilityComponent)
    {
        return false;
    }

    // Check if current stance allows time shift
    if (!Stances[CurrentStance].bCanTimeShift)
    {
        return false;
    }

    // Check timeline energy
    float EnergyCost = TimeShiftEnergyCost * GetTimelineEnergyModifier();
    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    // Apply time shift
    bIsTimeShifted = true;
    CurrentTimeShiftTime = TimeShiftDuration;
    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify time shift start
    BP_OnTimeShiftStart(TargetTimeline);

    return true;
}

void USETimeWarrior::EndTimeShift()
{
    if (!bIsTimeShifted)
    {
        return;
    }

    bIsTimeShifted = false;
    CurrentTimeShiftTime = 0.0f;

    // Notify time shift end
    BP_OnTimeShiftEnd();
}

bool USETimeWarrior::ExecuteStanceAbility()
{
    // FromSoftware-style: Stance-specific abilities
    if (!AbilityComponent)
    {
        return false;
    }

    const FTimelineStance& CurrentStanceData = Stances[CurrentStance];
    float EnergyCost = 30.0f * CurrentStanceData.TimelineEnergyRate;

    if (!AbilityComponent->HasEnoughTimelineEnergy(EnergyCost))
    {
        return false;
    }

    // Execute stance-specific ability
    switch (CurrentStance)
    {
        case ETimelineState::DarkWorld:
            // Dark stance ability: Timeline Rend
            // High damage, low defense
            break;

        case ETimelineState::BrightWorld:
            // Light stance ability: Timeline Ward
            // High defense, moderate damage
            break;

        case ETimelineState::Any:
            // Neutral stance ability: Timeline Balance
            // Balanced effects
            break;
    }

    AbilityComponent->ConsumeTimelineEnergy(EnergyCost);

    // Notify ability use
    BP_OnStanceAbilityUsed();

    return true;
}

float USETimeWarrior::GetStanceBonus() const
{
    if (!Stances.Contains(CurrentStance))
    {
        return 1.0f;
    }

    const FTimelineStance& StanceData = Stances[CurrentStance];
    float Bonus = StanceData.DamageMultiplier;

    // Timeline synergy
    if (TimelineManager && TimelineManager->GetCurrentState() == CurrentStance)
    {
        Bonus *= 1.2f;
    }

    return Bonus;
}

void USETimeWarrior::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // FromSoftware-style: Timeline affects abilities
    UpdateTimelineEffects();

    // Update stance effects
    UpdateStanceEffects();
}

float USETimeWarrior::ModifyDamage(float BaseDamage, ETimelineState DamageTimeline)
{
    float FinalDamage = Super::ModifyDamage(BaseDamage, DamageTimeline);

    // Apply stance modifiers
    if (Stances.Contains(CurrentStance))
    {
        FinalDamage *= Stances[CurrentStance].DamageMultiplier;
    }

    // Time shift bonus
    if (bIsTimeShifted)
    {
        FinalDamage *= 1.3f;
    }

    return FinalDamage;
}

float USETimeWarrior::ModifyDefense(float BaseDefense, ETimelineState AttackTimeline)
{
    float FinalDefense = Super::ModifyDefense(BaseDefense, AttackTimeline);

    // Apply stance modifiers
    if (Stances.Contains(CurrentStance))
    {
        FinalDefense *= Stances[CurrentStance].DefenseMultiplier;
    }

    // Time shift bonus
    if (bIsTimeShifted)
    {
        FinalDefense *= 1.2f;
    }

    return FinalDefense;
}

void USETimeWarrior::UpdateStanceEffects()
{
    if (!Stances.Contains(CurrentStance))
    {
        return;
    }

    const FTimelineStance& StanceData = Stances[CurrentStance];

    // Apply movement speed
    if (AbilityComponent)
    {
        AbilityComponent->SetMovementSpeedModifier(StanceData.SpeedMultiplier);
    }

    // Apply timeline energy regen rate
    // This would be handled by the ability system
}

void USETimeWarrior::ProcessComboWindow()
{
    if (ComboTimeRemaining <= 0.0f)
    {
        return;
    }

    ComboTimeRemaining -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (ComboTimeRemaining <= 0.0f)
    {
        ResetCombo();
    }
}

void USETimeWarrior::UpdateTimeShift()
{
    if (!bIsTimeShifted)
    {
        return;
    }

    CurrentTimeShiftTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
    if (CurrentTimeShiftTime <= 0.0f)
    {
        EndTimeShift();
    }
}

bool USETimeWarrior::ValidateCombo(const FName& ComboID) const
{
    if (!Combos.Contains(ComboID))
    {
        return false;
    }

    const FTimelineCombo& Combo = Combos[ComboID];

    // Check combo sequence
    if (CurrentCombo.Num() != Combo.ComboSequence.Num())
    {
        return false;
    }

    for (int32 i = 0; i < CurrentCombo.Num(); ++i)
    {
        if (CurrentCombo[i] != Combo.ComboSequence[i])
        {
            return false;
        }
    }

    return true;
}

float USETimeWarrior::CalculateComboBonus() const
{
    // FromSoftware-style: Complex combo calculations
    float Bonus = 1.0f;

    // Stance bonus
    if (Stances.Contains(CurrentStance))
    {
        Bonus *= Stances[CurrentStance].DamageMultiplier;
    }

    // Time shift bonus
    if (bIsTimeShifted)
    {
        Bonus *= 1.2f;
    }

    // Combo length bonus
    Bonus *= (1.0f + (CurrentCombo.Num() * 0.1f));

    return Bonus;
}

void USETimeWarrior::ApplyStanceEffects()
{
    // FromSoftware-style: Stance-specific effects
    if (!Stances.Contains(CurrentStance))
    {
        return;
    }

    const FTimelineStance& StanceData = Stances[CurrentStance];

    // Apply movement speed
    if (AbilityComponent)
    {
        AbilityComponent->SetMovementSpeedModifier(StanceData.SpeedMultiplier);
    }

    // Apply visual effects
    // This would be handled in blueprints
}

float USETimeWarrior::GetTimelineStanceModifier() const
{
    if (!TimelineManager)
    {
        return 1.0f;
    }

    ETimelineState CurrentTimeline = TimelineManager->GetCurrentState();
    
    // Bonus for matching timeline
    if (CurrentTimeline == CurrentStance)
    {
        return 1.3f;
    }
    // Penalty for opposing timeline
    else if (CurrentTimeline != ETimelineState::Any && CurrentStance != ETimelineState::Any)
    {
        return 0.8f;
    }

    return 1.0f;
}

bool USETimeWarrior::CanUseTimeShift() const
{
    if (!Stances.Contains(CurrentStance))
    {
        return false;
    }

    return Stances[CurrentStance].bCanTimeShift;
}

void USETimeWarrior::UpdateTimelineEffects()
{
    // FromSoftware-style: Dynamic ability adjustments
    if (!TimelineManager)
    {
        return;
    }

    // Update stance bonuses based on timeline
    float TimelineModifier = GetTimelineStanceModifier();
    if (Stances.Contains(CurrentStance))
    {
        FTimelineStance& StanceData = Stances[CurrentStance];
        StanceData.DamageMultiplier *= TimelineModifier;
        StanceData.DefenseMultiplier *= TimelineModifier;
    }

    // Update time shift duration
    if (bIsTimeShifted)
    {
        if (TimelineManager->GetCurrentState() == CurrentStance)
        {
            CurrentTimeShiftTime += 2.0f;  // Bonus duration in matching timeline
        }
    }
}
