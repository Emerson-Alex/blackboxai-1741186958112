// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Characters/Classes/SECharacterClassBase.h"
#include "Systems/TimelineManager.h"
#include "Combat/AbilityComponent.h"

USECharacterClassBase::USECharacterClassBase()
{
    // Default base stats
    BaseStats.Health = 1000.0f;
    BaseStats.TimelineEnergy = 100.0f;
    BaseStats.StaminaRegen = 10.0f;
    BaseStats.TimelinePower = 50.0f;
    BaseStats.CriticalChance = 0.05f;
    BaseStats.DodgeChance = 0.05f;

    // Default timeline affinity
    TimelineAffinity.PreferredTimeline = ETimelineState::Any;
    TimelineAffinity.TimelineEnergyEfficiency = 1.0f;
    TimelineAffinity.TimelineDamageBonus = 1.0f;
    TimelineAffinity.TimelineResistance = 1.0f;

    CurrentSpecialization = EClassSpecialization::None;
}

void USECharacterClassBase::Initialize(UTimelineManager* InTimelineManager, UAbilityComponent* InAbilityComponent)
{
    TimelineManager = InTimelineManager;
    AbilityComponent = InAbilityComponent;

    // Apply initial stats
    UpdateStats();

    // Apply timeline effects if already in a specific timeline
    if (TimelineManager)
    {
        ApplyTimelineEffects(TimelineManager->GetCurrentState());
    }
}

void USECharacterClassBase::OnTimelineStateChanged(ETimelineState NewState)
{
    // FromSoftware-style: Timeline changes significantly affect character power
    ApplyTimelineEffects(NewState);
    BP_OnTimelineStateChanged(NewState);
}

void USECharacterClassBase::LevelUp()
{
    // FromSoftware-style: Each level provides meaningful power increases
    BaseStats.Health *= 1.1f;
    BaseStats.TimelineEnergy *= 1.05f;
    BaseStats.TimelinePower *= 1.08f;
    BaseStats.CriticalChance += 0.002f;
    BaseStats.DodgeChance += 0.002f;

    // Apply specialization bonuses
    ApplySpecializationBonuses();

    // Update all stats
    UpdateStats();

    BP_OnLevelUp();
}

void USECharacterClassBase::UnlockSpecialization(EClassSpecialization Specialization)
{
    // FromSoftware-style: Specializations dramatically change playstyle
    if (!AvailableSpecializations.Contains(Specialization))
    {
        return;
    }

    CurrentSpecialization = Specialization;

    // Apply specialization bonuses
    ApplySpecializationBonuses();

    // Update stats with new specialization
    UpdateStats();

    BP_OnSpecializationUnlocked(Specialization);
}

float USECharacterClassBase::ModifyDamage(float BaseDamage, ETimelineState DamageTimeline)
{
    float FinalDamage = BaseDamage;

    // FromSoftware-style: Timeline alignment significantly affects damage
    if (TimelineManager)
    {
        ETimelineState CurrentState = TimelineManager->GetCurrentState();
        
        // Timeline synergy bonus
        if (CurrentState == DamageTimeline)
        {
            FinalDamage *= TimelineAffinity.TimelineDamageBonus;
        }
        // Timeline opposition penalty
        else if (CurrentState != ETimelineState::Any && DamageTimeline != ETimelineState::Any)
        {
            FinalDamage *= 0.7f;
        }
    }

    // Apply specialization modifiers
    switch (CurrentSpecialization)
    {
        case EClassSpecialization::TimelineWeaver:
            FinalDamage *= 1.2f;  // Timeline mastery bonus
            break;

        case EClassSpecialization::VoidWalker:
            if (DamageTimeline == ETimelineState::DarkWorld)
            {
                FinalDamage *= 1.3f;  // Dark timeline bonus
            }
            break;

        case EClassSpecialization::LightBringer:
            if (DamageTimeline == ETimelineState::BrightWorld)
            {
                FinalDamage *= 1.3f;  // Light timeline bonus
            }
            break;

        case EClassSpecialization::ChronoMaster:
            FinalDamage *= (1.0f + CalculateTimelineSynergy());
            break;
    }

    return FinalDamage;
}

float USECharacterClassBase::ModifyDefense(float BaseDefense, ETimelineState AttackTimeline)
{
    float FinalDefense = BaseDefense;

    // FromSoftware-style: Timeline resistance provides strategic depth
    if (TimelineManager)
    {
        ETimelineState CurrentState = TimelineManager->GetCurrentState();
        
        // Timeline resistance bonus
        if (CurrentState == TimelineAffinity.PreferredTimeline)
        {
            FinalDefense *= TimelineAffinity.TimelineResistance;
        }
    }

    // Apply specialization modifiers
    switch (CurrentSpecialization)
    {
        case EClassSpecialization::TimelineWeaver:
            FinalDefense *= 1.1f;  // General defense bonus
            break;

        case EClassSpecialization::VoidWalker:
            if (AttackTimeline == ETimelineState::BrightWorld)
            {
                FinalDefense *= 0.8f;  // Weakness to light
            }
            break;

        case EClassSpecialization::LightBringer:
            if (AttackTimeline == ETimelineState::DarkWorld)
            {
                FinalDefense *= 0.8f;  // Weakness to darkness
            }
            break;

        case EClassSpecialization::ChronoMaster:
            FinalDefense *= (1.0f + CalculateTimelineSynergy() * 0.5f);
            break;
    }

    return FinalDefense;
}

