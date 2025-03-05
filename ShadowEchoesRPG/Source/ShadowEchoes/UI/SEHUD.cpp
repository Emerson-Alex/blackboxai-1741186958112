// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SEHUD.h"
#include "UI/Widgets/TimelineIndicatorWidget.h"
#include "UI/Widgets/CombatWidget.h"
#include "UI/Widgets/QuestLogWidget.h"
#include "Systems/QuestManager.h"
#include "Blueprint/UserWidget.h"

const FName ASEHUD::LogCategory = TEXT("ShadowEchoes.HUD");

ASEHUD::ASEHUD()
{
}

void ASEHUD::BeginPlay()
{
    Super::BeginPlay();
    CreateWidgets();
}

void ASEHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindQuestEvents();
    Super::EndPlay(EndPlayReason);
}

void ASEHUD::CreateWidgets()
{
    if (APlayerController* PC = GetOwningPlayerController())
    {
        // Create timeline indicator
        if (TimelineIndicatorClass)
        {
            TimelineIndicator = CreateWidget<UTimelineIndicatorWidget>(PC, TimelineIndicatorClass);
            if (TimelineIndicator)
            {
                TimelineIndicator->AddToViewport();
            }
        }

        // Create combat widget
        if (CombatWidgetClass)
        {
            CombatWidget = CreateWidget<UCombatWidget>(PC, CombatWidgetClass);
            if (CombatWidget)
            {
                CombatWidget->AddToViewport();
            }
        }

        // Create quest log widget
        if (QuestLogWidgetClass)
        {
            QuestLogWidget = CreateWidget<UQuestLogWidget>(PC, QuestLogWidgetClass);
            if (QuestLogWidget)
            {
                // Add to viewport but keep hidden initially
                QuestLogWidget->AddToViewport();
                QuestLogWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}

void ASEHUD::SetQuestManager(UQuestManager* InQuestManager)
{
    // Unbind from previous quest manager if any
    UnbindQuestEvents();

    QuestManager = InQuestManager;

    // Bind to new quest manager
    if (QuestManager)
    {
        BindQuestEvents();

        // Update quest log with current quests
        if (QuestLogWidget)
        {
            UpdateQuestLog(QuestManager->GetActiveQuests());
        }
    }
}

void ASEHUD::ShowQuestNotification(const FText& QuestTitle, const FText& Message)
{
    if (CombatWidget)
    {
        // Use combat widget's notification system for quest notifications
        CombatWidget->ShowNotification(QuestTitle, Message);
    }
}

void ASEHUD::UpdateQuestLog(const TArray<FQuestData>& Quests)
{
    if (QuestLogWidget)
    {
        QuestLogWidget->UpdateQuestList(Quests);
    }
}

void ASEHUD::ToggleQuestLog()
{
    if (!QuestLogWidget)
    {
        return;
    }

    const bool bIsVisible = QuestLogWidget->IsVisible();
    QuestLogWidget->SetVisibility(bIsVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

    // Update input mode based on visibility
    if (APlayerController* PC = GetOwningPlayerController())
    {
        if (!bIsVisible)
        {
            // Show cursor and allow UI input
            PC->SetShowMouseCursor(true);
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
        }
        else
        {
            // Hide cursor and return to game input
            PC->SetShowMouseCursor(false);
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void ASEHUD::OnQuestAdded(const FQuestData& QuestData)
{
    // Show notification
    ShowQuestNotification(
        QuestData.Title,
        FText::FromString(TEXT("New Quest Added"))
    );

    // Update quest log
    if (QuestManager)
    {
        UpdateQuestLog(QuestManager->GetActiveQuests());
    }
}

void ASEHUD::OnQuestCompleted(const FQuestData& QuestData)
{
    // Show notification
    ShowQuestNotification(
        QuestData.Title,
        FText::FromString(TEXT("Quest Completed"))
    );

    // Update quest log
    if (QuestManager)
    {
        UpdateQuestLog(QuestManager->GetActiveQuests());
    }

    // Play completion animation if quest log is visible
    if (QuestLogWidget && QuestLogWidget->IsVisible())
    {
        // TODO: Play completion animation
    }
}

void ASEHUD::OnQuestFailed(const FQuestData& QuestData)
{
    // Show notification
    ShowQuestNotification(
        QuestData.Title,
        FText::FromString(TEXT("Quest Failed"))
    );

    // Update quest log
    if (QuestManager)
    {
        UpdateQuestLog(QuestManager->GetActiveQuests());
    }
}

void ASEHUD::OnQuestProgressUpdated(const FName& QuestID, const FQuestObjective& UpdatedObjective)
{
    // Only show notification for significant progress (e.g., objective completion)
    if (UpdatedObjective.bIsComplete)
    {
        FQuestData QuestData;
        if (QuestManager && QuestManager->GetQuestData(QuestID, QuestData))
        {
            ShowQuestNotification(
                QuestData.Title,
                FText::Format(
                    FText::FromString(TEXT("Objective Complete: {0}")),
                    UpdatedObjective.Description
                )
            );
        }
    }

    // Update quest log
    if (QuestManager)
    {
        UpdateQuestLog(QuestManager->GetActiveQuests());
    }
}

void ASEHUD::BindQuestEvents()
{
    if (!QuestManager)
    {
        return;
    }

    QuestManager->OnQuestAdded.AddDynamic(this, &ASEHUD::OnQuestAdded);
    QuestManager->OnQuestCompleted.AddDynamic(this, &ASEHUD::OnQuestCompleted);
    QuestManager->OnQuestFailed.AddDynamic(this, &ASEHUD::OnQuestFailed);
    QuestManager->OnQuestProgressUpdated.AddDynamic(this, &ASEHUD::OnQuestProgressUpdated);
}

void ASEHUD::UnbindQuestEvents()
{
    if (!QuestManager)
    {
        return;
    }

    QuestManager->OnQuestAdded.RemoveDynamic(this, &ASEHUD::OnQuestAdded);
    QuestManager->OnQuestCompleted.RemoveDynamic(this, &ASEHUD::OnQuestCompleted);
    QuestManager->OnQuestFailed.RemoveDynamic(this, &ASEHUD::OnQuestFailed);
    QuestManager->OnQuestProgressUpdated.RemoveDynamic(this, &ASEHUD::OnQuestProgressUpdated);
}
