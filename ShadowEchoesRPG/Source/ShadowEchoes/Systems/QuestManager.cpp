// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Systems/QuestManager.h"
#include "Core/SEGameInstance.h"
#include "Engine/DataTable.h"

UQuestManager::UQuestManager()
    : CurrentTimelineState(ETimelineState::BrightWorld)
{
}

void UQuestManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    LoadQuestData();
}

void UQuestManager::LoadQuestData()
{
    LoadQuestTables();
    PopulateQuestDatabase();
    PopulateObjectiveDatabase();
    CheckNewQuestsAvailable();
}

bool UQuestManager::StartQuest(const FName& QuestID)
{
    if (!CanStartQuest(QuestID))
    {
        return false;
    }

    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    if (!Quest)
    {
        return false;
    }

    // Initialize objective progress
    TMap<FName, float>& Objectives = ObjectiveProgress.FindOrAdd(QuestID);
    for (const FName& ObjectiveID : Quest->ObjectiveIDs)
    {
        Objectives.Add(ObjectiveID, 0.0f);
    }

    // Notify game instance
    if (GameInstance)
    {
        GameInstance->StartQuest(QuestID);
    }

    // Notify blueprint
    BP_OnQuestStarted(*Quest);

    return true;
}

bool UQuestManager::CompleteQuest(const FName& QuestID)
{
    if (!ValidateQuest(QuestID))
    {
        return false;
    }

    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    if (!Quest)
    {
        return false;
    }

    // Check all objectives are complete
    if (!AreAllObjectivesComplete(QuestID))
    {
        return false;
    }

    // Award rewards
    AwardQuestRewards(*Quest);

    // Notify game instance
    if (GameInstance)
    {
        GameInstance->CompleteQuest(QuestID);
    }

    // Notify blueprint
    BP_OnQuestCompleted(*Quest);

    // Check for new available quests
    CheckNewQuestsAvailable();

    return true;
}

bool UQuestManager::FailQuest(const FName& QuestID)
{
    if (!ValidateQuest(QuestID))
    {
        return false;
    }

    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    if (!Quest)
    {
        return false;
    }

    // Notify game instance
    if (GameInstance)
    {
        GameInstance->FailQuest(QuestID);
    }

    // Notify blueprint
    BP_OnQuestFailed(*Quest);

    return true;
}

bool UQuestManager::AbandonQuest(const FName& QuestID)
{
    if (!ValidateQuest(QuestID))
    {
        return false;
    }

    // Remove objective progress
    ObjectiveProgress.Remove(QuestID);

    // Notify game instance
    if (GameInstance)
    {
        GameInstance->FailQuest(QuestID);
    }

    return true;
}

bool UQuestManager::CompleteObjective(const FName& QuestID, const FName& ObjectiveID)
{
    if (!ValidateObjective(QuestID, ObjectiveID))
    {
        return false;
    }

    // Set progress to complete
    TMap<FName, float>& Objectives = ObjectiveProgress.FindOrAdd(QuestID);
    Objectives.Add(ObjectiveID, 1.0f);

    // Award objective rewards
    const FQuestObjectiveInfo* Objective = ObjectiveDatabase.Find(ObjectiveID);
    if (Objective)
    {
        AwardObjectiveRewards(*Objective);
    }

    // Notify events
    OnQuestObjectiveCompleted.Broadcast(QuestID, ObjectiveID);
    BP_OnObjectiveCompleted(QuestID, ObjectiveID);

    // Check if quest is complete
    CheckQuestCompletion(QuestID);

    return true;
}

bool UQuestManager::UpdateObjectiveProgress(const FName& QuestID, const FName& ObjectiveID, float Progress)
{
    if (!ValidateObjective(QuestID, ObjectiveID))
    {
        return false;
    }

    // Update progress
    TMap<FName, float>& Objectives = ObjectiveProgress.FindOrAdd(QuestID);
    float& CurrentProgress = Objectives.FindOrAdd(ObjectiveID);
    CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

    // Check for completion
    if (CurrentProgress >= 1.0f)
    {
        CompleteObjective(QuestID, ObjectiveID);
    }

    return true;
}

