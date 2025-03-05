// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/QuestEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"

UQuestEntryWidget::UQuestEntryWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , QuestLevel(1)
    , RequiredTimeline(ETimelineState::Any)
    , Progress(0.0f)
    , bIsTracked(false)
    , bIsSelected(false)
    , NormalColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
    , SelectedColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f))
    , TrackedColor(FLinearColor(0.0f, 0.8f, 1.0f, 1.0f))
{
}

void UQuestEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeWidgets();
}

void UQuestEntryWidget::SetQuestInfo(const FQuestInfo& Quest)
{
    // Store quest data
    QuestID = Quest.QuestID;
    QuestLevel = Quest.RequiredLevel;
    RequiredTimeline = Quest.RequiredTimeline;

    // Update UI elements
    if (QuestTitleText)
    {
        QuestTitleText->SetText(Quest.Title);
    }

    if (QuestLevelText)
    {
        QuestLevelText->SetText(FText::AsNumber(QuestLevel));
    }

    // Update timeline icon
    UpdateTimelineIcon();

    // Update visuals
    UpdateVisuals();

    // Notify blueprint
    BP_OnQuestSet(Quest);
}

void UQuestEntryWidget::UpdateProgress(float NewProgress)
{
    Progress = FMath::Clamp(NewProgress, 0.0f, 1.0f);

    // Update progress bar
    if (QuestProgressBar)
    {
        QuestProgressBar->SetPercent(Progress);
    }

    // Notify blueprint
    BP_OnProgressUpdated(Progress);
}

void UQuestEntryWidget::SetTracked(bool bTracked)
{
    if (bIsTracked != bTracked)
    {
        bIsTracked = bTracked;

        // Update tracked icon visibility
        if (TrackedIcon)
        {
            TrackedIcon->SetVisibility(bIsTracked ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }

        // Update colors
        UpdateColors();

        // Notify blueprint
        BP_OnTrackedStateChanged(bIsTracked);
    }
}

void UQuestEntryWidget::SetSelected(bool bSelected)
{
    if (bIsSelected != bSelected)
    {
        bIsSelected = bSelected;

        // Update colors
        UpdateColors();

        // Notify blueprint
        BP_OnSelectedStateChanged(bIsSelected);
    }
}

void UQuestEntryWidget::InitializeWidgets()
{
    // Setup button callback
    if (SelectButton)
    {
        SelectButton->OnClicked.AddDynamic(this, &UQuestEntryWidget::OnSelectButtonClicked);
    }

    // Initialize tracked icon
    if (TrackedIcon)
    {
        TrackedIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    // Initialize progress bar
    if (QuestProgressBar)
    {
        QuestProgressBar->SetPercent(0.0f);
    }
}

void UQuestEntryWidget::UpdateVisuals()
{
    UpdateTimelineIcon();
    UpdateColors();
}

void UQuestEntryWidget::UpdateTimelineIcon()
{
    if (!TimelineIcon)
    {
        return;
    }

    // Set appropriate timeline icon
    switch (RequiredTimeline)
    {
        case ETimelineState::BrightWorld:
            if (BrightTimelineIcon)
            {
                TimelineIcon->SetBrushFromTexture(BrightTimelineIcon);
                TimelineIcon->SetVisibility(ESlateVisibility::Visible);
            }
            break;

        case ETimelineState::DarkWorld:
            if (DarkTimelineIcon)
            {
                TimelineIcon->SetBrushFromTexture(DarkTimelineIcon);
                TimelineIcon->SetVisibility(ESlateVisibility::Visible);
            }
            break;

        default:
            TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
            break;
    }
}

void UQuestEntryWidget::UpdateColors()
{
    FLinearColor TextColor = NormalColor;

    // Apply color based on state
    if (bIsSelected)
    {
        TextColor = SelectedColor;
    }
    else if (bIsTracked)
    {
        TextColor = TrackedColor;
    }

    // Update text colors
    if (QuestTitleText)
    {
        QuestTitleText->SetColorAndOpacity(TextColor);
    }

    if (QuestLevelText)
    {
        QuestLevelText->SetColorAndOpacity(TextColor);
    }
}

void UQuestEntryWidget::OnSelectButtonClicked()
{
    // Broadcast selection event
    OnQuestSelected.Broadcast(QuestID);

    // Update selected state
    SetSelected(true);
}
