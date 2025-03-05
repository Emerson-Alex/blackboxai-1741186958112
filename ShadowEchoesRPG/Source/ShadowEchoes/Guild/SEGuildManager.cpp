// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Guild/SEGuildManager.h"
#include "Core/SEGameInstance.h"
#include "Systems/TimelineManager.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

USEGuildManager::USEGuildManager()
    : MaxGuildLevel(50)
    , BaseMaxMembers(50)
    , MissionCheckInterval(60.0f)
{
}

void USEGuildManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    if (GameInstance)
    {
        TimelineManager = GameInstance->GetTimelineManager();
    }

    // Start mission check loop
    FTimerHandle MissionTimer;
    GetWorld()->GetTimerManager().SetTimer(
        MissionTimer,
        this,
        &USEGuildManager::ProcessGuildMissions,
        MissionCheckInterval,
        true
    );

    // Load guild data
    UDataTable* GuildTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_GuildContent")));
    if (GuildTable)
    {
        // Initialize guild features and upgrades
    }
}

bool USEGuildManager::CreateGuild(const FString& GuildName, const FString& FounderID, ETimelineState PreferredTimeline)
{
    if (!ValidateGuildCreation(GuildName, FounderID))
    {
        return false;
    }

    // Create guild data
    FGuildData NewGuild;
    NewGuild.GuildID = FGuid::NewGuid().ToString();
    NewGuild.GuildName = GuildName;
    NewGuild.PreferredTimeline = PreferredTimeline;
    NewGuild.Level = 1;
    NewGuild.MaxMembers = BaseMaxMembers;
    NewGuild.Members.Add(FounderID);
    NewGuild.MemberRanks.Add(FounderID, EGuildRank::GuildMaster);
    NewGuild.TimelinePower = 0;

    // Create guild hall
    FGuildHall NewHall;
    NewHall.HallID = FGuid::NewGuid().ToString();
    NewHall.Timeline = PreferredTimeline;
    NewGuild.GuildHallID = NewHall.HallID;

    // Store guild data
    Guilds.Add(NewGuild.GuildID, NewGuild);
    GuildHalls.Add(NewHall.HallID, NewHall);

    // Notify creation
    OnGuildCreated.Broadcast(NewGuild);
    BP_OnGuildCreated(NewGuild);

    return true;
}

bool USEGuildManager::JoinGuild(const FString& PlayerID, const FString& GuildID)
{
    if (!Guilds.Contains(GuildID))
    {
        return false;
    }

    FGuildData& Guild = Guilds[GuildID];

    // Check member limit
    if (Guild.Members.Num() >= Guild.MaxMembers)
    {
        return false;
    }

    // Add member
    Guild.Members.AddUnique(PlayerID);
    Guild.MemberRanks.Add(PlayerID, EGuildRank::Member);

    // Apply timeline effects
    if (TimelineManager)
    {
        ETimelineState PlayerTimeline = TimelineManager->GetPlayerTimeline(PlayerID);
        if (PlayerTimeline == Guild.PreferredTimeline)
        {
            UpdateGuildTimelinePower(GuildID, 10);  // Timeline alignment bonus
        }
    }

    return true;
}

void USEGuildManager::LeaveGuild(const FString& PlayerID, const FString& GuildID)
{
    if (!Guilds.Contains(GuildID))
    {
        return;
    }

    FGuildData& Guild = Guilds[GuildID];

    // Remove member
    Guild.Members.Remove(PlayerID);
    Guild.MemberRanks.Remove(PlayerID);

    // Check if guild should be disbanded
    if (Guild.Members.Num() == 0)
    {
        Guilds.Remove(GuildID);
        if (!Guild.GuildHallID.IsEmpty())
        {
            GuildHalls.Remove(Guild.GuildHallID);
        }
    }
    else if (Guild.MemberRanks[PlayerID] == EGuildRank::GuildMaster)
    {
        // Promote new guild master
        if (Guild.Members.Num() > 0)
        {
            FString NewMaster = Guild.Members[0];
            Guild.MemberRanks[NewMaster] = EGuildRank::GuildMaster;
        }
    }
}

bool USEGuildManager::StartGuildMission(const FString& GuildID, const FString& MissionID)
{
    if (!ValidateGuildMission(GuildID, MissionID))
    {
        return false;
    }

    // Load mission data
    UDataTable* MissionTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_GuildContent")));
    if (!MissionTable)
    {
        return false;
    }

    FGuildMission* Mission = MissionTable->FindRow<FGuildMission>(FName(*MissionID), "");
    if (!Mission)
    {
        return false;
    }

    // Check timeline requirements
    if (Mission->RequiredTimeline != ETimelineState::Any)
    {
        FGuildData& Guild = Guilds[GuildID];
        if (Guild.PreferredTimeline != Mission->RequiredTimeline)
        {
            return false;
        }
    }

    // Start mission
    ActiveMissions.Add(MissionID, *Mission);

    // Set mission timer
    FTimerHandle MissionTimer;
    GetWorld()->GetTimerManager().SetTimer(
        MissionTimer,
        [this, GuildID, MissionID]()
        {
            CompleteGuildMission(GuildID, MissionID);
        },
        Mission->Duration,
        false
    );

    return true;
}

