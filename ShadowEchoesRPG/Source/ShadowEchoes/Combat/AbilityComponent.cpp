#include "AbilityComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/CombatComponent.h"

const float UAbilityComponent::ComboWindowDuration = 2.0f;

UAbilityComponent::UAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.016f; // ~60 FPS for smooth ability updates

    CurrentTimelineState = ETimelineState::None;
    LastComboTime = 0.0f;
}

void UAbilityComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCooldowns(DeltaTime);
    CleanupExpiredEffects();
}

bool UAbilityComponent::ActivateAbility(const FName& AbilityName)
{
    const FAbilityData* AbilityData = GetAbilityData(AbilityName);
    if (!AbilityData)
    {
        HandleAbilityFailure(AbilityName, TEXT("Ability not found"));
        return false;
    }

    if (!ValidateAbilityUse(*AbilityData))
    {
        return false;
    }

    if (!HasEnoughResources(*AbilityData))
    {
        HandleAbilityFailure(AbilityName, TEXT("Insufficient resources"));
        return false;
    }

    // Execute ability
    bool bSuccess = ExecuteAbility(*AbilityData);
    if (bSuccess)
    {
        ConsumeResources(*AbilityData);
        StartCooldown(AbilityName, AbilityData->Cooldown);
        PlayAbilityEffects(*AbilityData);
    }

    OnAbilityActivated.Broadcast(AbilityName, bSuccess);
    return bSuccess;
}

void UAbilityComponent::LearnAbility(const FAbilityData& AbilityData)
{
    if (!ValidateAbilityData(AbilityData))
    {
        LogAbilityError(FString::Printf(TEXT("Invalid ability data for %s"), *AbilityData.Name.ToString()));
        return;
    }

    LearnedAbilities.Add(AbilityData.Name, AbilityData);
    OnAbilityLearned.Broadcast(AbilityData);
}

void UAbilityComponent::ForgetAbility(const FName& AbilityName)
{
    LearnedAbilities.Remove(AbilityName);
    AbilityCooldowns.Remove(AbilityName);
}

bool UAbilityComponent::HasAbility(const FName& AbilityName) const
{
    return LearnedAbilities.Contains(AbilityName);
}

const FAbilityData* UAbilityComponent::GetAbilityData(const FName& AbilityName) const
{
    return LearnedAbilities.Find(AbilityName);
}

void UAbilityComponent::HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState)
{
    CurrentTimelineState = NewState;

    // Update ability modifiers for new timeline state
    for (auto& Ability : LearnedAbilities)
    {
        ApplyTimelineModifiers(Ability.Value);
    }

    // Reset combo if timeline changes
    CurrentComboChain.Empty();
    LastComboTime = 0.0f;
}

TArray<FName> UAbilityComponent::GetAvailableAbilities(ETimelineState State) const
{
    TArray<FName> Available;

    for (const auto& Pair : LearnedAbilities)
    {
        const FAbilityData& Ability = Pair.Value;
        if (Ability.RequiredState == ETimelineState::None || Ability.RequiredState == State)
        {
            Available.Add(Ability.Name);
        }
    }

    return Available;
}

float UAbilityComponent::GetRemainingCooldown(const FName& AbilityName) const
{
    const float* Cooldown = AbilityCooldowns.Find(AbilityName);
    return Cooldown ? *Cooldown : 0.0f;
}

bool UAbilityComponent::IsAbilityReady(const FName& AbilityName) const
{
    return GetRemainingCooldown(AbilityName) <= 0.0f;
}

bool UAbilityComponent::HasEnoughResources(const FAbilityData& AbilityData) const
{
    return CheckResourceCosts(AbilityData);
}

void UAbilityComponent::ConsumeResources(const FAbilityData& AbilityData)
{
    HandleResourceConsumption(AbilityData);
}

bool UAbilityComponent::CanUseInCombo(const FName& AbilityName, const TArray<FName>& CurrentCombo) const
{
    const FAbilityData* AbilityData = GetAbilityData(AbilityName);
    if (!AbilityData || !AbilityData->bCanCombo)
    {
        return false;
    }

    // Check if this ability can follow the current combo chain
    if (CurrentCombo.Num() > 0)
    {
        const FName& LastAbility = CurrentCombo.Last();
        const FAbilityData* LastAbilityData = GetAbilityData(LastAbility);
        
        if (!LastAbilityData || !LastAbilityData->ValidComboFollowUps.Contains(AbilityName))
        {
            return false;
        }
    }

    return true;
}

