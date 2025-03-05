// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Combat/SEAbilityManager.h"
#include "Core/SEGameInstance.h"
#include "Kismet/GameplayStatics.h"

USEAbilityManager::USEAbilityManager()
    : ComboTimeoutDuration(2.0f)
    , MaxComboLength(4)
    , bIsInCombo(false)
    , CurrentComboAbilityID(NAME_None)
    , LastComboTime(0.0f)
    , CurrentComboHits(0)
    , CurrentTimelineState(ETimelineState::BrightWorld)
{
}

void USEAbilityManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
}

bool USEAbilityManager::UnlockAbility(const FAbilityInfo& Ability)
{
    if (!ValidateAbility(Ability))
    {
        return false;
    }

    // Add to unlocked abilities
    UnlockedAbilities.Add(Ability.AbilityID, Ability);

    // Notify events
    OnAbilityUnlocked.Broadcast(Ability);
    BP_OnAbilityUnlocked(Ability);

    return true;
}

bool USEAbilityManager::HasAbility(const FName& AbilityID) const
{
    return UnlockedAbilities.Contains(AbilityID);
}

const FAbilityInfo* USEAbilityManager::GetAbilityInfo(const FName& AbilityID) const
{
    return UnlockedAbilities.Find(AbilityID);
}

bool USEAbilityManager::CanUseAbility(const FName& AbilityID) const
{
    const FAbilityInfo* Ability = GetAbilityInfo(AbilityID);
    if (!Ability)
    {
        return false;
    }

    // Check requirements
    if (!CheckAbilityRequirements(*Ability))
    {
        return false;
    }

    // Check cooldown
    if (IsOnCooldown(AbilityID))
    {
        return false;
    }

    // Check timeline requirement
    if (!CheckTimelineRequirement(*Ability))
    {
        return false;
    }

    // Check energy cost if game instance exists
    if (GameInstance && GameInstance->GetTimelineEnergy() < Ability->EnergyCost)
    {
        return false;
    }

    return true;
}

bool USEAbilityManager::UseAbility(const FName& AbilityID)
{
    if (!CanUseAbility(AbilityID))
    {
        return false;
    }

    const FAbilityInfo* Ability = GetAbilityInfo(AbilityID);
    if (!Ability)
    {
        return false;
    }

    // Consume energy
    if (GameInstance)
    {
        GameInstance->ConsumeTimelineEnergy(Ability->EnergyCost);
    }

    // Start cooldown
    StartCooldown(AbilityID);

    // Check for combo
    if (bIsInCombo)
    {
        if (!ContinueCombo(AbilityID))
        {
            EndCombo();
        }
    }
    else if (Ability->bCanStartCombo)
    {
        StartCombo(AbilityID);
    }

    return true;
}

bool USEAbilityManager::StartCombo(const FName& AbilityID)
{
    const FAbilityInfo* Ability = GetAbilityInfo(AbilityID);
    if (!Ability || !Ability->bCanStartCombo)
    {
        return false;
    }

    // Initialize combo
    bIsInCombo = true;
    CurrentComboAbilityID = AbilityID;
    CurrentComboChain.Empty();
    CurrentComboChain.Add(AbilityID);
    LastComboTime = UGameplayStatics::GetTimeSeconds(this);
    CurrentComboHits = 1;

    // Notify events
    OnComboStarted.Broadcast(*Ability);
    BP_OnComboStarted(*Ability);

    return true;
}

bool USEAbilityManager::ContinueCombo(const FName& AbilityID)
{
    if (!bIsInCombo)
    {
        return false;
    }

    // Check combo timeout
    CheckComboTimeout();
    if (!bIsInCombo)
    {
        return false;
    }

    // Validate combo
    if (!ValidateCombo(AbilityID))
    {
        EndCombo();
        return false;
    }

    // Add to combo chain
    CurrentComboChain.Add(AbilityID);
    LastComboTime = UGameplayStatics::GetTimeSeconds(this);
    CurrentComboHits++;

    // Process combo hit
    ProcessComboHit();

    return true;
}

void USEAbilityManager::EndCombo()
{
    if (!bIsInCombo)
    {
        return;
    }

    // Check if combo was successful
    const FAbilityInfo* ComboAbility = GetAbilityInfo(CurrentComboAbilityID);
    if (ComboAbility && CurrentComboHits >= ComboAbility->ComboRequirement.RequiredHits)
    {
        OnComboCompleted.Broadcast(*ComboAbility);
        BP_OnComboCompleted(*ComboAbility);
    }
    else
    {
        OnComboFailed.Broadcast();
        BP_OnComboFailed();
    }

    // Reset combo state
    ResetCombo();
}

