// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/CombatWidget.h"
#include "UI/Widgets/AbilitySlotWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

UCombatWidget::UCombatWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentHealthPercent(1.0f)
    , CurrentEnergyPercent(1.0f)
    , bIsLowHealth(false)
    , bIsLowEnergy(false)
    , HealthBarColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f))
    , LowHealthColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f))
    , EnergyBarColor(FLinearColor(0.0f, 0.8f, 1.0f, 1.0f))
    , LowEnergyColor(FLinearColor(0.3f, 0.3f, 1.0f, 1.0f))
{
}

void UCombatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidgets();
    CreateMaterialInstances();
}

void UCombatWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    // Calculate percentage
    CurrentHealthPercent = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;

    // Update progress bar
    if (HealthBar)
    {
        HealthBar->SetPercent(CurrentHealthPercent);
    }

    // Update text
    if (HealthText)
    {
        HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth))));
    }

    // Update visuals
    UpdateHealthVisuals();

    // Check for low health
    CheckLowHealth();

    // Notify blueprint
    BP_OnHealthChanged(CurrentHealth, MaxHealth);
}

void UCombatWidget::UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy)
{
    // Calculate percentage
    CurrentEnergyPercent = MaxEnergy > 0.0f ? CurrentEnergy / MaxEnergy : 0.0f;

    // Update progress bar
    if (TimelineEnergyBar)
    {
        TimelineEnergyBar->SetPercent(CurrentEnergyPercent);
    }

    // Update text
    if (TimelineEnergyText)
    {
        TimelineEnergyText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), FMath::RoundToInt(CurrentEnergy), FMath::RoundToInt(MaxEnergy))));
    }

    // Update visuals
    UpdateEnergyVisuals();

    // Update ability slots
    UpdateAbilitySlotEnergy();

    // Check for low energy
    CheckLowEnergy();

    // Notify blueprint
    BP_OnEnergyChanged(CurrentEnergy, MaxEnergy);
}

void UCombatWidget::UpdateAbilities(const TArray<FAbilityInfo>& Abilities)
{
    // Clear existing abilities
    ClearAbilities();

    // Create new ability slots
    for (const FAbilityInfo& Ability : Abilities)
    {
        UAbilitySlotWidget* SlotWidget = CreateAbilitySlot();
        if (SlotWidget)
        {
            SlotWidget->SetAbility(Ability);
            AbilitySlots.Add(SlotWidget);
        }
    }

    // Update energy costs
    UpdateAbilitySlotEnergy();

    // Notify blueprint
    BP_OnAbilitiesUpdated();
}

void UCombatWidget::ClearAbilities()
{
    // Clear container
    if (AbilityContainer)
    {
        AbilityContainer->ClearChildren();
    }

    // Clear array
    AbilitySlots.Empty();
}

void UCombatWidget::ShowWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UCombatWidget::HideWidget()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UCombatWidget::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // Update timeline icon
    UpdateTimelineIcon();

    // Update ability slots
    for (UAbilitySlotWidget* Slot : AbilitySlots)
    {
        if (Slot)
        {
            Slot->OnTimelineStateChanged(NewState);
        }
    }

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

void UCombatWidget::InitializeWidgets()
{
    // Set initial visibility
    SetVisibility(ESlateVisibility::Hidden);

    // Initialize progress bars
    if (HealthBar)
    {
        HealthBar->SetPercent(1.0f);
    }

    if (TimelineEnergyBar)
    {
        TimelineEnergyBar->SetPercent(1.0f);
    }

    // Initialize timeline icon
    UpdateTimelineIcon();
}

void UCombatWidget::CreateMaterialInstances()
{
    // Create health bar material instance
    if (HealthBarMaterial && HealthBar)
    {
        HealthBarMID = UMaterialInstanceDynamic::Create(HealthBarMaterial, this);
        HealthBar->SetFillColorAndOpacity(HealthBarColor);
    }

    // Create energy bar material instance
    if (EnergyBarMaterial && TimelineEnergyBar)
    {
        EnergyBarMID = UMaterialInstanceDynamic::Create(EnergyBarMaterial, this);
        TimelineEnergyBar->SetFillColorAndOpacity(EnergyBarColor);
    }
}

void UCombatWidget::UpdateHealthVisuals()
{
    if (HealthBarMID)
    {
        // Update health bar color based on percentage
        FLinearColor Color = FLinearColor::LerpUsingHSV(LowHealthColor, HealthBarColor, CurrentHealthPercent);
        HealthBarMID->SetVectorParameterValue(TEXT("Color"), Color);
    }
}

void UCombatWidget::UpdateEnergyVisuals()
{
    if (EnergyBarMID)
    {
        // Update energy bar color based on percentage
        FLinearColor Color = FLinearColor::LerpUsingHSV(LowEnergyColor, EnergyBarColor, CurrentEnergyPercent);
        EnergyBarMID->SetVectorParameterValue(TEXT("Color"), Color);
    }
}

void UCombatWidget::UpdateTimelineIcon()
{
    if (!TimelineIcon)
    {
        return;
    }

    // Set appropriate icon
    UTexture2D* Icon = nullptr;
    switch (GetCurrentTimelineState())
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
    else
    {
        TimelineIcon->SetVisibility(ESlateVisibility::Hidden);
    }
}

UAbilitySlotWidget* UCombatWidget::CreateAbilitySlot()
{
    if (!AbilityContainer || !AbilitySlotClass)
    {
        return nullptr;
    }

    // Create widget
    UAbilitySlotWidget* SlotWidget = CreateWidget<UAbilitySlotWidget>(this, AbilitySlotClass);
    if (SlotWidget)
    {
        AbilityContainer->AddChild(SlotWidget);
    }

    return SlotWidget;
}

void UCombatWidget::UpdateAbilitySlotEnergy()
{
    float CurrentEnergy = CurrentEnergyPercent * 100.0f;
    for (UAbilitySlotWidget* Slot : AbilitySlots)
    {
        if (Slot)
        {
            Slot->UpdateEnergyCost(CurrentEnergy);
        }
    }
}

void UCombatWidget::CheckLowHealth()
{
    static const float LowHealthThreshold = 0.3f;
    bool bWasLowHealth = bIsLowHealth;
    bIsLowHealth = CurrentHealthPercent <= LowHealthThreshold;

    if (bIsLowHealth && !bWasLowHealth)
    {
        // Play low health sound
        if (LowHealthSound)
        {
            UGameplayStatics::PlaySound2D(this, LowHealthSound);
        }

        // Notify blueprint
        BP_OnLowHealth(true);
    }
    else if (!bIsLowHealth && bWasLowHealth)
    {
        // Notify blueprint
        BP_OnLowHealth(false);
    }
}

void UCombatWidget::CheckLowEnergy()
{
    static const float LowEnergyThreshold = 0.2f;
    bool bWasLowEnergy = bIsLowEnergy;
    bIsLowEnergy = CurrentEnergyPercent <= LowEnergyThreshold;

    if (bIsLowEnergy && !bWasLowEnergy)
    {
        // Play low energy sound
        if (LowEnergySound)
        {
            UGameplayStatics::PlaySound2D(this, LowEnergySound);
        }

        // Notify blueprint
        BP_OnLowEnergy(true);
    }
    else if (!bIsLowEnergy && bWasLowEnergy)
    {
        // Notify blueprint
        BP_OnLowEnergy(false);
    }
}