float UAbilityComponent::GetComboBonus(const FName& AbilityName, const TArray<FName>& CurrentCombo) const
{
    if (CurrentCombo.Num() == 0)
    {
        return 1.0f;
    }

    // Base combo multiplier increases with combo length
    float ComboMultiplier = 1.0f + (CurrentCombo.Num() * 0.1f);

    // Additional bonus for timeline-matching combos
    bool bAllSameTimeline = true;
    ETimelineState ComboState = ETimelineState::None;

    for (const FName& AbilityInChain : CurrentCombo)
    {
        const FAbilityData* AbilityData = GetAbilityData(AbilityInChain);
        if (!AbilityData)
        {
            continue;
        }

        if (ComboState == ETimelineState::None)
        {
            ComboState = AbilityData->RequiredState;
        }
        else if (AbilityData->RequiredState != ComboState)
        {
            bAllSameTimeline = false;
            break;
        }
    }

    if (bAllSameTimeline && ComboState == CurrentTimelineState)
    {
        ComboMultiplier *= 1.5f;
    }

    return ComboMultiplier;
}

bool UAbilityComponent::ExecuteAbility(const FAbilityData& AbilityData)
{
    // Get owner's combat component
    UCombatComponent* Combat = GetOwner()->FindComponentByClass<UCombatComponent>();
    if (!Combat)
    {
        return false;
    }

    // Apply timeline and combo modifiers
    FAbilityData ModifiedAbility = AbilityData;
    ApplyTimelineModifiers(ModifiedAbility);
    
    // Execute ability effects
    ApplyAbilityEffects(ModifiedAbility);

    // Update combo system
    if (ModifiedAbility.bCanCombo)
    {
        if (IsComboWindowOpen())
        {
            CurrentComboChain.Add(ModifiedAbility.Name);
        }
        else
        {
            CurrentComboChain = { ModifiedAbility.Name };
        }
        LastComboTime = GetWorld()->GetTimeSeconds();
    }

    return true;
}

bool UAbilityComponent::ValidateAbilityUse(const FAbilityData& AbilityData) const
{
    // Check cooldown
    if (!IsAbilityReady(AbilityData.Name))
    {
        return false;
    }

    // Check timeline state requirement
    if (!IsValidAbilityForCurrentState(AbilityData))
    {
        return false;
    }

    // Check resources
    if (!AreResourcesAvailable(AbilityData))
    {
        return false;
    }

    return true;
}

void UAbilityComponent::ApplyAbilityEffects(const FAbilityData& AbilityData)
{
    // Apply damage/healing
    if (AbilityData.DamageType != EDamageType::None)
    {
        if (AActor* Target = Cast<ACharacter>(GetOwner())->GetCurrentTarget())
        {
            float Damage = AbilityData.BaseDamage * CalculateTimelineBonus(AbilityData);
            UGameplayStatics::ApplyDamage(
                Target,
                Damage,
                GetOwner()->GetInstigatorController(),
                GetOwner(),
                nullptr
            );
        }
    }

    // Apply status effects
    for (const FStatusEffect& Effect : AbilityData.StatusEffects)
    {
        if (AActor* Target = Cast<ACharacter>(GetOwner())->GetCurrentTarget())
        {
            if (UCombatComponent* TargetCombat = Target->FindComponentByClass<UCombatComponent>())
            {
                TargetCombat->ApplyStatusEffect(Effect);
            }
        }
    }
}

void UAbilityComponent::StartCooldown(const FName& AbilityName, float Duration)
{
    AbilityCooldowns.Add(AbilityName, Duration);
}

void UAbilityComponent::ApplyTimelineModifiers(FAbilityData& AbilityData) const
{
    if (AbilityData.RequiredState == CurrentTimelineState)
    {
        AbilityData.BaseDamage *= 1.25f;
        AbilityData.ResourceCost *= 0.75f;
    }
}

