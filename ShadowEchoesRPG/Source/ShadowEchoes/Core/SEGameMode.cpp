// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SEGameMode.h"
#include "Systems/TimelineManager.h"
#include "Systems/QuestManager.h"
#include "UI/SEHUD.h"
#include "Kismet/GameplayStatics.h"

const FName ASEGameMode::LogCategory = TEXT("ShadowEchoes.GameMode");

ASEGameMode::ASEGameMode()
{
    // Set default HUD class
    HUDClass = ASEHUD::StaticClass();
}

void ASEGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    InitializeGameSystems();
}

void ASEGameMode::StartPlay()
{
    Super::StartPlay();

    // Connect systems to HUD
    ConnectSystemsToHUD();

    // Load initial quests
    LoadInitialQuests();
}

void ASEGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clean up systems
    if (TimelineManager)
    {
        TimelineManager->ConditionalBeginDestroy();
        TimelineManager = nullptr;
    }

    if (QuestManager)
    {
        QuestManager->ConditionalBeginDestroy();
        QuestManager = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void ASEGameMode::InitializeGameSystems()
{
    // Create timeline manager
    TimelineManager = NewObject<UTimelineManager>(this, TEXT("TimelineManager"));
    if (!ensure(TimelineManager))
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to create TimelineManager"), *LogCategory.ToString());
        return;
    }

    // Initialize quest system
    InitializeQuestSystem();
}

void ASEGameMode::InitializeQuestSystem()
{
    // Create quest manager
    QuestManager = NewObject<UQuestManager>(this, TEXT("QuestManager"));
    if (!ensure(QuestManager))
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to create QuestManager"), *LogCategory.ToString());
        return;
    }

    // Initialize quest manager with timeline manager
    QuestManager->Initialize(TimelineManager);
}

void ASEGameMode::LoadInitialQuests()
{
    if (!QuestManager)
    {
        return;
    }

    // Load quest data tables
    UDataTable* QuestTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_Quests")));
    UDataTable* ObjectiveTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_QuestObjectives")));

    if (!QuestTable || !ObjectiveTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Failed to load quest data tables"), *LogCategory.ToString());
        return;
    }

    // Load quests from data tables
    if (!QuestManager->LoadQuestsFromDataTable(QuestTable, ObjectiveTable))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Failed to load quests from data tables"), *LogCategory.ToString());
        return;
    }

    // Add available starter quests
    TArray<FQuestData> StarterQuests = QuestManager->GetAvailableQuestsForLevel(1);
    for (const FQuestData& Quest : StarterQuests)
    {
        QuestManager->AddQuest(Quest);
    }
}

void ASEGameMode::CreateDefaultQuests()
{
    // This method is now deprecated as quests are loaded from data tables
    UE_LOG(LogTemp, Warning, TEXT("%s: CreateDefaultQuests is deprecated. Quests should be defined in data tables."), 
        *LogCategory.ToString());
}

void ASEGameMode::ConnectSystemsToHUD()
{
    // Get HUD reference
    ASEHUD* HUD = Cast<ASEHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    if (!HUD)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to get HUD reference"), *LogCategory.ToString());
        return;
    }

    // Connect quest manager to HUD
    if (QuestManager)
    {
        HUD->SetQuestManager(QuestManager);
    }

    // Connect timeline manager to HUD
    if (TimelineManager)
    {
        // TODO: Implement timeline manager connection to HUD if needed
    }
}