void USEAbilityManager::OnTimelineStateChanged(ETimelineState NewState)
{
    CurrentTimelineState = NewState;

    // End any active combos
    if (bIsInCombo)
    {
        EndCombo();
    }

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

TArray<FAbilityInfo> USEAbilityManager::GetUnlockableAbilities(const FName& BossID) const
{
    TArray<FAbilityInfo> UnlockableAbilities;

    // TODO: Implement boss ability unlocks based on boss data table
    // This would check which abilities are available from the defeated boss

    return UnlockableAbilities;
}

bool USEAbilityManager::CanUnlockBossAbility(const FName& AbilityID) const
{
    // TODO: Implement boss ability unlock requirements
    // This would check if the player meets the requirements to unlock this ability

    return false;
}

bool USEAbilityManager::ValidateAbility(const FAbilityInfo& Ability) const
{
    // Check for valid ID
    if (Ability.AbilityID.IsNone())
    {
        return false;
    }

    // Check if already unlocked
    if (HasAbility(Ability.AbilityID))
    {
        return false;
    }

    return true;
}

bool USEAbilityManager::CheckAbilityRequirements(const FAbilityInfo& Ability) const
{
    // Check level requirement if game instance exists
    if (GameInstance && GameInstance->GetPlayerLevel() < Ability.RequiredLevel)
    {
        return false;
    }

    return true;
}

bool USEAbilityManager::CheckTimelineRequirement(const FAbilityInfo& Ability) const
{
    return Ability.RequiredTimeline == ETimelineState::Any || Ability.RequiredTimeline == CurrentTimelineState;
}

bool USEAbilityManager::ValidateCombo(const FName& AbilityID) const
{
    const FAbilityInfo* ComboAbility = GetAbilityInfo(CurrentComboAbilityID);
    if (!ComboAbility)
    {
        return false;
    }

    // Check combo requirements
    if (!CheckComboRequirements(*ComboAbility))
    {
        return false;
    }

    // Check if ability is valid for combo
    if (!ComboAbility->ComboRequirement.RequiredAbilities.Contains(AbilityID))
    {
        return false;
    }

    // Check combo length
    if (CurrentComboChain.Num() >= MaxComboLength)
    {
        return false;
    }

    return true;
}

bool USEAbilityManager::CheckComboRequirements(const FAbilityInfo& Ability) const
{
    // Check time window
    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    if (CurrentTime - LastComboTime > Ability.ComboRequirement.TimeWindow)
    {
        return false;
    }

    return true;
}

void USEAbilityManager::ProcessComboHit()
{
    const FAbilityInfo* ComboAbility = GetAbilityInfo(CurrentComboAbilityID);
    if (!ComboAbility)
    {
        return;
    }

    // Check for combo completion
    if (CurrentComboHits >= ComboAbility->ComboRequirement.RequiredHits)
    {
        EndCombo();
    }
}

void USEAbilityManager::CheckComboTimeout()
{
    const FAbilityInfo* ComboAbility = GetAbilityInfo(CurrentComboAbilityID);
    if (!ComboAbility)
    {
        EndCombo();
        return;
    }

    float CurrentTime = UGameplayStatics::GetTimeSeconds(this);
    if (CurrentTime - LastComboTime > ComboAbility->ComboRequirement.TimeWindow)
    {
        EndCombo();
    }
}

void USEAbilityManager::ResetCombo()
{
    bIsInCombo = false;
    CurrentComboAbilityID = NAME_None;
    CurrentComboChain.Empty();
    LastComboTime = 0.0f;
    CurrentComboHits = 0;
}

void USEAbilityManager::UpdateCooldowns(float DeltaTime)
{
    TArray<FName> CompletedCooldowns;

    // Update all cooldowns
    for (auto& Pair : AbilityCooldowns)
    {
        Pair.Value -= DeltaTime;
        if (Pair.Value <= 0.0f)
        {
            CompletedCooldowns.Add(Pair.Key);
        }
    }

    // Remove completed cooldowns
    for (const FName& AbilityID : CompletedCooldowns)
    {
        AbilityCooldowns.Remove(AbilityID);
    }
}

void USEAbilityManager::StartCooldown(const FName& AbilityID)
{
    const FAbilityInfo* Ability = GetAbilityInfo(AbilityID);
    if (Ability)
    {
        AbilityCooldowns.Add(AbilityID, Ability->Cooldown);
    }
}

bool USEAbilityManager::IsOnCooldown(const FName& AbilityID) const
{
    return AbilityCooldowns.Contains(AbilityID);
}

float USEAbilityManager::GetRemainingCooldown(const FName& AbilityID) const
{
    const float* Cooldown = AbilityCooldowns.Find(AbilityID);
    return Cooldown ? *Cooldown : 0.0f;
}
