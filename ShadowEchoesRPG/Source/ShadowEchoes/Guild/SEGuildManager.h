// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SEGuildManager.generated.h"

class USEGameInstance;
class UTimelineManager;

UENUM(BlueprintType)
enum class EGuildRank : uint8
{
    Member          UMETA(DisplayName = "Member"),
    Officer         UMETA(DisplayName = "Officer"),
    TimelineGuide   UMETA(DisplayName = "Timeline Guide"),
    GuildMaster     UMETA(DisplayName = "Guild Master")
};

USTRUCT(BlueprintType)
struct FGuildData
{
    GENERATED_BODY()

    UPROPERTY()
    FString GuildID;

    UPROPERTY()
    FString GuildName;

    UPROPERTY()
    FString Description;

    UPROPERTY()
    ETimelineState PreferredTimeline;

    UPROPERTY()
    int32 Level;

    UPROPERTY()
    int32 MaxMembers;

    UPROPERTY()
    TArray<FString> Members;

    UPROPERTY()
    TMap<FString, EGuildRank> MemberRanks;

    UPROPERTY()
    TArray<FName> UnlockedFeatures;

    UPROPERTY()
    int32 TimelinePower;

    UPROPERTY()
    FString GuildHallID;
};

USTRUCT(BlueprintType)
struct FGuildMission
{
    GENERATED_BODY()

    UPROPERTY()
    FString MissionID;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Description;

    UPROPERTY()
    ETimelineState RequiredTimeline;

    UPROPERTY()
    int32 RequiredMembers;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    TArray<FName> Objectives;

    UPROPERTY()
    TArray<FName> Rewards;
};

USTRUCT(BlueprintType)
struct FGuildHall
{
    GENERATED_BODY()

    UPROPERTY()
    FString HallID;

    UPROPERTY()
    FString Name;

    UPROPERTY()
    ETimelineState Timeline;

    UPROPERTY()
    TArray<FName> Features;

    UPROPERTY()
    TArray<FName> Upgrades;

    UPROPERTY()
    TMap<FName, float> Bonuses;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGuildCreated, const FGuildData&, GuildData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuildMissionCompleted, const FString&, GuildID, const FGuildMission&, Mission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGuildHallUpgraded, const FString&, GuildID, const FName&, UpgradeID);

/**
 * Manages guild systems including timeline-specific halls and missions
 */
UCLASS()
class SHADOWECHOES_API USEGuildManager : public UObject
{
    GENERATED_BODY()

public:
    USEGuildManager();

    /** Initialize the guild system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Guild management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    bool CreateGuild(const FString& GuildName, const FString& FounderID, ETimelineState PreferredTimeline);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    bool JoinGuild(const FString& PlayerID, const FString& GuildID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void LeaveGuild(const FString& PlayerID, const FString& GuildID);

    /** Guild missions */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    bool StartGuildMission(const FString& GuildID, const FString& MissionID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void CompleteGuildMission(const FString& GuildID, const FString& MissionID);

    /** Guild hall */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    bool UpgradeGuildHall(const FString& GuildID, const FName& UpgradeID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void UnlockGuildFeature(const FString& GuildID, const FName& FeatureID);

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Guild progression */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void AddGuildExperience(const FString& GuildID, int32 Experience);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Guild")
    void UpdateGuildTimelinePower(const FString& GuildID, int32 PowerChange);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Guild|Events")
    FOnGuildCreated OnGuildCreated;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Guild|Events")
    FOnGuildMissionCompleted OnGuildMissionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Guild|Events")
    FOnGuildHallUpgraded OnGuildHallUpgraded;

protected:
    /** Guild settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Guild")
    int32 MaxGuildLevel;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Guild")
    int32 BaseMaxMembers;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Guild")
    float MissionCheckInterval;

private:
    /** Current state */
    UPROPERTY()
    TMap<FString, FGuildData> Guilds;

    UPROPERTY()
    TMap<FString, FGuildMission> ActiveMissions;

    UPROPERTY()
    TMap<FString, FGuildHall> GuildHalls;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    bool ValidateGuildCreation(const FString& GuildName, const FString& FounderID) const;
    bool ValidateGuildMission(const FString& GuildID, const FString& MissionID) const;
    void ProcessGuildMissions();
    void UpdateGuildBonuses(const FString& GuildID);
    void CheckTimelineEffects(ETimelineState NewState);
    bool CanAccessGuildHall(const FString& GuildID, const FString& PlayerID) const;
    void ApplyGuildHallEffects(const FGuildHall& Hall);
    int32 CalculateGuildPowerBonus(const FGuildData& Guild) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Guild|Events")
    void BP_OnGuildCreated(const FGuildData& GuildData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Guild|Events")
    void BP_OnGuildMissionCompleted(const FString& GuildID, const FGuildMission& Mission);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Guild|Events")
    void BP_OnGuildHallUpgraded(const FString& GuildID, const FName& UpgradeID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Guild|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
