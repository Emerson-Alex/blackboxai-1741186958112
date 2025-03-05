// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Combat/SEBossManager.h"
#include "Combat/SEAbilityManager.h"
#include "Core/SEGameInstance.h"
#include "Engine/DataTable.h"

USEBossManager::USEBossManager()
    : PhaseTransitionDuration(2.0f)
    , TimelinePreferenceBonus(1.25f)
    , bIsBossFightActive(false)
    , CurrentBossID(NAME_None)
    , CurrentPhaseIndex(-1)
    , CurrentHealthPercent(1.0f)
{
}

void USEBossManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    if (GameInstance)
    {
        AbilityManager = GameInstance->GetAbilityManager();
    }

    // Load boss data
    UDataTable* BossTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_BossFights")));
    if (BossTable)
    {
        TArray<FBossInfo*> BossRows;
        BossTable->GetAllRows<FBossInfo>("", BossRows);
        for (const FBossInfo* Boss : BossRows)
        {
            if (Boss)
            {
                BossDatabase.Add(Boss->BossID, *Boss);
            }
        }
    }
}

bool USEBossManager::StartBossFight(const FName& BossID)
{
    if (!ValidateBoss(BossID))
    {
        return false;
    }

    const FBossInfo* Boss = GetBossInfo(BossID);
    if (!Boss || !CheckBossRequirements(*Boss))
    {
        return false;
    }

    // Initialize boss fight
    bIsBossFightActive = true;
    CurrentBossID = BossID;
    CurrentHealthPercent = 1.0f;

    // Initialize phases
    InitializePhases();

    return true;
}

void USEBossManager::EndBossFight(bool bWasVictorious)
{
    if (!bIsBossFightActive)
    {
        return;
    }

    // Handle victory rewards
    if (bWasVictorious)
    {
        // Check for ability unlocks
        CheckAbilityUnlocks();

        // Notify defeat
        OnBossDefeated.Broadcast(CurrentBossID);
        BP_OnBossDefeated(CurrentBossID);
    }

    // Reset state
    bIsBossFightActive = false;
    CurrentBossID = NAME_None;
    CurrentPhaseIndex = -1;
    CurrentHealthPercent = 1.0f;
}

const FBossInfo* USEBossManager::GetBossInfo(const FName& BossID) const
{
    return BossDatabase.Find(BossID);
}

const FBossPhaseInfo* USEBossManager::GetCurrentPhase() const
{
    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss || CurrentPhaseIndex < 0 || CurrentPhaseIndex >= Boss->Phases.Num())
    {
        return nullptr;
    }

    return &Boss->Phases[CurrentPhaseIndex];
}

void USEBossManager::UpdateBossHealth(float NewHealthPercent)
{
    if (!bIsBossFightActive)
    {
        return;
    }

    CurrentHealthPercent = FMath::Clamp(NewHealthPercent, 0.0f, 1.0f);

    // Check for phase transition
    CheckPhaseTransition();
}

void USEBossManager::AdvanceToNextPhase()
{
    if (!bIsBossFightActive)
    {
        return;
    }

    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss)
    {
        return;
    }

    // Move to next phase
    int32 NextPhase = CurrentPhaseIndex + 1;
    if (NextPhase < Boss->Phases.Num())
    {
        TransitionToPhase(NextPhase);
    }
}

void USEBossManager::OnTimelineStateChanged(ETimelineState NewState)
{
    // Update current phase if in preferred timeline
    if (bIsBossFightActive)
    {
        const FBossPhaseInfo* CurrentPhase = GetCurrentPhase();
        if (CurrentPhase)
        {
            ApplyPhaseModifiers(*CurrentPhase);
        }
    }

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

TArray<FAbilityInfo> USEBossManager::GetUnlockableAbilities() const
{
    TArray<FAbilityInfo> UnlockableAbilities;

    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss || !AbilityManager)
    {
        return UnlockableAbilities;
    }

    // Get abilities that can be unlocked
    for (const FName& AbilityID : Boss->UnlockableAbilities)
    {
        if (const FAbilityInfo* Ability = AbilityManager->GetAbilityInfo(AbilityID))
        {
            if (!AbilityManager->HasAbility(AbilityID))
            {
                UnlockableAbilities.Add(*Ability);
            }
        }
    }

    return UnlockableAbilities;
}

