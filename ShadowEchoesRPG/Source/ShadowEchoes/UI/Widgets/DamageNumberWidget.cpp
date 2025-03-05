// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/DamageNumberWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Curves/CurveFloat.h"

UDamageNumberWidget::UDamageNumberWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentDamage(0.0f)
    , DamageType(EDamageNumberType::Normal)
    , CurrentLifetime(0.0f)
    , MovementSpeed(200.0f)
    , Lifetime(1.5f)
    , bIsActive(false)
    , NormalColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
    , CriticalColor(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f))
    , TimelineColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f))
    , HealingColor(FLinearColor(0.2f, 1.0f, 0.2f, 1.0f))
{
    // Enable tick for movement and animations
    PrimaryComponentTick.bCanEverTick = true;
}

void UDamageNumberWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidget();

    // Bind animation finished events
    if (FadeAnimation)
    {
        FadeAnimation->OnAnimationFinished.AddDynamic(this, &UDamageNumberWidget::OnFadeAnimationFinished);
    }
}

void UDamageNumberWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsActive)
    {
        // Update lifetime
        CurrentLifetime += InDeltaTime;
        float Progress = FMath::Clamp(CurrentLifetime / Lifetime, 0.0f, 1.0f);

        // Update movement
        UpdateMovement(InDeltaTime);

        // Update scale and opacity
        UpdateScale(Progress);
        UpdateOpacity(Progress);

        // Check for expiration
        if (CurrentLifetime >= Lifetime)
        {
            bIsActive = false;
            BP_OnLifetimeExpired();
        }
    }
}

void UDamageNumberWidget::SetDamageNumber(float Damage, EDamageNumberType Type)
{
    CurrentDamage = Damage;
    DamageType = Type;
    CurrentLifetime = 0.0f;
    bIsActive = true;

    // Update text
    if (DamageText)
    {
        DamageText->SetText(GetDamageText());
        DamageText->SetColorAndOpacity(GetDamageColor());
    }

    // Play animations
    if (ScaleAnimation)
    {
        PlayAnimation(ScaleAnimation);
    }

    // Notify blueprint
    BP_OnDamageSet(Damage, Type);
}

void UDamageNumberWidget::SetMovementDirection(const FVector2D& Direction)
{
    MovementDir = Direction.GetSafeNormal();
}

void UDamageNumberWidget::SetStartPosition(const FVector2D& Position)
{
    CurrentPosition = Position;
    SetRenderTranslation(CurrentPosition);
}

void UDamageNumberWidget::InitializeWidget()
{
    // Set initial visibility
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    // Initialize text
    if (DamageText)
    {
        DamageText->SetText(FText::GetEmpty());
    }
}

void UDamageNumberWidget::UpdateMovement(float DeltaTime)
{
    // Apply movement curve if available
    float SpeedMultiplier = 1.0f;
    if (MovementCurve)
    {
        SpeedMultiplier = MovementCurve->GetFloatValue(CurrentLifetime / Lifetime);
    }

    // Update position
    CurrentPosition += MovementDir * MovementSpeed * SpeedMultiplier * DeltaTime;
    SetRenderTranslation(CurrentPosition);
}

void UDamageNumberWidget::UpdateScale(float Progress)
{
    float Scale = 1.0f;
    if (ScaleCurve)
    {
        Scale = ScaleCurve->GetFloatValue(Progress);
    }
    SetRenderScale(FVector2D(Scale));
}

void UDamageNumberWidget::UpdateOpacity(float Progress)
{
    float Opacity = 1.0f;
    if (OpacityCurve)
    {
        Opacity = OpacityCurve->GetFloatValue(Progress);
    }
    SetRenderOpacity(Opacity);
}

FLinearColor UDamageNumberWidget::GetDamageColor() const
{
    switch (DamageType)
    {
        case EDamageNumberType::Critical:
            return CriticalColor;
        case EDamageNumberType::Timeline:
            return TimelineColor;
        case EDamageNumberType::Healing:
            return HealingColor;
        default:
            return NormalColor;
    }
}

FText UDamageNumberWidget::GetDamageText() const
{
    // Format number based on type
    FString Prefix;
    if (DamageType == EDamageNumberType::Healing)
    {
        Prefix = TEXT("+");
    }

    // Add special formatting for critical hits
    if (DamageType == EDamageNumberType::Critical)
    {
        return FText::FromString(FString::Printf(TEXT("%s%d!"), *Prefix, FMath::RoundToInt(CurrentDamage)));
    }

    // Standard formatting
    return FText::FromString(FString::Printf(TEXT("%s%d"), *Prefix, FMath::RoundToInt(CurrentDamage)));
}

void UDamageNumberWidget::OnFadeAnimationFinished()
{
    // Remove widget when fade completes
    RemoveFromParent();
}
