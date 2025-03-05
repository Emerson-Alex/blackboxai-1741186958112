// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/QuestObjectiveWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Core/SEGameInstance.h"
#include "Systems/QuestManager.h"

UQuestObjectiveWidget::UQuestObjectiveWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , Progress(0.0f)
    , bIsComplete(false)
    , bIsOptional(false)
    , RequiredTimeline(ETimelineState::Any)
    , NormalColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f))
    , CompleteColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
    , OptionalColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
{
}

void UQuestObjectiveWidget::NativeConstruct()
{
    Super::NativeConstruct();
    InitializeWidgets();
}

void UQuestObjectiveWidget::SetObjectiveInfo(const FName& InObjectiveID)
{
    ObjectiveID = InObjectiveID;

    // Get objective info from quest manager
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        if (UQuestManager* QuestManager = GameInstance->GetQuestManager())
        {
            const FQuestObjectiveInfo* Objective = QuestManager->GetObjectiveInfo(ObjectiveID);
            if (Objective)
            {
                // Store objective data
                bIsOptional = Objective->bIsOptional;
                RequiredTimeline = Objective->RequiredTimeline;

                // Update text
                if (ObjectiveText)
                {
                    ObjectiveText->SetText(Objective->Description);
                }

                // Update visuals
                UpdateVisuals();

                // Notify blueprint
                BP_OnObjectiveSet(*Objective);
            }
        }
    }
}

void UQuestObjectiveWidget::UpdateProgress(float NewProgress)
{
    Progress = FMath::Clamp(NewProgress, 0.0f, 1.0f);

    // Check for completion
    if (Progress >= 1.0f && !bIsComplete)
    {
        SetComplete(true);
    }

    // Update progress display
    UpdateProgressDisplay();

    // Notify blueprint
    BP_OnProgressUpdated(Progress);
}

void UQuestObjectiveWidget::SetComplete(bool bComplete)
{
    if (bIsComplete != bComplete)
    {
        bIsComplete = bComplete;

        // Update complete icon
        if (CompleteIcon)
        {
            CompleteIcon->SetVisibility(bIsComplete ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }

        // Update colors
        UpdateColors();

        // Notify blueprint
        BP_OnCompleteStateChanged(bIsComplete);
    }
}

void UQuestObjectiveWidget::SetOptional(bool bOptional)
{
    if (bIsOptional != bOptional)
    {
        bIsOptional = bOptional;

        // Update optional icon
        if (OptionalIcon)
        {
            OptionalIcon->SetVisibility(bIsOptional ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }

        // Update colors
        UpdateColors();

        // Notify blueprint
        BP_OnOptionalStateChanged(bIsOptional);
    }
}

void UQuestObjectiveWidget::OnTimelineStateChanged(ETimelineState NewState)
{
    // Update timeline icon
    UpdateTimelineIcon();

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

void UQuestObjectiveWidget::InitializeWidgets()
{
    // Initialize icons
    if (CompleteIcon)
    {
        CompleteIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    if (OptionalIcon)
    {
        OptionalIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    if (TimelineIcon)
    {
        TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    // Initialize progress bar
    if (ObjectiveProgressBar)
    {
        ObjectiveProgressBar->SetPercent(0.0f);
    }
}

void UQuestObjectiveWidget::UpdateVisuals()
{
    UpdateTimelineIcon();
    UpdateColors();
    UpdateProgressDisplay();
}

void UQuestObjectiveWidget::UpdateTimelineIcon()
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

void UQuestObjectiveWidget::UpdateColors()
{
    FLinearColor TextColor = NormalColor;

    // Apply color based on state
    if (bIsComplete)
    {
        TextColor = CompleteColor;
    }
    else if (bIsOptional)
    {
        TextColor = OptionalColor;
    }

    // Update text color
    if (ObjectiveText)
    {
        ObjectiveText->SetColorAndOpacity(TextColor);
    }

    // Update progress bar color
    if (ObjectiveProgressBar)
    {
        ObjectiveProgressBar->SetFillColorAndOpacity(TextColor);
    }
}

void UQuestObjectiveWidget::UpdateProgressDisplay()
{
    // Update progress bar
    if (ObjectiveProgressBar)
    {
        ObjectiveProgressBar->SetPercent(Progress);

        // Hide progress bar if complete
        ObjectiveProgressBar->SetVisibility(bIsComplete ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
    }

    // Update text style for completion
    if (ObjectiveText && bIsComplete)
    {
        // Add strikethrough or completion marker
        FText CurrentText = ObjectiveText->GetText();
        ObjectiveText->SetText(FText::FromString(FString::Printf(TEXT("✓ %s"), *CurrentText.ToString())));
    }
}
