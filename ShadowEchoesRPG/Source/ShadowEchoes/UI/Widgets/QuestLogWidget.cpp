// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/QuestLogWidget.h"
#include "UI/Widgets/QuestEntryWidget.h"
#include "UI/Widgets/QuestObjectiveWidget.h"
#include "Core/SEGameInstance.h"
#include "Systems/QuestManager.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UQuestLogWidget::UQuestLogWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentTimelineFilter(ETimelineState::Any)
{
}

void UQuestLogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidgets();
    SetupCallbacks();
    RefreshQuestList();
}

void UQuestLogWidget::NativeDestruct()
{
    // Clear callbacks
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            QuestManager->OnQuestObjectiveCompleted.RemoveDynamic(this, &UQuestLogWidget::OnObjectiveCompleted);
        }
    }

    Super::NativeDestruct();
}

void UQuestLogWidget::DisplayQuest(const FQuestInfo& Quest)
{
    // Update selected quest
    SelectedQuestID = Quest.QuestID;

    // Update quest details
    if (QuestTitleText)
    {
        QuestTitleText->SetText(Quest.Title);
    }

    if (QuestDescriptionText)
    {
        QuestDescriptionText->SetText(Quest.Description);
    }

    // Display objectives
    DisplayObjectives(Quest);

    // Update button states
    if (TrackButton)
    {
        TrackButton->SetIsEnabled(true);
        bool bIsTracked = TrackedQuests.Contains(Quest.QuestID);
        TrackButton->SetVisibility(bIsTracked ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }

    if (AbandonButton)
    {
        AbandonButton->SetIsEnabled(true);
    }

    // Notify blueprint
    BP_OnQuestSelected(Quest);
}

void UQuestLogWidget::RefreshQuestList()
{
    ClearQuestList();

    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            // Get quests based on filter
            TArray<FQuestInfo> Quests;
            if (CurrentTimelineFilter == ETimelineState::Any)
            {
                Quests = QuestManager->GetActiveQuests();
            }
            else
            {
                Quests = QuestManager->GetTimelineQuests(CurrentTimelineFilter);
            }

            // Add quest entries
            for (const FQuestInfo& Quest : Quests)
            {
                AddQuestEntry(Quest);
            }
        }
    }
}

void UQuestLogWidget::FilterQuests(ETimelineState Timeline)
{
    if (CurrentTimelineFilter != Timeline)
    {
        CurrentTimelineFilter = Timeline;
        RefreshQuestList();
        BP_OnTimelineFilterChanged(Timeline);
    }
}

void UQuestLogWidget::TrackQuest(const FName& QuestID)
{
    if (!TrackedQuests.Contains(QuestID))
    {
        TrackedQuests.Add(QuestID);

        // Update UI
        if (UQuestEntryWidget* Entry = QuestEntryWidgets.FindRef(QuestID))
        {
            Entry->SetTracked(true);
        }

        // Notify blueprint
        if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
        {
            if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
            {
                if (const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID))
                {
                    BP_OnQuestTracked(*Quest);
                }
            }
        }
    }
}

void UQuestLogWidget::UntrackQuest(const FName& QuestID)
{
    if (TrackedQuests.Remove(QuestID) > 0)
    {
        // Update UI
        if (UQuestEntryWidget* Entry = QuestEntryWidgets.FindRef(QuestID))
        {
            Entry->SetTracked(false);
        }

        // Notify blueprint
        if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
        {
            if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
            {
                if (const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID))
                {
                    BP_OnQuestUntracked(*Quest);
                }
            }
        }
    }
}

void UQuestLogWidget::SortQuestsByLevel()
{
    if (QuestListBox)
    {
        TArray<UWidget*> Children = QuestListBox->GetAllChildren();
        Children.Sort([](const UWidget& A, const UWidget& B) {
            const UQuestEntryWidget* EntryA = Cast<UQuestEntryWidget>(&A);
            const UQuestEntryWidget* EntryB = Cast<UQuestEntryWidget>(&B);
            if (EntryA && EntryB)
            {
                return EntryA->GetQuestLevel() < EntryB->GetQuestLevel();
            }
            return false;
        });

        QuestListBox->ClearChildren();
        for (UWidget* Child : Children)
        {
            QuestListBox->AddChild(Child);
        }
    }
}

void UQuestLogWidget::SortQuestsByProgress()
{
    if (QuestListBox)
    {
        TArray<UWidget*> Children = QuestListBox->GetAllChildren();
        Children.Sort([](const UWidget& A, const UWidget& B) {
            const UQuestEntryWidget* EntryA = Cast<UQuestEntryWidget>(&A);
            const UQuestEntryWidget* EntryB = Cast<UQuestEntryWidget>(&B);
            if (EntryA && EntryB)
            {
                return EntryA->GetQuestProgress() > EntryB->GetQuestProgress();
            }
            return false;
        });

        QuestListBox->ClearChildren();
        for (UWidget* Child : Children)
        {
            QuestListBox->AddChild(Child);
        }
    }
}

void UQuestLogWidget::InitializeWidgets()
{
    // Setup button callbacks
    if (TrackButton)
    {
        TrackButton->OnClicked.AddDynamic(this, &UQuestLogWidget::OnTrackButtonClicked);
        TrackButton->SetIsEnabled(false);
    }

    if (AbandonButton)
    {
        AbandonButton->OnClicked.AddDynamic(this, &UQuestLogWidget::OnAbandonButtonClicked);
        AbandonButton->SetIsEnabled(false);
    }
}

