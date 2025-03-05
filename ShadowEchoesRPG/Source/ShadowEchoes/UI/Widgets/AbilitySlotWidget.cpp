// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/AbilitySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

UAbilitySlotWidget::UAbilitySlotWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CooldownDuration(0.0f)
    , CurrentCooldown(0.0f)
    , EnergyCost(0.0f)
    , RequiredTimeline(ETimelineState::Any)
    , bIsOnCooldown(false)
    , AvailableColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
    , UnavailableColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
    , CooldownColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.8f))
{
    // Enable tick for cooldown updates
    PrimaryComponentTick.bCanEverTick = true;
}

void UAbilitySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidgets();
    CreateMaterialInstance();
}

void UAbilitySlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsOnCooldown)
    {
        // Update cooldown
        CurrentCooldown -= InDeltaTime;
        UpdateCooldownDisplay();

        // Check for completion
        if (CurrentCooldown <= 0.0f)
        {
            CompleteCooldown();
        }
    }
}

void UAbilitySlotWidget::SetAbility(const FAbilityInfo& Ability)
{
    // Store ability data
    AbilityID = Ability.AbilityID;
    CooldownDuration = Ability.Cooldown;
    EnergyCost = Ability.EnergyCost;
    RequiredTimeline = Ability.RequiredTimeline;

    // Update visuals
    if (AbilityIcon && Ability.Icon)
    {
        AbilityIcon->SetBrushFromTexture(Ability.Icon);
    }

    if (AbilityNameText)
    {
        AbilityNameText->SetText(Ability.Name);
    }

    // Update timeline icon
    UpdateTimelineIcon();

    // Reset cooldown
    bIsOnCooldown = false;
    CurrentCooldown = 0.0f;
    UpdateCooldownDisplay();

    // Enable button
    if (ActivateButton)
    {
        ActivateButton->SetIsEnabled(true);
    }

    // Notify blueprint
    BP_OnAbilitySet(Ability);
}