void USEGuildManager::CompleteGuildMission(const FString& GuildID, const FString& MissionID)
{
    if (!ActiveMissions.Contains(MissionID))
    {
        return;
    }

    FGuildMission Mission = ActiveMissions[MissionID];
    FGuildData& Guild = Guilds[GuildID];

    // Grant rewards
    for (const FName& Reward : Mission.Rewards)
    {
        // Process rewards
        // Similar to Dark Souls covenant rewards
    }

    // Add guild experience
    AddGuildExperience(GuildID, 1000);  // Base experience

    // Update timeline power
    if (Mission.RequiredTimeline != ETimelineState::Any && 
        Mission.RequiredTimeline == Guild.PreferredTimeline)
    {
        UpdateGuildTimelinePower(GuildID, 50);  // Timeline alignment bonus
    }

    // Notify completion
    OnGuildMissionCompleted.Broadcast(GuildID, Mission);
    BP_OnGuildMissionCompleted(GuildID, Mission);

    // Cleanup
    ActiveMissions.Remove(MissionID);
}

bool USEGuildManager::UpgradeGuildHall(const FString& GuildID, const FName& UpgradeID)
{
    if (!Guilds.Contains(GuildID))
    {
        return false;
    }

    FGuildData& Guild = Guilds[GuildID];
    if (!GuildHalls.Contains(Guild.GuildHallID))
    {
        return false;
    }

    FGuildHall& Hall = GuildHalls[Guild.GuildHallID];

    // Check requirements
    // Similar to Dark Souls covenant ranking requirements

    // Apply upgrade
    Hall.Upgrades.AddUnique(UpgradeID);
    UpdateGuildBonuses(GuildID);

    // Notify upgrade
    OnGuildHallUpgraded.Broadcast(GuildID, UpgradeID);
    BP_OnGuildHallUpgraded(GuildID, UpgradeID);

    return true;
}

void USEGuildManager::UnlockGuildFeature(const FString& GuildID, const FName& FeatureID)
{
    if (!Guilds.Contains(GuildID))
    {
        return;
    }

    FGuildData& Guild = Guilds[GuildID];
    Guild.UnlockedFeatures.AddUnique(FeatureID);

    // Apply feature effects
    UpdateGuildBonuses(GuildID);
}

void USEGuildManager::OnTimelineStateChanged(ETimelineState NewState)
{
    // Update guild effects based on timeline
    CheckTimelineEffects(NewState);
    BP_OnTimelineStateChanged(NewState);
}

void USEGuildManager::AddGuildExperience(const FString& GuildID, int32 Experience)
{
    if (!Guilds.Contains(GuildID))
    {
        return;
    }

    FGuildData& Guild = Guilds[GuildID];

    // Calculate level up
    int32 RequiredExp = Guild.Level * 1000;  // Basic progression formula
    
    // Apply timeline bonus
    if (TimelineManager && TimelineManager->GetCurrentState() == Guild.PreferredTimeline)
    {
        Experience *= 1.5f;  // Timeline alignment bonus
    }

    // Update guild level
    while (Experience >= RequiredExp && Guild.Level < MaxGuildLevel)
    {
        Experience -= RequiredExp;
        Guild.Level++;
        Guild.MaxMembers = BaseMaxMembers + (Guild.Level - 1) * 5;
        
        // Unlock features based on level
        if (Guild.Level % 10 == 0)  // Major upgrades every 10 levels
        {
            UnlockGuildFeature(GuildID, *FString::Printf(TEXT("Level_%d_Feature"), Guild.Level));
        }
    }
}

void USEGuildManager::UpdateGuildTimelinePower(const FString& GuildID, int32 PowerChange)
{
    if (!Guilds.Contains(GuildID))
    {
        return;
    }

    FGuildData& Guild = Guilds[GuildID];
    Guild.TimelinePower += PowerChange;

    // Apply power effects
    int32 PowerBonus = CalculateGuildPowerBonus(Guild);
    
    // Update guild hall effects
    if (GuildHalls.Contains(Guild.GuildHallID))
    {
        ApplyGuildHallEffects(GuildHalls[Guild.GuildHallID]);
    }
}

