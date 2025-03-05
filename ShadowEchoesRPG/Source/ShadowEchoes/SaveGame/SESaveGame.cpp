// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SaveGame/SESaveGame.h"
#include "Core/SEGameInstance.h"
#include "Systems/QuestManager.h"

USESaveGame::USESaveGame()
    : PlayerLevel(1)
    , PlayerExperience(0)
    , PlayerCurrency(0)
    , CurrentTimelineState(ETimelineState::BrightWorld)
    , MaxHealth(100.0f)
    , MaxTimelineEnergy(100.0f)
    , BaseDamage(10.0f)
    , BrightTimelineMastery(0.0f)
    , DarkTimelineMastery(0.0f)
    , SaveVersion(CurrentSaveVersion)
{
    InitializeDefaults();
}

void USESaveGame::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    // Version handling
    Ar << SaveVersion;
    if (SaveVersion != CurrentSaveVersion)
    {
        UpdateToLatestVersion();
    }

    // Save metadata
    Ar << LastSaveTime;
    Ar << SaveSlotName;

    // Player data
    Ar << PlayerLevel;
    Ar << PlayerExperience;
    Ar << PlayerCurrency;
    Ar << CurrentTimelineState;

    // Quest data
    Ar << QuestStates;

    // Ability data
    Ar << UnlockedAbilities;

    // Inventory data
    Ar << InventoryItems;
    Ar << EquippedItems;

    // Combat stats
    Ar << MaxHealth;
    Ar << MaxTimelineEnergy;
    Ar << BaseDamage;

    // Timeline mastery
    Ar << BrightTimelineMastery;
    Ar << DarkTimelineMastery;

    // Achievement data
    Ar << AchievementProgress;

    // World data
    Ar << DiscoveredLocations;
    Ar << BossFightRecords;

    // Settings
    Ar << GameSettings;

    // Validate data after loading
    if (Ar.IsLoading())
    {
        if (!ValidateData())
        {
            RepairInvalidData();
        }
    }
}

void USESaveGame::InitializeDefaults()
{
    // Set default game settings
    GameSettings.Add(TEXT("MusicVolume"), 0.8f);
    GameSettings.Add(TEXT("SFXVolume"), 1.0f);
    GameSettings.Add(TEXT("CameraShake"), 1.0f);
    GameSettings.Add(TEXT("ScreenShake"), 1.0f);
    GameSettings.Add(TEXT("AutoSave"), 1.0f);

    // Initialize achievement tracking
    AchievementProgress.Add(TEXT("TimelineShifts"), 0.0f);
    AchievementProgress.Add(TEXT("BossesDefeated"), 0.0f);
    AchievementProgress.Add(TEXT("QuestsCompleted"), 0.0f);
    AchievementProgress.Add(TEXT("AbilitiesUnlocked"), 0.0f);

    // Set initial timeline mastery
    BrightTimelineMastery = 0.0f;
    DarkTimelineMastery = 0.0f;

    // Set save metadata
    LastSaveTime = FDateTime::Now();
    SaveSlotName = TEXT("MainSave");
}

void USESaveGame::UpdateToLatestVersion()
{
    while (SaveVersion < CurrentSaveVersion)
    {
        MigrateFromVersion(SaveVersion);
        SaveVersion++;
    }
}

void USESaveGame::MigrateFromVersion(int32 OldVersion)
{
    switch (OldVersion)
    {
        case 0:
            // Migrate from version 0 to 1
            // Add timeline mastery system
            if (!AchievementProgress.Contains(TEXT("TimelineMastery")))
            {
                AchievementProgress.Add(TEXT("TimelineMastery"), 0.0f);
            }
            break;

        // Add more version migrations here
    }
}

bool USESaveGame::ValidateData()
{
    bool bIsValid = true;

    bIsValid &= ValidatePlayerData();
    bIsValid &= ValidateQuestData();
    bIsValid &= ValidateAbilityData();
    bIsValid &= ValidateInventoryData();

    return bIsValid;
}

void USESaveGame::RepairInvalidData()
{
    // Fix player data
    PlayerLevel = FMath::Max(1, PlayerLevel);
    PlayerExperience = FMath::Max(0, PlayerExperience);
    PlayerCurrency = FMath::Max(0, PlayerCurrency);

    // Fix combat stats
    MaxHealth = FMath::Max(100.0f, MaxHealth);
    MaxTimelineEnergy = FMath::Max(100.0f, MaxTimelineEnergy);
    BaseDamage = FMath::Max(10.0f, BaseDamage);

    // Fix timeline mastery
    BrightTimelineMastery = FMath::Clamp(BrightTimelineMastery, 0.0f, 1.0f);
    DarkTimelineMastery = FMath::Clamp(DarkTimelineMastery, 0.0f, 1.0f);

    // Fix achievement progress
    for (auto& Pair : AchievementProgress)
    {
        Pair.Value = FMath::Max(0.0f, Pair.Value);
    }

    // Fix game settings
    for (auto& Pair : GameSettings)
    {
        Pair.Value = FMath::Clamp(Pair.Value, 0.0f, 1.0f);
    }
}

bool USESaveGame::ValidatePlayerData()
{
    if (PlayerLevel < 1 || PlayerExperience < 0 || PlayerCurrency < 0)
    {
        return false;
    }

    if (MaxHealth <= 0.0f || MaxTimelineEnergy <= 0.0f || BaseDamage <= 0.0f)
    {
        return false;
    }

    if (BrightTimelineMastery < 0.0f || BrightTimelineMastery > 1.0f ||
        DarkTimelineMastery < 0.0f || DarkTimelineMastery > 1.0f)
    {
        return false;
    }

    return true;
}

bool USESaveGame::ValidateQuestData()
{
    for (const auto& Pair : QuestStates)
    {
        if (Pair.Key.IsNone())
        {
            return false;
        }
    }
    return true;
}

bool USESaveGame::ValidateAbilityData()
{
    for (const FName& AbilityID : UnlockedAbilities)
    {
        if (AbilityID.IsNone())
        {
            return false;
        }
    }
    return true;
}

bool USESaveGame::ValidateInventoryData()
{
    for (const FName& ItemID : InventoryItems)
    {
        if (ItemID.IsNone())
        {
            return false;
        }
    }

    for (const auto& Pair : EquippedItems)
    {
        if (Pair.Key.IsNone() || Pair.Value.IsNone())
        {
            return false;
        }
    }

    return true;
}