void UQuestLogWidget::SetupCallbacks()
{
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            QuestManager->OnQuestObjectiveCompleted.AddDynamic(this, &UQuestLogWidget::OnObjectiveCompleted);
        }
    }
}

void UQuestLogWidget::ClearQuestList()
{
    if (QuestListBox)
    {
        QuestListBox->ClearChildren();
    }
    QuestEntryWidgets.Empty();
}

void UQuestLogWidget::AddQuestEntry(const FQuestInfo& Quest)
{
    if (!QuestListBox || !QuestEntryWidgetClass)
    {
        return;
    }

    // Create quest entry widget
    UQuestEntryWidget* EntryWidget = CreateWidget<UQuestEntryWidget>(this, QuestEntryWidgetClass);
    if (EntryWidget)
    {
        EntryWidget->SetQuestInfo(Quest);
        EntryWidget->SetTracked(TrackedQuests.Contains(Quest.QuestID));
        EntryWidget->OnQuestSelected.AddDynamic(this, &UQuestLogWidget::OnQuestSelected);

        QuestListBox->AddChild(EntryWidget);
        QuestEntryWidgets.Add(Quest.QuestID, EntryWidget);
    }
}

void UQuestLogWidget::UpdateQuestEntry(const FName& QuestID)
{
    if (UQuestEntryWidget* Entry = QuestEntryWidgets.FindRef(QuestID))
    {
        if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
        {
            if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
            {
                if (const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID))
                {
                    Entry->UpdateProgress(QuestManager->GetObjectiveProgress(QuestID, NAME_None));
                }
            }
        }
    }
}

void UQuestLogWidget::DisplayObjectives(const FQuestInfo& Quest)
{
    ClearObjectives();

    if (!ObjectivesBox || !QuestObjectiveWidgetClass)
    {
        return;
    }

    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            // Create objective widgets
            for (const FName& ObjectiveID : Quest.ObjectiveIDs)
            {
                UQuestObjectiveWidget* ObjectiveWidget = CreateWidget<UQuestObjectiveWidget>(this, QuestObjectiveWidgetClass);
                if (ObjectiveWidget)
                {
                    ObjectiveWidget->SetObjectiveInfo(ObjectiveID);
                    ObjectiveWidget->UpdateProgress(QuestManager->GetObjectiveProgress(Quest.QuestID, ObjectiveID));
                    ObjectivesBox->AddChild(ObjectiveWidget);
                    ObjectiveWidgets.Add(ObjectiveID, ObjectiveWidget);
                }
            }
        }
    }
}

void UQuestLogWidget::ClearObjectives()
{
    if (ObjectivesBox)
    {
        ObjectivesBox->ClearChildren();
    }
    ObjectiveWidgets.Empty();
}

void UQuestLogWidget::UpdateObjectiveProgress(const FName& QuestID, const FName& ObjectiveID)
{
    if (QuestID == SelectedQuestID)
    {
        if (UQuestObjectiveWidget* ObjectiveWidget = ObjectiveWidgets.FindRef(ObjectiveID))
        {
            if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
            {
                if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
                {
                    ObjectiveWidget->UpdateProgress(QuestManager->GetObjectiveProgress(QuestID, ObjectiveID));
                }
            }
        }
    }
}

void UQuestLogWidget::OnQuestSelected(const FName& QuestID)
{
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            if (const FQuestInfo* Quest = QuestManager->GetQuestInfo(QuestID))
            {
                DisplayQuest(*Quest);
            }
        }
    }
}

void UQuestLogWidget::OnTrackButtonClicked()
{
    if (SelectedQuestID.IsNone())
    {
        return;
    }

    if (TrackedQuests.Contains(SelectedQuestID))
    {
        UntrackQuest(SelectedQuestID);
    }
    else
    {
        TrackQuest(SelectedQuestID);
    }
}

void UQuestLogWidget::OnAbandonButtonClicked()
{
    if (SelectedQuestID.IsNone())
    {
        return;
    }

    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            QuestManager->AbandonQuest(SelectedQuestID);
            RefreshQuestList();
        }
    }
}

void UQuestLogWidget::OnQuestStarted(const FQuestInfo& Quest)
{
    RefreshQuestList();
}

void UQuestLogWidget::OnQuestCompleted(const FQuestInfo& Quest)
{
    if (Quest.QuestID == SelectedQuestID)
    {
        SelectedQuestID = NAME_None;
        ClearObjectives();
    }

    UntrackQuest(Quest.QuestID);
    RefreshQuestList();
}

void UQuestLogWidget::OnQuestFailed(const FQuestInfo& Quest)
{
    if (Quest.QuestID == SelectedQuestID)
    {
        SelectedQuestID = NAME_None;
        ClearObjectives();
    }

    UntrackQuest(Quest.QuestID);
    RefreshQuestList();
}

void UQuestLogWidget::OnObjectiveCompleted(const FName& QuestID, const FName& ObjectiveID)
{
    UpdateQuestEntry(QuestID);
    UpdateObjectiveProgress(QuestID, ObjectiveID);
}

void UQuestLogWidget::OnObjectiveProgressUpdated(const FName& QuestID, const FName& ObjectiveID, float Progress)
{
    UpdateQuestEntry(QuestID);
    UpdateObjectiveProgress(QuestID, ObjectiveID);
}