bool USEGuildManager::ValidateGuildCreation(const FString& GuildName, const FString& FounderID) const
{
    // Check name availability
    for (const auto& Pair : Guilds)
    {
        if (Pair.Value.GuildName == GuildName)
        {
            return false;
        }
    }

    // Check founder requirements
    if (!GameInstance)
    {
        return false;
    }

    // Check timeline mastery
    if (TimelineManager)
    {
        int32 TimelineMastery = TimelineManager->GetTimelineMastery(TimelineManager->GetCurrentState());
        if (TimelineMastery < 30)  // Require significant timeline mastery
        {
            return false;
        }
    }

    return true;
}

bool USEGuildManager::ValidateGuildMission(const FString& GuildID, const FString& MissionID) const
{
    if (!Guilds.Contains(GuildID))
    {
        return false;
    }

    const FGuildData& Guild = Guilds[GuildID];

    // Check active missions limit
    int32 ActiveCount = 0;
    for (const auto& Pair : ActiveMissions)
    {
        if (Pair.Value.RequiredTimeline == Guild.PreferredTimeline)
        {
            ActiveCount++;
        }
    }

    return ActiveCount < Guild.Level / 10 + 1;  // Scale with guild level
}

void USEGuildManager::ProcessGuildMissions()
{
    // Update active missions
    TArray<FString> CompletedMissions;

    for (const auto& Pair : ActiveMissions)
    {
        // Check mission progress
        // Similar to Dark Souls covenant progress tracking
    }

    // Complete finished missions
    for (const FString& MissionID : CompletedMissions)
    {
        // Find guild ID for mission
        for (const auto& GuildPair : Guilds)
        {
            CompleteGuildMission(GuildPair.Key, MissionID);
        }
    }
}

void USEGuildManager::UpdateGuildBonuses(const FString& GuildID)
{
    if (!Guilds.Contains(GuildID))
    {
        return;
    }

    const FGuildData& Guild = Guilds[GuildID];
    if (!GuildHalls.Contains(Guild.GuildHallID))
    {
        return;
    }

    FGuildHall& Hall = GuildHalls[Guild.GuildHallID];

    // Calculate bonuses
    TMap<FName, float> NewBonuses;
    
    // Base bonuses from guild level
    NewBonuses.Add("TimelinePower", Guild.Level * 2.0f);
    
    // Feature bonuses
    for (const FName& Feature : Guild.UnlockedFeatures)
    {
        // Add feature-specific bonuses
    }

    // Upgrade bonuses
    for (const FName& Upgrade : Hall.Upgrades)
    {
        // Add upgrade-specific bonuses
    }

    // Timeline alignment bonus
    if (TimelineManager && TimelineManager->GetCurrentState() == Guild.PreferredTimeline)
    {
        for (auto& Bonus : NewBonuses)
        {
            Bonus.Value *= 1.5f;
        }
    }

    Hall.Bonuses = NewBonuses;
}

void USEGuildManager::CheckTimelineEffects(ETimelineState NewState)
{
    // Update all guilds
    for (auto& Pair : Guilds)
    {
        FGuildData& Guild = Pair.Value;
        
        // Apply timeline effects
        if (Guild.PreferredTimeline == NewState)
        {
            UpdateGuildTimelinePower(Pair.Key, 25);  // Timeline alignment bonus
        }
        else if (Guild.PreferredTimeline != ETimelineState::Any)
        {
            UpdateGuildTimelinePower(Pair.Key, -10);  // Timeline misalignment penalty
        }

        // Update guild hall effects
        if (GuildHalls.Contains(Guild.GuildHallID))
        {
            ApplyGuildHallEffects(GuildHalls[Guild.GuildHallID]);
        }
    }
}

bool USEGuildManager::CanAccessGuildHall(const FString& GuildID, const FString& PlayerID) const
{
    if (!Guilds.Contains(GuildID))
    {
        return false;
    }

    const FGuildData& Guild = Guilds[GuildID];
    
    // Check membership
    if (!Guild.Members.Contains(PlayerID))
    {
        return false;
    }

    // Check timeline state
    if (TimelineManager)
    {
        ETimelineState PlayerTimeline = TimelineManager->GetPlayerTimeline(PlayerID);
        if (Guild.PreferredTimeline != ETimelineState::Any && 
            PlayerTimeline != Guild.PreferredTimeline)
        {
            return false;
        }
    }

    return true;
}

void USEGuildManager::ApplyGuildHallEffects(const FGuildHall& Hall)
{
    // Apply hall bonuses to members
    // Similar to Dark Souls covenant bonuses
}

int32 USEGuildManager::CalculateGuildPowerBonus(const FGuildData& Guild) const
{
    // Calculate power bonus based on:
    // - Guild level
    // - Timeline alignment
    // - Active missions
    // - Member count
    // Similar to Dark Souls covenant ranking

    int32 Bonus = Guild.Level * 10;
    
    if (TimelineManager && TimelineManager->GetCurrentState() == Guild.PreferredTimeline)
    {
        Bonus *= 1.5f;
    }

    return Bonus;
}