float USECharacterClassBase::GetTimelineEnergyModifier() const
{
    float Modifier = TimelineAffinity.TimelineEnergyEfficiency;

    // FromSoftware-style: Resource management is crucial
    if (TimelineManager)
    {
        if (TimelineManager->GetCurrentState() == TimelineAffinity.PreferredTimeline)
        {
            Modifier *= 1.5f;  // Significant bonus in preferred timeline
        }
    }

    // Apply specialization modifiers
    switch (CurrentSpecialization)
    {
        case EClassSpecialization::TimelineWeaver:
            Modifier *= 1.3f;
            break;

        case EClassSpecialization::ChronoMaster:
            Modifier *= (1.0f + CalculateTimelineSynergy());
            break;
    }

    return Modifier;
}

bool USECharacterClassBase::CanUseTimelineAbility(ETimelineState RequiredTimeline) const
{
    if (!TimelineManager)
    {
        return false;
    }

    // FromSoftware-style: Strict ability requirements
    ETimelineState CurrentState = TimelineManager->GetCurrentState();
    
    // Timeline Weaver and Chrono Master can use abilities from any timeline
    if (CurrentSpecialization == EClassSpecialization::TimelineWeaver ||
        CurrentSpecialization == EClassSpecialization::ChronoMaster)
    {
        return true;
    }

    // Others must be in the correct timeline
    return CurrentState == RequiredTimeline || RequiredTimeline == ETimelineState::Any;
}

void USECharacterClassBase::ApplyTimelineEffects(ETimelineState State)
{
    // FromSoftware-style: Timeline changes have dramatic effects
    float TimelineSynergy = CalculateTimelineSynergy();

    switch (State)
    {
        case ETimelineState::DarkWorld:
            BaseStats.CriticalChance *= 1.3f;
            BaseStats.DodgeChance *= 1.2f;
            BaseStats.TimelinePower *= (1.0f + TimelineSynergy);
            break;

        case ETimelineState::BrightWorld:
            BaseStats.Health *= 1.2f;
            BaseStats.StaminaRegen *= 1.3f;
            BaseStats.TimelinePower *= (1.0f + TimelineSynergy);
            break;
    }

    UpdateStats();
}

void USECharacterClassBase::UpdateStats()
{
    // Apply all stat modifiers
    if (AbilityComponent)
    {
        AbilityComponent->UpdateStats(BaseStats);
    }
}

void USECharacterClassBase::ApplySpecializationBonuses()
{
    // FromSoftware-style: Specializations provide unique advantages
    switch (CurrentSpecialization)
    {
        case EClassSpecialization::TimelineWeaver:
            TimelineAffinity.TimelineEnergyEfficiency *= 1.5f;
            TimelineAffinity.TimelineDamageBonus *= 1.3f;
            break;

        case EClassSpecialization::VoidWalker:
            BaseStats.CriticalChance *= 1.5f;
            BaseStats.DodgeChance *= 1.3f;
            break;

        case EClassSpecialization::LightBringer:
            BaseStats.Health *= 1.3f;
            BaseStats.TimelinePower *= 1.3f;
            break;

        case EClassSpecialization::ChronoMaster:
            TimelineAffinity.TimelineResistance *= 1.3f;
            TimelineAffinity.TimelineEnergyEfficiency *= 1.3f;
            break;
    }
}

float USECharacterClassBase::CalculateTimelineSynergy() const
{
    if (!TimelineManager)
    {
        return 0.0f;
    }

    // FromSoftware-style: Complex synergy calculations
    float Synergy = 0.0f;
    ETimelineState CurrentState = TimelineManager->GetCurrentState();

    // Base timeline synergy
    if (CurrentState == TimelineAffinity.PreferredTimeline)
    {
        Synergy += 0.3f;
    }

    // Specialization synergy
    switch (CurrentSpecialization)
    {
        case EClassSpecialization::TimelineWeaver:
            Synergy += 0.2f;
            break;

        case EClassSpecialization::VoidWalker:
            if (CurrentState == ETimelineState::DarkWorld)
            {
                Synergy += 0.4f;
            }
            break;

        case EClassSpecialization::LightBringer:
            if (CurrentState == ETimelineState::BrightWorld)
            {
                Synergy += 0.4f;
            }
            break;

        case EClassSpecialization::ChronoMaster:
            Synergy += 0.3f;
            break;
    }

    return Synergy;
}
