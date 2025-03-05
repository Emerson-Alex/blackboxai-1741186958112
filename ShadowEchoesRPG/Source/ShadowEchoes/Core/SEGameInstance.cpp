// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Core/SEGameInstance.h"
#include "Systems/TimelineManager.h"
#include "Systems/QuestManager.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SESaveGame.h"

USEGameInstance::USEGameInstance()
    : DefaultTimelineState(ETimelineState::BrightWorld)
    , CurrentTimelineState(ETimelineState::BrightWorld)
    , PlayerLevel(1)
    , PlayerExperience(0)
    , PlayerCurrency(0)
{
}

void USEGameInstance::Init()
{
    Super::Init();

    // Initialize systems
    InitializeManagers();
    InitializePlayerData();
    LoadQuestData();

    // Load saved game if exists
    LoadGame();
}

void USEGameInstance::Shutdown()
{
    // Save game on shutdown
    SaveGame();

    Super::Shutdown();
}

void USEGameInstance::SetTimelineState(ETimelineState NewState)
{
    if (CurrentTimelineState != NewState)
    {
        CurrentTimelineState = NewState;

        // Notify timeline manager
        if (TimelineManager)
        {
            TimelineManager->OnTimelineStateChanged(NewState);
        }

        // Broadcast events
        OnTimelineStateChanged.Broadcast(NewState);
        BP_OnTimelineStateChanged(NewState);
    }
}

void USEGameInstance::RegisterTimelineManager(UTimelineManager* Manager)
{
    TimelineManager = Manager;
    if (TimelineManager)
    {
        TimelineManager->OnTimelineStateChanged(CurrentTimelineState);
    }
}

void USEGameInstance::StartQuest(const FName& QuestID)
{
    if (QuestManager)
    {
        const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID);
        if (Quest)
        {
            // Check requirements
            if (PlayerLevel < Quest->RequiredLevel)
            {
                return;
            }

            // Check prerequisites
            for (const FName& PrereqID : Quest->PrerequisiteQuests)
            {
                if (GetQuestState(PrereqID) != EQuestState::Completed)
                {
                    return;
                }
            }

            // Start quest
            QuestStates.Add(QuestID, EQuestState::InProgress);
            OnQuestStateChanged.Broadcast(*Quest, EQuestState::InProgress);
            BP_OnQuestStateChanged(*Quest, EQuestState::InProgress);
        }
    }
}

void USEGameInstance::CompleteQuest(const FName& QuestID)
{
    if (QuestManager)
    {
        const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID);
        if (Quest && QuestStates.Contains(QuestID))
        {
            // Award rewards
            AddExperience(Quest->RewardXP);
            AddCurrency(Quest->RewardCurrency);

            // Update state
            QuestStates[QuestID] = EQuestState::Completed;
            OnQuestStateChanged.Broadcast(*Quest, EQuestState::Completed);
            BP_OnQuestStateChanged(*Quest, EQuestState::Completed);
        }
    }
}

void USEGameInstance::FailQuest(const FName& QuestID)
{
    if (QuestManager)
    {
        const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID);
        if (Quest && QuestStates.Contains(QuestID))
        {
            QuestStates[QuestID] = EQuestState::Failed;
            OnQuestStateChanged.Broadcast(*Quest, EQuestState::Failed);
            BP_OnQuestStateChanged(*Quest, EQuestState::Failed);
        }
    }
}

EQuestState USEGameInstance::GetQuestState(const FName& QuestID) const
{
    return QuestStates.Contains(QuestID) ? QuestStates[QuestID] : EQuestState::NotStarted;
}

TArray<FQuestInfo> USEGameInstance::GetActiveQuests() const
{
    TArray<FQuestInfo> ActiveQuests;
    if (QuestManager)
    {
        for (const auto& Pair : QuestStates)
        {
            if (Pair.Value == EQuestState::InProgress)
            {
                const FQuestInfo* Quest = QuestManager->GetQuestInfo(Pair.Key);
                if (Quest)
                {
                    ActiveQuests.Add(*Quest);
                }
            }
        }
    }
    return ActiveQuests;
}

void USEGameInstance::AddExperience(int32 XP)
{
    PlayerExperience += XP;
    CheckLevelUp();
}

void USEGameInstance::AddCurrency(int32 Amount)
{
    PlayerCurrency += Amount;
}

bool USEGameInstance::SaveGame()
{
    USESaveGame* SaveGameInstance = Cast<USESaveGame>(UGameplayStatics::CreateSaveGameObject(USESaveGame::StaticClass()));
    if (SaveGameInstance)
    {
        // Save player data
        SaveGameInstance->PlayerLevel = PlayerLevel;
        SaveGameInstance->PlayerExperience = PlayerExperience;
        SaveGameInstance->PlayerCurrency = PlayerCurrency;
        SaveGameInstance->CurrentTimelineState = CurrentTimelineState;
        SaveGameInstance->QuestStates = QuestStates;

        // Save to slot
        return UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MainSave"), 0);
    }
    return false;
}

bool USEGameInstance::LoadGame()
{
    USESaveGame* SaveGameInstance = Cast<USESaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("MainSave"), 0));
    if (SaveGameInstance)
    {
        // Load player data
        PlayerLevel = SaveGameInstance->PlayerLevel;
        PlayerExperience = SaveGameInstance->PlayerExperience;
        PlayerCurrency = SaveGameInstance->PlayerCurrency;
        SetTimelineState(SaveGameInstance->CurrentTimelineState);
        QuestStates = SaveGameInstance->QuestStates;
        return true;
    }
    return false;
}

void USEGameInstance::InitializeManagers()
{
    // Create timeline manager if needed
    if (!TimelineManager)
    {
        TimelineManager = NewObject<UTimelineManager>(this);
    }

    // Create quest manager if needed
    if (!QuestManager)
    {
        QuestManager = NewObject<UQuestManager>(this);
    }
}

void USEGameInstance::InitializePlayerData()
{
    PlayerLevel = 1;
    PlayerExperience = 0;
    PlayerCurrency = 0;
    CurrentTimelineState = DefaultTimelineState;
}

void USEGameInstance::LoadQuestData()
{
    if (QuestManager)
    {
        QuestManager->LoadQuestData();
    }
}

void USEGameInstance::CheckLevelUp()
{
    while (true)
    {
        int32 RequiredXP = GetExperienceForLevel(PlayerLevel + 1);
        if (PlayerExperience >= RequiredXP)
        {
            PlayerLevel++;
            OnPlayerLevelUp.Broadcast(PlayerLevel);
            BP_OnPlayerLevelUp(PlayerLevel);
        }
        else
        {
            break;
        }
    }
}

int32 USEGameInstance::GetExperienceForLevel(int32 Level) const
{
    if (ExperienceCurve)
    {
        return FMath::FloorToInt(ExperienceCurve->GetFloatValue(Level));
    }
    // Fallback to simple exponential curve if no curve asset
    return FMath::FloorToInt(100.0f * FMath::Pow(1.5f, Level - 1));
}
