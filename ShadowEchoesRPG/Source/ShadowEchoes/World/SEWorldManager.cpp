// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "World/SEWorldManager.h"
#include "Core/SEGameInstance.h"
#include "Systems/TimelineManager.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

USEWorldManager::USEWorldManager()
    : EventCheckInterval(30.0f)
    , BossRespawnMultiplier(1.5f)
    , MaxActiveEvents(3)
{
}

void USEWorldManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    if (GameInstance)
    {
        TimelineManager = GameInstance->GetTimelineManager();
    }

    LoadWorldData();

    // Start world state update loop
    FTimerHandle UpdateTimer;
    GetWorld()->GetTimerManager().SetTimer(
        UpdateTimer,
        this,
        &USEWorldManager::UpdateWorldState,
        EventCheckInterval,
        true
    );
}

void USEWorldManager::LoadWorldData()
{
    // Load area data
    UDataTable* AreaTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WorldAreas")));
    if (AreaTable)
    {
        TArray<FWorldArea*> AreaRows;
        AreaTable->GetAllRows<FWorldArea>("", AreaRows);
        for (const FWorldArea* Area : AreaRows)
        {
            if (Area)
            {
                Areas.Add(Area->AreaID, *Area);
            }
        }
    }

    // Load event data
    UDataTable* EventTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WorldEvents")));
    if (EventTable)
    {
        // Initialize events (not active by default)
    }

    // Load world boss data
    UDataTable* BossTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WorldBosses")));
    if (BossTable)
    {
        // Initialize world bosses (not spawned by default)
    }

    // Unlock starting area
    UnlockArea(FName("StartingArea"));
}

void USEWorldManager::UpdateWorldState()
{
    // Update active events
    ProcessEventCooldowns();

    // Update world bosses
    ManageWorldBosses();

    // Update area difficulty based on player level and timeline
    UpdateAreaDifficulty();

    // Check for potential new events
    if (ActiveEvents.Num() < MaxActiveEvents)
    {
        // Randomly trigger new events based on area and timeline state
    }
}

const FWorldArea* USEWorldManager::GetAreaInfo(const FName& AreaID) const
{
    return Areas.Find(AreaID);
}

void USEWorldManager::UnlockArea(const FName& AreaID)
{
    const FWorldArea* Area = GetAreaInfo(AreaID);
    if (!Area)
    {
        return;
    }

    // Check if player meets requirements
    if (GameInstance && GameInstance->GetPlayerLevel() < Area->RecommendedLevel)
    {
        return;
    }

    // Check if area is connected to already unlocked areas
    bool bHasValidConnection = false;
    for (const FName& ConnectedArea : Area->ConnectedAreas)
    {
        if (IsAreaUnlocked(ConnectedArea))
        {
            bHasValidConnection = true;
            break;
        }
    }

    if (!bHasValidConnection && AreaID != FName("StartingArea"))
    {
        return;
    }

    UnlockedAreas.Add(AreaID);

    // FromSoftware-style: Reveal connected areas but keep them locked
    for (const FName& ConnectedArea : Area->ConnectedAreas)
    {
        // Show area on map but keep locked until requirements are met
    }
}

bool USEWorldManager::IsAreaUnlocked(const FName& AreaID) const
{
    return UnlockedAreas.Contains(AreaID);
}

void USEWorldManager::TriggerWorldEvent(const FName& EventID)
{
    if (ActiveEvents.Contains(EventID))
    {
        return;
    }

    // Load event data
    UDataTable* EventTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WorldEvents")));
    if (!EventTable)
    {
        return;
    }

    FWorldEvent* Event = EventTable->FindRow<FWorldEvent>(EventID, "");
    if (!Event)
    {
        return;
    }

    // Check timeline requirements
    if (Event->RequiredTimeline != ETimelineState::Any && 
        TimelineManager && 
        TimelineManager->GetCurrentState() != Event->RequiredTimeline)
    {
        return;
    }

    // Start event
    ActiveEvents.Add(EventID, *Event);

    // Spawn event-specific content
    for (const FName& BossID : Event->SpawnableBosses)
    {
        SpawnWorldBoss(BossID);
    }

    // Notify event start
    OnWorldEventStarted.Broadcast(*Event);
    BP_OnWorldEventStarted(*Event);
}

void USEWorldManager::EndWorldEvent(const FName& EventID)
{
    if (!ActiveEvents.Contains(EventID))
    {
        return;
    }

    // Clean up event content
    FWorldEvent Event = ActiveEvents[EventID];
    for (const FName& BossID : Event.SpawnableBosses)
    {
        DespawnWorldBoss(BossID);
    }

    ActiveEvents.Remove(EventID);
}

bool USEWorldManager::IsEventActive(const FName& EventID) const
{
    return ActiveEvents.Contains(EventID);
}

