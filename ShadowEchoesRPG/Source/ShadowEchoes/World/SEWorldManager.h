// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "SEWorldManager.generated.h"

class USEGameInstance;
class UTimelineManager;

USTRUCT(BlueprintType)
struct FWorldArea
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FName AreaID;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FText Name;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FText Description;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    int32 RecommendedLevel;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    ETimelineState PreferredTimeline;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> ConnectedAreas;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> HiddenSecrets;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    bool bIsSafeZone;
};

USTRUCT(BlueprintType)
struct FWorldEvent
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FName EventID;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FText Name;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FText Description;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    float Duration;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    float CooldownTime;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    ETimelineState RequiredTimeline;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> SpawnableBosses;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> SpecialRewards;
};

USTRUCT(BlueprintType)
struct FWorldBoss
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FName BossID;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    FText Name;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    int32 Level;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    float RespawnTime;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> PossibleSpawnAreas;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    bool bIsRoaming;

    UPROPERTY(EditDefaultsOnly, Category = "World")
    TArray<FName> UniqueDrops;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldEventStarted, const FWorldEvent&, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldBossSpawned, const FWorldBoss&, Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSecretDiscovered, const FName&, AreaID, const FName&, SecretID);

/**
 * Manages the open world system including areas, events, and world bosses
 */
UCLASS()
class SHADOWECHOES_API USEWorldManager : public UObject
{
    GENERATED_BODY()

public:
    USEWorldManager();

    /** Initialize the world system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Area management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    const FWorldArea* GetAreaInfo(const FName& AreaID) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void UnlockArea(const FName& AreaID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    bool IsAreaUnlocked(const FName& AreaID) const;

    /** Event system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void TriggerWorldEvent(const FName& EventID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void EndWorldEvent(const FName& EventID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    bool IsEventActive(const FName& EventID) const;

    /** World boss system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void SpawnWorldBoss(const FName& BossID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void DespawnWorldBoss(const FName& BossID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    bool IsWorldBossActive(const FName& BossID) const;

    /** Secret discovery */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void DiscoverSecret(const FName& AreaID, const FName& SecretID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    bool IsSecretDiscovered(const FName& AreaID, const FName& SecretID) const;

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|World")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|World|Events")
    FOnWorldEventStarted OnWorldEventStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|World|Events")
    FOnWorldBossSpawned OnWorldBossSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|World|Events")
    FOnSecretDiscovered OnSecretDiscovered;

protected:
    /** World settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|World")
    float EventCheckInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|World")
    float BossRespawnMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|World")
    int32 MaxActiveEvents;

private:
    /** Current state */
    UPROPERTY()
    TMap<FName, FWorldArea> Areas;

    UPROPERTY()
    TMap<FName, FWorldEvent> ActiveEvents;

    UPROPERTY()
    TMap<FName, FWorldBoss> ActiveBosses;

    UPROPERTY()
    TSet<FName> UnlockedAreas;

    UPROPERTY()
    TSet<FName> DiscoveredSecrets;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    void LoadWorldData();
    void UpdateWorldState();
    void ProcessEventCooldowns();
    void ManageWorldBosses();
    bool ValidateAreaConnection(const FName& FromArea, const FName& ToArea) const;
    void CheckTimelineSpecificContent(ETimelineState NewState);
    void UpdateAreaDifficulty();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|World|Events")
    void BP_OnWorldEventStarted(const FWorldEvent& Event);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|World|Events")
    void BP_OnWorldBossSpawned(const FWorldBoss& Boss);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|World|Events")
    void BP_OnSecretDiscovered(const FName& AreaID, const FName& SecretID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|World|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
