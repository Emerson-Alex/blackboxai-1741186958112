#include "CombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.016f; // ~60 FPS for smooth combat

    // Initialize default values
    CurrentCombatState = ECombatState::OutOfCombat;
    CurrentTimelineState = ETimelineState::None;
    ComboCount = 0;
    LastComboTime = 0.0f;
    ComboWindowDuration = 2.0f;
    TimelineBonusMultiplier = 1.25f;
}

void UCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentStats = BaseStats;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateStatusEffects(DeltaTime);
    ProcessComboTimeout(DeltaTime);
}

void UCombatComponent::EnterCombat(AActor* Target)
{
    if (!Target || CurrentCombatState == ECombatState::InCombat)
    {
        return;
    }

    CurrentTarget = Target;
    SetCombatState(ECombatState::InCombat);
}

void UCombatComponent::ExitCombat()
{
    if (CurrentCombatState == ECombatState::OutOfCombat)
    {
        return;
    }

    CleanupCombatState();
    SetCombatState(ECombatState::OutOfCombat);
}

bool UCombatComponent::IsInCombat() const
{
    return CurrentCombatState == ECombatState::InCombat;
}

void UCombatComponent::InitializeWithStats(const FCharacterStats& InitStats)
{
    BaseStats = InitStats;
    CurrentStats = InitStats;
    ApplyCombatModifiers(CurrentStats);
}

void UCombatComponent::UpdateStats(const FCharacterStats& NewStats)
{
    CurrentStats = NewStats;
    ApplyCombatModifiers(CurrentStats);
}

float UCombatComponent::CalculateDamage(const FAttackData& AttackData, AActor* Target)
{
    if (!Target)
    {
        return 0.0f;
    }

    // Base damage calculation
    float BaseDamage = AttackData.BaseDamage * GetCurrentAttackPower();

    // Apply timeline modifiers
    BaseDamage *= GetTimelineAttackModifier();

    // Apply combo multiplier
    BaseDamage *= GetComboDamageMultiplier();

    // Check for critical hit
    if (ShouldTriggerCritical())
    {
        BaseDamage = CalculateCriticalDamage(BaseDamage);
    }

    // Apply target's defense
    if (UCombatComponent* TargetCombat = Target->FindComponentByClass<UCombatComponent>())
    {
        float Defense = TargetCombat->GetCurrentDefense() * TargetCombat->GetTimelineDefenseModifier();
        BaseDamage = FMath::Max(0.0f, BaseDamage - Defense);
    }

    return BaseDamage;
}

void UCombatComponent::ExecuteAttack(const FAttackData& AttackData, AActor* Target)
{
    if (!Target || !IsInCombat())
    {
        return;
    }

    float FinalDamage = CalculateDamage(AttackData, Target);
    bool bWasCritical = ShouldTriggerCritical();

    // Apply damage to target
    UGameplayStatics::ApplyDamage(
        Target,
        FinalDamage,
        GetOwner()->GetInstigatorController(),
        GetOwner(),
        nullptr
    );

    // Broadcast damage event
    OnDamageDealt.Broadcast(FinalDamage, Target, bWasCritical);

    // Update combo if applicable
    if (AttackData.bCanCombo)
    {
        ContinueCombo(AttackData.AbilityName);
    }
}

void UCombatComponent::ApplyStatusEffect(const FStatusEffect& Effect, AActor* Target)
{
    if (!Target)
    {
        return;
    }

    if (UCombatComponent* TargetCombat = Target->FindComponentByClass<UCombatComponent>())
    {
        // Check if effect should be applied based on timeline state
        if (Effect.RequiredState == ETimelineState::None || 
            Effect.RequiredState == CurrentTimelineState)
        {
            TargetCombat->ActiveStatusEffects.Add(Effect);
        }
    }
}

void UCombatComponent::HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState)
{
    CurrentTimelineState = NewState;

    // Remove incompatible effects
    RemoveExpiredEffects();

    // Apply new timeline-specific effects
    ApplyTimelineEffects();

    // Recalculate stats with new timeline modifiers
    ApplyCombatModifiers(CurrentStats);
}

float UCombatComponent::GetTimelineAttackModifier() const
{
    float Modifier = 1.0f;

    switch (CurrentTimelineState)
    {
        case ETimelineState::Light:
            Modifier *= (1.0f + BaseStats.LightDamageBonus);
            break;
        case ETimelineState::Dark:
            Modifier *= (1.0f + BaseStats.DarkDamageBonus);
            break;
        default:
            break;
    }

    return Modifier;
}

float UCombatComponent::GetTimelineDefenseModifier() const
{
    float Modifier = 1.0f;

    switch (CurrentTimelineState)
    {
        case ETimelineState::Light:
            Modifier *= 1.15f; // Light timeline defense bonus
            break;
        case ETimelineState::Dark:
            Modifier *= 0.85f; // Dark timeline defense penalty
            break;
        default:
            break;
    }

    return Modifier;
}

float UCombatComponent::GetCurrentAttackPower() const
{
    return CurrentStats.AttackPower * GetStatusEffectModifier(EStatType::AttackPower);
}

float UCombatComponent::GetCurrentDefense() const
{
    return CurrentStats.Defense * GetStatusEffectModifier(EStatType::Defense);
}

float UCombatComponent::GetCriticalChance() const
{
    float BaseChance = CurrentStats.CriticalChance;

    // Increase crit chance in Dark timeline
    if (CurrentTimelineState == ETimelineState::Dark)
    {
        BaseChance += 0.1f;
    }

    return FMath::Clamp(BaseChance, 0.0f, 1.0f);
}

