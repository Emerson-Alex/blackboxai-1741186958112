// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/SETypes.h"
#include "Engine/DataTable.h"
#include "QuestManager.generated.h"

class USEGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveCompleted, const FName&, QuestID, const FName&, ObjectiveID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAvailable, const FQuestInfo&, Quest);

/**
 * Manages quest system functionality including tracking, progression, and rewards
 */
UCLASS()
class SHADOWECHOES_API UQuestManager : public UObject
{
    GENERATED_BODY()

public:
    UQuestManager();

    /** Initialize the quest system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Quest data loading */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    void LoadQuestData();

    /** Quest management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool StartQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool CompleteQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool FailQuest(const FName& QuestID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool AbandonQuest(const FName& QuestID);

    /** Objective management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool CompleteObjective(const FName& QuestID, const FName& ObjectiveID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    bool UpdateObjectiveProgress(const FName& QuestID, const FName& ObjectiveID, float Progress);

    /** Quest queries */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    const FQuestInfo* GetQuestInfo(const FName& QuestID) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    TArray<FQuestInfo> GetAvailableQuests() const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    TArray<FQuestInfo> GetActiveQuests() const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    TArray<FQuestInfo> GetCompletedQuests() const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    float GetObjectiveProgress(const FName& QuestID, const FName& ObjectiveID) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    bool AreQuestPrerequisitesMet(const FName& QuestID) const;

    /** Timeline-specific quests */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Quests")
    TArray<FQuestInfo> GetTimelineQuests(ETimelineState Timeline) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Quests")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Quests|Events")
    FOnQuestObjectiveCompleted OnQuestObjectiveCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Quests|Events")
    FOnQuestAvailable OnQuestAvailable;

protected:
    /** Quest data tables */
    UPROPERTY()
    UDataTable* MainQuestTable;

    UPROPERTY()
    UDataTable* SideQuestTable;

    UPROPERTY()
    UDataTable* ObjectiveTable;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    /** Current timeline state */
    UPROPERTY()
    ETimelineState CurrentTimelineState;

private:
    /** Quest data */
    TMap<FName, FQuestInfo> QuestDatabase;
    TMap<FName, FQuestObjectiveInfo> ObjectiveDatabase;
    TMap<FName, TMap<FName, float>> ObjectiveProgress;

    /** Load data tables */
    void LoadQuestTables();
    void PopulateQuestDatabase();
    void PopulateObjectiveDatabase();

    /** Quest validation */
    bool ValidateQuest(const FName& QuestID) const;
    bool ValidateObjective(const FName& QuestID, const FName& ObjectiveID) const;
    bool CanStartQuest(const FName& QuestID) const;

    /** Quest completion checks */
    bool AreAllObjectivesComplete(const FName& QuestID) const;
    void CheckQuestCompletion(const FName& QuestID);
    void CheckNewQuestsAvailable();

    /** Reward handling */
    void AwardQuestRewards(const FQuestInfo& Quest);
    void AwardObjectiveRewards(const FQuestObjectiveInfo& Objective);

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Quests|Events")
    void BP_OnQuestStarted(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Quests|Events")
    void BP_OnQuestCompleted(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Quests|Events")
    void BP_OnQuestFailed(const FQuestInfo& Quest);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Quests|Events")
    void BP_OnObjectiveCompleted(const FName& QuestID, const FName& ObjectiveID);
};