const FQuestInfo* UQuestManager::GetQuestInfo(const FName& QuestID) const
{
    return QuestDatabase.Find(QuestID);
}

TArray<FQuestInfo> UQuestManager::GetAvailableQuests() const
{
    TArray<FQuestInfo> AvailableQuests;

    for (const auto& Pair : QuestDatabase)
    {
        const FQuestInfo& Quest = Pair.Value;
        if (AreQuestPrerequisitesMet(Quest.QuestID))
        {
            AvailableQuests.Add(Quest);
        }
    }

    return AvailableQuests;
}

TArray<FQuestInfo> UQuestManager::GetActiveQuests() const
{
    TArray<FQuestInfo> ActiveQuests;

    if (GameInstance)
    {
        for (const auto& Pair : QuestDatabase)
        {
            const FQuestInfo& Quest = Pair.Value;
            if (GameInstance->GetQuestState(Quest.QuestID) == EQuestState::InProgress)
            {
                ActiveQuests.Add(Quest);
            }
        }
    }

    return ActiveQuests;
}

TArray<FQuestInfo> UQuestManager::GetCompletedQuests() const
{
    TArray<FQuestInfo> CompletedQuests;

    if (GameInstance)
    {
        for (const auto& Pair : QuestDatabase)
        {
            const FQuestInfo& Quest = Pair.Value;
            if (GameInstance->GetQuestState(Quest.QuestID) == EQuestState::Completed)
            {
                CompletedQuests.Add(Quest);
            }
        }
    }

    return CompletedQuests;
}

float UQuestManager::GetObjectiveProgress(const FName& QuestID, const FName& ObjectiveID) const
{
    const TMap<FName, float>* Objectives = ObjectiveProgress.Find(QuestID);
    if (Objectives)
    {
        const float* Progress = Objectives->Find(ObjectiveID);
        if (Progress)
        {
            return *Progress;
        }
    }
    return 0.0f;
}

bool UQuestManager::AreQuestPrerequisitesMet(const FName& QuestID) const
{
    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    if (!Quest || !GameInstance)
    {
        return false;
    }

    // Check level requirement
    if (GameInstance->GetPlayerLevel() < Quest->RequiredLevel)
    {
        return false;
    }

    // Check timeline requirement
    if (Quest->RequiredTimeline != ETimelineState::Any && 
        Quest->RequiredTimeline != CurrentTimelineState)
    {
        return false;
    }

    // Check prerequisites
    for (const FName& PrereqID : Quest->PrerequisiteQuests)
    {
        if (GameInstance->GetQuestState(PrereqID) != EQuestState::Completed)
        {
            return false;
        }
    }

    return true;
}

TArray<FQuestInfo> UQuestManager::GetTimelineQuests(ETimelineState Timeline) const
{
    TArray<FQuestInfo> TimelineQuests;

    for (const auto& Pair : QuestDatabase)
    {
        const FQuestInfo& Quest = Pair.Value;
        if (Quest.RequiredTimeline == Timeline || Quest.RequiredTimeline == ETimelineState::Any)
        {
            TimelineQuests.Add(Quest);
        }
    }

    return TimelineQuests;
}

void UQuestManager::OnTimelineStateChanged(ETimelineState NewState)
{
    CurrentTimelineState = NewState;
    CheckNewQuestsAvailable();
}

void UQuestManager::LoadQuestTables()
{
    // Load data tables
    MainQuestTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_MainQuests")));
    SideQuestTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_SideQuests")));
    ObjectiveTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_QuestObjectives")));
}

