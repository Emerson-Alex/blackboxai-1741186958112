// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/SETypes.h"
#include "SESaveGame.generated.h"

/**
 * Save Game class for Shadow Echoes RPG
 * Handles persistent storage of game state
 */
UCLASS()
class SHADOWECHOES_API USESaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    USESaveGame();

    /** Player data */
    UPROPERTY()
    int32 PlayerLevel;

    UPROPERTY()
    int32 PlayerExperience;

    UPROPERTY()
    int32 PlayerCurrency;

    /** Timeline state */
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    /** Quest progress */
    UPROPERTY()
    TMap<FName, EQuestState> QuestStates;

    /** Unlocked abilities */
    UPROPERTY()
    TArray<FName> UnlockedAbilities;

    /** Inventory */
    UPROPERTY()
    TArray<FName> InventoryItems;

    /** Equipment */
    UPROPERTY()
    TMap<FName, FName> EquippedItems;

    /** Combat stats */
    UPROPERTY()
    float MaxHealth;

    UPROPERTY()
    float MaxTimelineEnergy;

    UPROPERTY()
    float BaseDamage;

    /** Timeline mastery */
    UPROPERTY()
    float BrightTimelineMastery;

    UPROPERTY()
    float DarkTimelineMastery;

    /** Achievement progress */
    UPROPERTY()
    TMap<FName, float> AchievementProgress;

    /** Discovered locations */
    UPROPERTY()
    TArray<FName> DiscoveredLocations;

    /** Boss fight records */
    UPROPERTY()
    TMap<FName, float> BossFightRecords;

    /** Game settings */
    UPROPERTY()
    TMap<FName, float> GameSettings;

    /** Save metadata */
    UPROPERTY()
    FDateTime LastSaveTime;

    UPROPERTY()
    FString SaveSlotName;

    UPROPERTY()
    int32 SaveVersion;

    /** Serialization */
    virtual void Serialize(FArchive& Ar) override;

protected:
    /** Initialize default values */
    void InitializeDefaults();

    /** Version handling */
    void UpdateToLatestVersion();
    void MigrateFromVersion(int32 OldVersion);

    /** Data validation */
    bool ValidateData();
    void RepairInvalidData();

private:
    /** Current save version */
    static const int32 CurrentSaveVersion = 1;

    /** Validation helpers */
    bool ValidatePlayerData();
    bool ValidateQuestData();
    bool ValidateAbilityData();
    bool ValidateInventoryData();
};