bool USEBossManager::UnlockBossAbility(const FName& AbilityID)
{
    if (!ValidateAbilityUnlock(AbilityID))
    {
        return false;
    }

    // Unlock ability through ability manager
    if (AbilityManager)
    {
        const FAbilityInfo* Ability = AbilityManager->GetAbilityInfo(AbilityID);
        if (Ability)
        {
            return AbilityManager->UnlockAbility(*Ability);
        }
    }

    return false;
}

bool USEBossManager::ValidateBoss(const FName& BossID) const
{
    if (BossID.IsNone() || !BossDatabase.Contains(BossID))
    {
        return false;
    }

    if (bIsBossFightActive)
    {
        return false;
    }

    return true;
}

bool USEBossManager::CheckBossRequirements(const FBossInfo& Boss) const
{
    // Check level requirement if game instance exists
    if (GameInstance && GameInstance->GetPlayerLevel() < Boss.RequiredLevel)
    {
        return false;
    }

    return true;
}

void USEBossManager::InitializePhases()
{
    CurrentPhaseIndex = -1;

    // Start with first phase
    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (Boss && Boss->Phases.Num() > 0)
    {
        TransitionToPhase(0);
    }
}

void USEBossManager::CheckPhaseTransition()
{
    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss)
    {
        return;
    }

    // Check if health threshold reached for next phase
    for (int32 i = CurrentPhaseIndex + 1; i < Boss->Phases.Num(); ++i)
    {
        if (CurrentHealthPercent <= Boss->Phases[i].HealthThreshold)
        {
            TransitionToPhase(i);
            break;
        }
    }
}

void USEBossManager::TransitionToPhase(int32 PhaseIndex)
{
    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss || PhaseIndex < 0 || PhaseIndex >= Boss->Phases.Num())
    {
        return;
    }

    // Update phase
    CurrentPhaseIndex = PhaseIndex;
    const FBossPhaseInfo& NewPhase = Boss->Phases[PhaseIndex];

    // Apply phase modifiers
    ApplyPhaseModifiers(NewPhase);

    // Notify events
    OnBossPhaseChanged.Broadcast(NewPhase);
    BP_OnBossPhaseChanged(NewPhase);
}

void USEBossManager::ApplyPhaseModifiers(const FBossPhaseInfo& Phase)
{
    // Apply timeline preference bonus
    if (IsInPreferredTimeline())
    {
        // Apply bonus to stats
        FCombatStats ModifiedStats = Phase.PhaseStats;
        float Bonus = CalculateTimelineBonus();
        ModifiedStats.Attack *= Bonus;
        ModifiedStats.Defense *= Bonus;

        // TODO: Apply modified stats to boss character
    }
}

bool USEBossManager::IsInPreferredTimeline() const
{
    const FBossPhaseInfo* CurrentPhase = GetCurrentPhase();
    if (!CurrentPhase || !GameInstance)
    {
        return false;
    }

    return CurrentPhase->PreferredTimeline == GameInstance->GetCurrentTimelineState();
}

float USEBossManager::CalculateTimelineBonus() const
{
    return IsInPreferredTimeline() ? TimelinePreferenceBonus : 1.0f;
}

void USEBossManager::CheckAbilityUnlocks()
{
    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss || !AbilityManager)
    {
        return;
    }

    // Check each unlockable ability
    for (const FName& AbilityID : Boss->UnlockableAbilities)
    {
        if (const FAbilityInfo* Ability = AbilityManager->GetAbilityInfo(AbilityID))
        {
            if (!AbilityManager->HasAbility(AbilityID))
            {
                // Notify ability available
                OnBossAbilityAvailable.Broadcast(CurrentBossID, *Ability);
                BP_OnBossAbilityAvailable(CurrentBossID, *Ability);
            }
        }
    }
}

bool USEBossManager::ValidateAbilityUnlock(const FName& AbilityID) const
{
    if (!bIsBossFightActive || !AbilityManager)
    {
        return false;
    }

    const FBossInfo* Boss = GetBossInfo(CurrentBossID);
    if (!Boss)
    {
        return false;
    }

    // Check if ability is unlockable from this boss
    if (!Boss->UnlockableAbilities.Contains(AbilityID))
    {
        return false;
    }

    // Check if already unlocked
    if (AbilityManager->HasAbility(AbilityID))
    {
        return false;
    }

    return true;
}