void UQuestManager::PopulateQuestDatabase()
{
    // Clear existing data
    QuestDatabase.Empty();

    // Load main quests
    if (MainQuestTable)
    {
        TArray<FQuestInfo*> MainQuests;
        MainQuestTable->GetAllRows<FQuestInfo>("", MainQuests);
        for (const FQuestInfo* Quest : MainQuests)
        {
            QuestDatabase.Add(Quest->QuestID, *Quest);
        }
    }

    // Load side quests
    if (SideQuestTable)
    {
        TArray<FQuestInfo*> SideQuests;
        SideQuestTable->GetAllRows<FQuestInfo>("", SideQuests);
        for (const FQuestInfo* Quest : SideQuests)
        {
            QuestDatabase.Add(Quest->QuestID, *Quest);
        }
    }
}

void UQuestManager::PopulateObjectiveDatabase()
{
    // Clear existing data
    ObjectiveDatabase.Empty();

    // Load objectives
    if (ObjectiveTable)
    {
        TArray<FQuestObjectiveInfo*> Objectives;
        ObjectiveTable->GetAllRows<FQuestObjectiveInfo>("", Objectives);
        for (const FQuestObjectiveInfo* Objective : Objectives)
        {
            ObjectiveDatabase.Add(Objective->ObjectiveID, *Objective);
        }
    }
}

bool UQuestManager::ValidateQuest(const FName& QuestID) const
{
    return QuestDatabase.Contains(QuestID);
}

bool UQuestManager::ValidateObjective(const FName& QuestID, const FName& ObjectiveID) const
{
    if (!ValidateQuest(QuestID))
    {
        return false;
    }

    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    return Quest && Quest->ObjectiveIDs.Contains(ObjectiveID);
}

bool UQuestManager::CanStartQuest(const FName& QuestID) const
{
    if (!ValidateQuest(QuestID))
    {
        return false;
    }

    if (!GameInstance)
    {
        return false;
    }

    // Check if quest is already started or completed
    EQuestState State = GameInstance->GetQuestState(QuestID);
    if (State != EQuestState::NotStarted)
    {
        return false;
    }

    return AreQuestPrerequisitesMet(QuestID);
}

bool UQuestManager::AreAllObjectivesComplete(const FName& QuestID) const
{
    const FQuestInfo* Quest = GetQuestInfo(QuestID);
    if (!Quest)
    {
        return false;
    }

    const TMap<FName, float>* Objectives = ObjectiveProgress.Find(QuestID);
    if (!Objectives)
    {
        return false;
    }

    for (const FName& ObjectiveID : Quest->ObjectiveIDs)
    {
        const float* Progress = Objectives->Find(ObjectiveID);
        if (!Progress || *Progress < 1.0f)
        {
            return false;
        }
    }

    return true;
}

void UQuestManager::CheckQuestCompletion(const FName& QuestID)
{
    if (AreAllObjectivesComplete(QuestID))
    {
        CompleteQuest(QuestID);
    }
}

void UQuestManager::CheckNewQuestsAvailable()
{
    for (const auto& Pair : QuestDatabase)
    {
        const FQuestInfo& Quest = Pair.Value;
        if (AreQuestPrerequisitesMet(Quest.QuestID))
        {
            OnQuestAvailable.Broadcast(Quest);
        }
    }
}

void UQuestManager::AwardQuestRewards(const FQuestInfo& Quest)
{
    if (!GameInstance)
    {
        return;
    }

    // Award XP and currency
    GameInstance->AddExperience(Quest.RewardXP);
    GameInstance->AddCurrency(Quest.RewardCurrency);

    // TODO: Award items when inventory system is implemented
}

void UQuestManager::AwardObjectiveRewards(const FQuestObjectiveInfo& Objective)
{
    if (!GameInstance)
    {
        return;
    }

    // Award XP and currency
    GameInstance->AddExperience(Objective.RewardXP);
    GameInstance->AddCurrency(Objective.RewardCurrency);
}