void USEWorldManager::SpawnWorldBoss(const FName& BossID)
{
    if (ActiveBosses.Contains(BossID))
    {
        return;
    }

    // Load boss data
    UDataTable* BossTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WorldBosses")));
    if (!BossTable)
    {
        return;
    }

    FWorldBoss* Boss = BossTable->FindRow<FWorldBoss>(BossID, "");
    if (!Boss)
    {
        return;
    }

    // FromSoftware-style: Choose spawn location based on player position and area state
    FName SpawnArea;
    for (const FName& PossibleArea : Boss->PossibleSpawnAreas)
    {
        if (IsAreaUnlocked(PossibleArea))
        {
            SpawnArea = PossibleArea;
            break;
        }
    }

    if (SpawnArea.IsNone())
    {
        return;
    }

    // Spawn boss
    ActiveBosses.Add(BossID, *Boss);

    // Notify boss spawn
    OnWorldBossSpawned.Broadcast(*Boss);
    BP_OnWorldBossSpawned(*Boss);
}

void USEWorldManager::DespawnWorldBoss(const FName& BossID)
{
    if (!ActiveBosses.Contains(BossID))
    {
        return;
    }

    ActiveBosses.Remove(BossID);
}

bool USEWorldManager::IsWorldBossActive(const FName& BossID) const
{
    return ActiveBosses.Contains(BossID);
}

void USEWorldManager::DiscoverSecret(const FName& AreaID, const FName& SecretID)
{
    const FWorldArea* Area = GetAreaInfo(AreaID);
    if (!Area || !Area->HiddenSecrets.Contains(SecretID))
    {
        return;
    }

    FName CombinedID = *FString::Printf(TEXT("%s_%s"), *AreaID.ToString(), *SecretID.ToString());
    if (DiscoveredSecrets.Contains(CombinedID))
    {
        return;
    }

    // FromSoftware-style: Secrets have meaningful impact
    DiscoveredSecrets.Add(CombinedID);

    // Notify secret discovery
    OnSecretDiscovered.Broadcast(AreaID, SecretID);
    BP_OnSecretDiscovered(AreaID, SecretID);
}

bool USEWorldManager::IsSecretDiscovered(const FName& AreaID, const FName& SecretID) const
{
    FName CombinedID = *FString::Printf(TEXT("%s_%s"), *AreaID.ToString(), *SecretID.ToString());
    return DiscoveredSecrets.Contains(CombinedID);
}

void USEWorldManager::OnTimelineStateChanged(ETimelineState NewState)
{
    // Update world state based on timeline
    CheckTimelineSpecificContent(NewState);

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

void USEWorldManager::ProcessEventCooldowns()
{
    TArray<FName> ExpiredEvents;

    for (const auto& Pair : ActiveEvents)
    {
        // Check event duration and cooldown
        if (/* Event duration expired */)
        {
            ExpiredEvents.Add(Pair.Key);
        }
    }

    for (const FName& EventID : ExpiredEvents)
    {
        EndWorldEvent(EventID);
    }
}

void USEWorldManager::ManageWorldBosses()
{
    // FromSoftware-style: World bosses have complex behaviors
    for (const auto& Pair : ActiveBosses)
    {
        const FWorldBoss& Boss = Pair.Value;
        if (Boss.bIsRoaming)
        {
            // Update roaming behavior
        }
    }
}

bool USEWorldManager::ValidateAreaConnection(const FName& FromArea, const FName& ToArea) const
{
    const FWorldArea* Area = GetAreaInfo(FromArea);
    return Area && Area->ConnectedAreas.Contains(ToArea);
}

void USEWorldManager::CheckTimelineSpecificContent(ETimelineState NewState)
{
    // Update area accessibility based on timeline
    for (const auto& Pair : Areas)
    {
        const FWorldArea& Area = Pair.Value;
        if (Area.PreferredTimeline != ETimelineState::Any && Area.PreferredTimeline != NewState)
        {
            // Modify area difficulty or accessibility
        }
    }

    // Update active events
    TArray<FName> InvalidEvents;
    for (const auto& Pair : ActiveEvents)
    {
        if (Pair.Value.RequiredTimeline != ETimelineState::Any && Pair.Value.RequiredTimeline != NewState)
        {
            InvalidEvents.Add(Pair.Key);
        }
    }

    for (const FName& EventID : InvalidEvents)
    {
        EndWorldEvent(EventID);
    }
}

void USEWorldManager::UpdateAreaDifficulty()
{
    if (!GameInstance)
    {
        return;
    }

    int32 PlayerLevel = GameInstance->GetPlayerLevel();
    ETimelineState CurrentTimeline = TimelineManager ? TimelineManager->GetCurrentState() : ETimelineState::Any;

    // FromSoftware-style: Area difficulty scales with player progress and timeline state
    for (const auto& Pair : Areas)
    {
        const FWorldArea& Area = Pair.Value;
        if (!IsAreaUnlocked(Area.AreaID))
        {
            continue;
        }

        // Calculate area difficulty modifier
        float DifficultyModifier = 1.0f;
        
        // Increase difficulty in non-preferred timeline
        if (Area.PreferredTimeline != ETimelineState::Any && Area.PreferredTimeline != CurrentTimeline)
        {
            DifficultyModifier *= 1.5f;
        }

        // Scale with player level
        if (PlayerLevel > Area.RecommendedLevel)
        {
            DifficultyModifier *= (1.0f + (PlayerLevel - Area.RecommendedLevel) * 0.1f);
        }

        // Apply difficulty modifier to area enemies and events
    }
}
