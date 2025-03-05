#include "TimelineIndicatorWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Systems/SETimelineStateManager.h"
#include "Kismet/GameplayStatics.h"

const FName UTimelineIndicatorWidget::TimelineBlendParam = TEXT("TimelineBlend");
const FName UTimelineIndicatorWidget::StateColorParam = TEXT("StateColor");
const FName UTimelineIndicatorWidget::TransitionProgressParam = TEXT("TransitionProgress");

UTimelineIndicatorWidget::UTimelineIndicatorWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    TransitionDuration = 1.0f;
    CurrentState = ETimelineState::None;
    CurrentEnergy = 0.0f;
    MaxEnergy = 100.0f;
    bIsTransitioning = false;
    TransitionProgress = 0.0f;

    // Default colors
    LightStateColor = FLinearColor(1.0f, 0.9f, 0.7f); // Warm light color
    DarkStateColor = FLinearColor(0.2f, 0.0f, 0.4f);  // Deep purple
    DisabledStateColor = FLinearColor(0.3f, 0.3f, 0.3f);
}

void UTimelineIndicatorWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeTimelineMaterial();
    SetupButtonBindings();
    RegisterTimelineCallbacks();

    // Initial updates
    UpdateTimelineIcon();
    UpdateStateText();
    UpdateEnergyBar();
    UpdateButtonStates();
}

void UTimelineIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsTransitioning)
    {
        UpdateTransitionEffect(InDeltaTime);
    }
}

void UTimelineIndicatorWidget::UpdateTimelineState(ETimelineState NewState)
{
    if (CurrentState != NewState)
    {
        ETimelineState OldState = CurrentState;
        CurrentState = NewState;

        StartTransitionAnimation(OldState, NewState);
        UpdateTimelineIcon();
        UpdateStateText();
        UpdateButtonStates();
    }
}

void UTimelineIndicatorWidget::UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy)
{
    this->CurrentEnergy = CurrentEnergy;
    this->MaxEnergy = MaxEnergy;
    UpdateEnergyBar();
    UpdateButtonStates();
}

void UTimelineIndicatorWidget::ShowTransitionEffect(ETimelineState FromState, ETimelineState ToState, float Duration)
{
    StartTransitionAnimation(FromState, ToState);
}

void UTimelineIndicatorWidget::InitializeTimelineMaterial()
{
    if (TimelineMaterial && TimelineIcon)
    {
        TimelineMaterialInstance = UMaterialInstanceDynamic::Create(TimelineMaterial, this);
        TimelineIcon->SetBrushFromMaterial(TimelineMaterialInstance);

        // Set initial material parameters
        if (TimelineMaterialInstance)
        {
            TimelineMaterialInstance->SetScalarParameterValue(TimelineBlendParam, 0.0f);
            TimelineMaterialInstance->SetVectorParameterValue(StateColorParam, LightStateColor);
            TimelineMaterialInstance->SetScalarParameterValue(TransitionProgressParam, 0.0f);
        }
    }
}

void UTimelineIndicatorWidget::SetupButtonBindings()
{
    if (LightStateButton)
    {
        LightStateButton->OnClicked.AddDynamic(this, &UTimelineIndicatorWidget::OnLightStateClicked);
    }

    if (DarkStateButton)
    {
        DarkStateButton->OnClicked.AddDynamic(this, &UTimelineIndicatorWidget::OnDarkStateClicked);
    }
}

void UTimelineIndicatorWidget::RegisterTimelineCallbacks()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            TimelineManager = Pawn->FindComponentByClass<USETimelineStateManager>();
            if (TimelineManager)
            {
                TimelineManager->OnTimelineStateChanged.AddDynamic(this, &UTimelineIndicatorWidget::HandleTimelineStateChanged);
                TimelineManager->OnEnergyChanged.AddDynamic(this, &UTimelineIndicatorWidget::HandleEnergyChanged);

                // Initialize with current state
                UpdateTimelineState(TimelineManager->GetCurrentState());
                UpdateTimelineEnergy(TimelineManager->GetTimelineEnergy(), TimelineManager->GetMaxTimelineEnergy());
            }
        }
    }
}

void UTimelineIndicatorWidget::UpdateTimelineIcon()
{
    if (!TimelineMaterialInstance)
    {
        return;
    }

    // Update icon texture based on state
    UTexture2D* StateIcon = (CurrentState == ETimelineState::Light) ? LightStateIcon : DarkStateIcon;
    if (TimelineIcon && StateIcon)
    {
        TimelineIcon->SetBrushFromTexture(StateIcon);
    }

    // Update material parameters
    FLinearColor StateColor = GetStateColor(CurrentState);
    TimelineMaterialInstance->SetVectorParameterValue(StateColorParam, StateColor);
}

void UTimelineIndicatorWidget::UpdateStateText()
{
    if (StateText)
    {
        FString DisplayText = GetStateDisplayText(CurrentState);
        StateText->SetText(FText::FromString(DisplayText));
        StateText->SetColorAndOpacity(GetStateColor(CurrentState));
    }
}