float UCombatComponent::GetCriticalMultiplier() const
{
    return CurrentStats.CriticalMultiplier;
}

void UCombatComponent::StartCombo()
{
    ComboCount = 0;
    LastComboTime = GetWorld()->GetTimeSeconds();
    ComboChain.Empty();
}

void UCombatComponent::ContinueCombo(const FName& AbilityName)
{
    if (!IsComboWindowOpen() || !ValidateComboChain(AbilityName))
    {
        ResetCombo();
        return;
    }

    ComboChain.Add(AbilityName);
    ComboCount++;
    LastComboTime = GetWorld()->GetTimeSeconds();
}

void UCombatComponent::EndCombo()
{
    ResetCombo();
}

int32 UCombatComponent::GetCurrentComboCount() const
{
    return ComboCount;
}

void UCombatComponent::UpdateStatusEffects(float DeltaTime)
{
    bool bNeedsUpdate = false;

    // Update effect durations and remove expired ones
    for (int32 i = ActiveStatusEffects.Num() - 1; i >= 0; --i)
    {
        FStatusEffect& Effect = ActiveStatusEffects[i];
        Effect.Duration -= DeltaTime;

        if (Effect.Duration <= 0.0f)
        {
            HandleStatusEffectExpiration(Effect);
            ActiveStatusEffects.RemoveAt(i);
            bNeedsUpdate = true;
        }
    }

    if (bNeedsUpdate)
    {
        ApplyCombatModifiers(CurrentStats);
    }
}

void UCombatComponent::ProcessComboTimeout(float DeltaTime)
{
    if (ComboCount > 0 && !IsComboWindowOpen())
    {
        ResetCombo();
    }
}

float UCombatComponent::CalculateCriticalDamage(float BaseDamage) const
{
    return BaseDamage * GetCriticalMultiplier();
}

bool UCombatComponent::ValidateComboChain(const FName& AbilityName) const
{
    // Check if this ability can be used in the current combo position
    // This would typically check against a data table of valid combo chains
    return true; // Simplified for now
}

void UCombatComponent::ApplyCombatModifiers(FCharacterStats& Stats) const
{
    // Apply timeline modifiers
    Stats.AttackPower *= GetTimelineAttackModifier();
    Stats.Defense *= GetTimelineDefenseModifier();

    // Apply status effect modifiers
    Stats.AttackPower *= GetStatusEffectModifier(EStatType::AttackPower);
    Stats.Defense *= GetStatusEffectModifier(EStatType::Defense);
}

void UCombatComponent::HandleStatusEffectExpiration(const FStatusEffect& Effect)
{
    // Handle any cleanup or triggers when an effect expires
}

float UCombatComponent::CalculateTimelineBonus(float BaseValue, ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::Light:
            return BaseValue * TimelineBonusMultiplier;
        case ETimelineState::Dark:
            return BaseValue * (2.0f - TimelineBonusMultiplier);
        default:
            return BaseValue;
    }
}

float UCombatComponent::GetStatusEffectModifier(EStatType StatType) const
{
    float Modifier = 1.0f;

    for (const FStatusEffect& Effect : ActiveStatusEffects)
    {
        if (Effect.AffectedStat == StatType)
        {
            Modifier *= Effect.Magnitude;
        }
    }

    return Modifier;
}

bool UCombatComponent::ShouldTriggerCritical() const
{
    return FMath::FRand() < GetCriticalChance();
}

void UCombatComponent::SetCombatState(ECombatState NewState)
{
    ECombatState OldState = CurrentCombatState;
    CurrentCombatState = NewState;
    OnCombatStateChanged.Broadcast(NewState, OldState);
}

void UCombatComponent::CleanupCombatState()
{
    CurrentTarget = nullptr;
    ResetCombo();
    RemoveExpiredEffects();
}

void UCombatComponent::ApplyTimelineEffects()
{
    // Apply timeline-specific status effects
    if (CurrentTimelineState != ETimelineState::None)
    {
        FStatusEffect TimelineEffect;
        TimelineEffect.Duration = -1.0f; // Permanent while in this timeline
        TimelineEffect.RequiredState = CurrentTimelineState;

        if (CurrentTimelineState == ETimelineState::Light)
        {
            TimelineEffect.AffectedStat = EStatType::Defense;
            TimelineEffect.Magnitude = 1.15f;
        }
        else if (CurrentTimelineState == ETimelineState::Dark)
        {
            TimelineEffect.AffectedStat = EStatType::AttackPower;
            TimelineEffect.Magnitude = 1.25f;
        }

        ActiveStatusEffects.Add(TimelineEffect);
    }
}

void UCombatComponent::RemoveExpiredEffects()
{
    ActiveStatusEffects.RemoveAll([this](const FStatusEffect& Effect) {
        return Effect.RequiredState != ETimelineState::None && 
               Effect.RequiredState != CurrentTimelineState;
    });
}

void UCombatComponent::ResetCombo()
{
    ComboCount = 0;
    LastComboTime = 0.0f;
    ComboChain.Empty();
}

bool UCombatComponent::IsComboWindowOpen() const
{
    return (GetWorld()->GetTimeSeconds() - LastComboTime) <= ComboWindowDuration;
}

float UCombatComponent::GetComboDamageMultiplier() const
{
    // Increase damage with combo count
    return 1.0f + (ComboCount * 0.1f);
}

void UCombatComponent::UpdateDamageCache()
{
    AbilityDamageCache.Empty();
    // Cache would be populated with frequently used ability damages
}