float UAbilityComponent::CalculateTimelineBonus(const FAbilityData& AbilityData) const
{
    float Bonus = 1.0f;

    // Timeline state matching bonus
    if (AbilityData.RequiredState == CurrentTimelineState)
    {
        Bonus *= 1.25f;
    }

    // Combo bonus
    if (AbilityData.bCanCombo && CurrentComboChain.Num() > 0)
    {
        Bonus *= GetComboBonus(AbilityData.Name, CurrentComboChain);
    }

    return Bonus;
}

bool UAbilityComponent::CheckResourceCosts(const FAbilityData& AbilityData) const
{
    // Check energy cost
    if (UCombatComponent* Combat = GetOwner()->FindComponentByClass<UCombatComponent>())
    {
        if (Combat->GetCurrentEnergy() < AbilityData.ResourceCost)
        {
            return false;
        }
    }

    return true;
}

void UAbilityComponent::HandleResourceConsumption(const FAbilityData& AbilityData)
{
    if (UCombatComponent* Combat = GetOwner()->FindComponentByClass<UCombatComponent>())
    {
        Combat->ConsumeEnergy(AbilityData.ResourceCost);
    }
}

void UAbilityComponent::PlayAbilityEffects(const FAbilityData& AbilityData)
{
    // Spawn VFX
    if (AbilityData.VisualEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(
            AbilityData.VisualEffect,
            GetOwner()->GetRootComponent()
        );
    }

    // Play sound
    if (AbilityData.SoundEffect)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            AbilityData.SoundEffect,
            GetOwner()->GetActorLocation()
        );
    }
}

void UAbilityComponent::HandleAbilityFailure(const FName& AbilityName, const FString& Reason)
{
    LogAbilityError(FString::Printf(TEXT("Failed to activate ability %s: %s"), 
        *AbilityName.ToString(), *Reason));
    OnAbilityActivated.Broadcast(AbilityName, false);
}

void UAbilityComponent::UpdateCooldowns(float DeltaTime)
{
    TArray<FName> CompletedCooldowns;

    for (auto& Pair : AbilityCooldowns)
    {
        Pair.Value -= DeltaTime;
        
        if (Pair.Value <= 0.0f)
        {
            CompletedCooldowns.Add(Pair.Key);
        }
        else
        {
            OnCooldownUpdated.Broadcast(Pair.Key, Pair.Value);
        }
    }

    // Remove completed cooldowns
    for (const FName& AbilityName : CompletedCooldowns)
    {
        ClearCooldown(AbilityName);
    }
}

void UAbilityComponent::ClearCooldown(const FName& AbilityName)
{
    AbilityCooldowns.Remove(AbilityName);
    OnCooldownUpdated.Broadcast(AbilityName, 0.0f);
}

bool UAbilityComponent::IsValidAbilityForCurrentState(const FAbilityData& AbilityData) const
{
    return AbilityData.RequiredState == ETimelineState::None || 
           AbilityData.RequiredState == CurrentTimelineState;
}

bool UAbilityComponent::AreResourcesAvailable(const FAbilityData& AbilityData) const
{
    return CheckResourceCosts(AbilityData);
}

void UAbilityComponent::CleanupExpiredEffects()
{
    // Cleanup combo if window expired
    if (!IsComboWindowOpen() && CurrentComboChain.Num() > 0)
    {
        CurrentComboChain.Empty();
        LastComboTime = 0.0f;
    }
}

void UAbilityComponent::ManageAbilityCache()
{
    UpdateDamageCache();
}

void UAbilityComponent::LogAbilityError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Warning, TEXT("Ability Error: %s"), *ErrorMessage);
}

bool UAbilityComponent::ValidateAbilityData(const FAbilityData& AbilityData) const
{
    if (AbilityData.Name.IsNone())
    {
        return false;
    }

    if (AbilityData.BaseDamage < 0.0f || AbilityData.ResourceCost < 0.0f)
    {
        return false;
    }

    return true;
}

void UAbilityComponent::UpdateDamageCache()
{
    DamageCache.Empty();

    for (const auto& Pair : LearnedAbilities)
    {
        const FAbilityData& AbilityData = Pair.Value;
        float ModifiedDamage = AbilityData.BaseDamage * CalculateTimelineBonus(AbilityData);
        DamageCache.Add(AbilityData.Name, ModifiedDamage);
    }
}