void UTimelineIndicatorWidget::UpdateEnergyBar()
{
    if (EnergyBar)
    {
        float EnergyPercent = (MaxEnergy > 0.0f) ? (CurrentEnergy / MaxEnergy) : 0.0f;
        EnergyBar->SetPercent(EnergyPercent);

        // Update color based on energy level
        FLinearColor EnergyColor = FLinearColor::LerpUsingHSV(
            FLinearColor(1.0f, 0.2f, 0.2f), // Red for low energy
            FLinearColor(0.2f, 1.0f, 0.2f), // Green for high energy
            EnergyPercent
        );
        EnergyBar->SetFillColorAndOpacity(EnergyColor);
    }
}

void UTimelineIndicatorWidget::UpdateButtonStates()
{
    if (LightStateButton)
    {
        bool bCanTransitionToLight = CanTransitionToState(ETimelineState::Light);
        LightStateButton->SetIsEnabled(bCanTransitionToLight);
        LightStateButton->SetBackgroundColor(bCanTransitionToLight ? LightStateColor : DisabledStateColor);
    }

    if (DarkStateButton)
    {
        bool bCanTransitionToDark = CanTransitionToState(ETimelineState::Dark);
        DarkStateButton->SetIsEnabled(bCanTransitionToDark);
        DarkStateButton->SetBackgroundColor(bCanTransitionToDark ? DarkStateColor : DisabledStateColor);
    }
}

void UTimelineIndicatorWidget::UpdateTransitionEffect(float DeltaTime)
{
    if (!bIsTransitioning)
    {
        return;
    }

    TransitionProgress += DeltaTime / TransitionDuration;
    if (TransitionProgress >= 1.0f)
    {
        EndTransitionAnimation();
        return;
    }

    // Apply transition curve if available
    float CurvedProgress = TransitionProgress;
    if (TransitionCurve)
    {
        CurvedProgress = TransitionCurve->GetFloatValue(TransitionProgress);
    }

    // Update material transition
    if (TimelineMaterialInstance)
    {
        TimelineMaterialInstance->SetScalarParameterValue(TransitionProgressParam, CurvedProgress);

        // Interpolate between state colors
        FLinearColor FromColor = GetStateColor(TransitionFromState);
        FLinearColor ToColor = GetStateColor(TransitionToState);
        FLinearColor BlendedColor = FMath::Lerp(FromColor, ToColor, CurvedProgress);
        TimelineMaterialInstance->SetVectorParameterValue(StateColorParam, BlendedColor);
    }
}

void UTimelineIndicatorWidget::OnLightStateClicked()
{
    if (TimelineManager && CanTransitionToState(ETimelineState::Light))
    {
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
    }
}

void UTimelineIndicatorWidget::OnDarkStateClicked()
{
    if (TimelineManager && CanTransitionToState(ETimelineState::Dark))
    {
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
    }
}

void UTimelineIndicatorWidget::HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState)
{
    UpdateTimelineState(NewState);
}

void UTimelineIndicatorWidget::HandleEnergyChanged(float NewEnergy, float MaxEnergy)
{
    UpdateTimelineEnergy(NewEnergy, MaxEnergy);
}

FString UTimelineIndicatorWidget::GetStateDisplayText(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::Light:
            return TEXT("Light Timeline");
        case ETimelineState::Dark:
            return TEXT("Dark Timeline");
        default:
            return TEXT("No Timeline");
    }
}

FLinearColor UTimelineIndicatorWidget::GetStateColor(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::Light:
            return LightStateColor;
        case ETimelineState::Dark:
            return DarkStateColor;
        default:
            return DisabledStateColor;
    }
}

bool UTimelineIndicatorWidget::CanTransitionToState(ETimelineState State) const
{
    if (!TimelineManager || State == CurrentState || State == ETimelineState::None)
    {
        return false;
    }

    float RequiredEnergy = TimelineManager->GetTimelineStats().TransitionEnergyCost;
    return CurrentEnergy >= RequiredEnergy;
}

void UTimelineIndicatorWidget::StartTransitionAnimation(ETimelineState FromState, ETimelineState ToState)
{
    TransitionFromState = FromState;
    TransitionToState = ToState;
    TransitionProgress = 0.0f;
    bIsTransitioning = true;

    if (TimelineMaterialInstance)
    {
        TimelineMaterialInstance->SetScalarParameterValue(TransitionProgressParam, 0.0f);
    }
}

void UTimelineIndicatorWidget::EndTransitionAnimation()
{
    bIsTransitioning = false;
    TransitionProgress = 0.0f;

    if (TimelineMaterialInstance)
    {
        TimelineMaterialInstance->SetScalarParameterValue(TransitionProgressParam, 0.0f);
        TimelineMaterialInstance->SetVectorParameterValue(StateColorParam, GetStateColor(CurrentState));
    }
}