void UAbilitySlotWidget::ClearAbility()
{
    // Clear data
    AbilityID = NAME_None;
    CooldownDuration = 0.0f;
    CurrentCooldown = 0.0f;
    EnergyCost = 0.0f;
    RequiredTimeline = ETimelineState::Any;
    bIsOnCooldown = false;

    // Clear visuals
    if (AbilityIcon)
    {
        AbilityIcon->SetBrushFromTexture(nullptr);
    }

    if (AbilityNameText)
    {
        AbilityNameText->SetText(FText::GetEmpty());
    }

    if (TimelineIcon)
    {
        TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    // Disable button
    if (ActivateButton)
    {
        ActivateButton->SetIsEnabled(false);
    }

    // Notify blueprint
    BP_OnAbilityCleared();
}

void UAbilitySlotWidget::StartCooldown()
{
    if (!bIsOnCooldown && CooldownDuration > 0.0f)
    {
        bIsOnCooldown = true;
        CurrentCooldown = CooldownDuration;
        UpdateCooldownDisplay();

        // Notify blueprint
        BP_OnCooldownStarted();
    }
}

void UAbilitySlotWidget::UpdateEnergyCost(float CurrentEnergy)
{
    UpdateEnergyDisplay(CurrentEnergy);
    UpdateAvailability(CanActivate());
}

void UAbilitySlotWidget::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // Update visuals
    UpdateTimelineIcon();
    UpdateAvailability(CanActivate());

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

float UAbilitySlotWidget::GetCooldownProgress() const
{
    if (!bIsOnCooldown || CooldownDuration <= 0.0f)
    {
        return 1.0f;
    }
    return 1.0f - (CurrentCooldown / CooldownDuration);
}

bool UAbilitySlotWidget::CanActivate() const
{
    if (!HasAbility() || bIsOnCooldown)
    {
        return false;
    }

    // Check timeline requirement
    if (RequiredTimeline != ETimelineState::Any && RequiredTimeline != GetCurrentTimelineState())
    {
        return false;
    }

    return true;
}

void UAbilitySlotWidget::InitializeWidgets()
{
    // Setup button callback
    if (ActivateButton)
    {
        ActivateButton->OnClicked.AddDynamic(this, &UAbilitySlotWidget::OnActivateButtonClicked);
        ActivateButton->SetIsEnabled(false);
    }

    // Initialize progress bars
    if (CooldownBar)
    {
        CooldownBar->SetPercent(1.0f);
    }

    if (EnergyBar)
    {
        EnergyBar->SetPercent(1.0f);
    }

    // Hide timeline icon
    if (TimelineIcon)
    {
        TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UAbilitySlotWidget::CreateMaterialInstance()
{
    if (CooldownMaterial && AbilityIcon)
    {
        CooldownMID = UMaterialInstanceDynamic::Create(CooldownMaterial, this);
        AbilityIcon->SetBrushFromMaterial(CooldownMID);
    }
}

void UAbilitySlotWidget::UpdateVisuals()
{
    UpdateTimelineIcon();
    UpdateCooldownDisplay();
    UpdateAvailability(CanActivate());
}

void UAbilitySlotWidget::UpdateTimelineIcon()
{
    if (!TimelineIcon)
    {
        return;
    }

    // Show timeline icon only for timeline-specific abilities
    if (RequiredTimeline == ETimelineState::Any)
    {
        TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    // Set appropriate icon
    UTexture2D* Icon = nullptr;
    switch (RequiredTimeline)
    {
        case ETimelineState::BrightWorld:
            Icon = BrightTimelineIcon;
            break;
        case ETimelineState::DarkWorld:
            Icon = DarkTimelineIcon;
            break;
        default:
            break;
    }

    if (Icon)
    {
        TimelineIcon->SetBrushFromTexture(Icon);
        TimelineIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

void UAbilitySlotWidget::UpdateCooldownDisplay()
{
    // Update cooldown progress
    float Progress = GetCooldownProgress();

    if (CooldownBar)
    {
        CooldownBar->SetPercent(Progress);
    }

    if (CooldownMID)
    {
        CooldownMID->SetScalarParameterValue(TEXT("Progress"), Progress);
    }
}

void UAbilitySlotWidget::UpdateEnergyDisplay(float CurrentEnergy)
{
    if (EnergyBar && EnergyCost > 0.0f)
    {
        float Progress = FMath::Clamp(CurrentEnergy / EnergyCost, 0.0f, 1.0f);
        EnergyBar->SetPercent(Progress);
    }
}

void UAbilitySlotWidget::UpdateAvailability(bool bCanActivate)
{
    FLinearColor Color = bCanActivate ? AvailableColor : UnavailableColor;

    if (AbilityIcon)
    {
        AbilityIcon->SetColorAndOpacity(Color);
    }

    if (AbilityNameText)
    {
        AbilityNameText->SetColorAndOpacity(Color);
    }

    if (ActivateButton)
    {
        ActivateButton->SetIsEnabled(bCanActivate);
    }
}

void UAbilitySlotWidget::OnActivateButtonClicked()
{
    if (CanActivate())
    {
        // Play activation sound
        if (ActivateSound)
        {
            UGameplayStatics::PlaySound2D(this, ActivateSound);
        }

        // Broadcast event
        OnAbilityActivated.Broadcast(AbilityID);
    }
    else if (!bIsOnCooldown)
    {
        // Play insufficient energy sound
        if (InsufficientEnergySound)
        {
            UGameplayStatics::PlaySound2D(this, InsufficientEnergySound);
        }

        // Notify blueprint
        BP_OnInsufficientEnergy();
    }
}

void UAbilitySlotWidget::CompleteCooldown()
{
    bIsOnCooldown = false;
    CurrentCooldown = 0.0f;

    // Update visuals
    UpdateCooldownDisplay();
    UpdateAvailability(CanActivate());

    // Play sound
    if (CooldownCompleteSound)
    {
        UGameplayStatics::PlaySound2D(this, CooldownCompleteSound);
    }

    // Notify events
    OnCooldownComplete.Broadcast(AbilityID);
    BP_OnCooldownCompleted();
}
