// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Core/SEBaseWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SEGameInstance.h"

USEBaseWidget::USEBaseWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentTimelineState(ETimelineState::BrightWorld)
    , bIsShown(false)
    , bIsAnimating(false)
    , DefaultOpacity(1.0f)
    , FadeInDuration(0.25f)
    , FadeOutDuration(0.25f)
{
}

void USEBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidget();

    // Get initial timeline state
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        CurrentTimelineState = GameInstance->GetCurrentTimelineState();
        UpdateTimelineVisuals();
    }
}

void USEBaseWidget::NativeDestruct()
{
    // Cleanup any active animations
    if (bIsAnimating)
    {
        if (ShowAnim)
        {
            ShowAnim->Stop();
        }
        if (HideAnim)
        {
            HideAnim->Stop();
        }
    }

    Super::NativeDestruct();
}

void USEBaseWidget::OnTimelineStateChanged(ETimelineState NewState)
{
    if (CurrentTimelineState != NewState)
    {
        CurrentTimelineState = NewState;
        UpdateTimelineVisuals();

        // Notify blueprint
        BP_OnTimelineStateChanged(NewState);
    }
}

void USEBaseWidget::Show()
{
    if (!bIsShown)
    {
        bIsShown = true;
        SetVisibility(ESlateVisibility::Visible);
        PlayShowAnimation();

        // Play sound
        if (ShowSound)
        {
            PlayWidgetSound(ShowSound);
        }

        // Notify blueprint
        BP_OnShow();
    }
}

void USEBaseWidget::Hide()
{
    if (bIsShown)
    {
        bIsShown = false;
        PlayHideAnimation();

        // Play sound
        if (HideSound)
        {
            PlayWidgetSound(HideSound);
        }

        // Notify blueprint
        BP_OnHide();
    }
}

void USEBaseWidget::PlayShowAnimation()
{
    if (ShowAnim)
    {
        // Bind completion callback
        if (!ShowAnim->OnAnimationFinished.IsAlreadyBound(this, &USEBaseWidget::OnShowAnimationFinished))
        {
            ShowAnim->OnAnimationFinished.AddDynamic(this, &USEBaseWidget::OnShowAnimationFinished);
        }

        // Play animation
        PlayAnimation(ShowAnim);
        bIsAnimating = true;
    }
    else
    {
        // No animation, just fade in
        FadeIn();
    }
}

void USEBaseWidget::PlayHideAnimation()
{
    if (HideAnim)
    {
        // Bind completion callback
        if (!HideAnim->OnAnimationFinished.IsAlreadyBound(this, &USEBaseWidget::OnHideAnimationFinished))
        {
            HideAnim->OnAnimationFinished.AddDynamic(this, &USEBaseWidget::OnHideAnimationFinished);
        }

        // Play animation
        PlayAnimation(HideAnim);
        bIsAnimating = true;
    }
    else
    {
        // No animation, just fade out
        FadeOut();
    }
}

void USEBaseWidget::PlayWidgetSound(USoundBase* Sound)
{
    if (Sound)
    {
        UGameplayStatics::PlaySound2D(this, Sound);
    }
}

void USEBaseWidget::OnShowAnimationFinished()
{
    bIsAnimating = false;
    SetWidgetOpacity(DefaultOpacity);

    // Notify blueprint
    BP_OnShowAnimationFinished();
}

void USEBaseWidget::OnHideAnimationFinished()
{
    bIsAnimating = false;
    SetVisibility(ESlateVisibility::Hidden);

    // Notify blueprint
    BP_OnHideAnimationFinished();
}

void USEBaseWidget::InitializeWidget()
{
    // Set initial state
    bIsShown = false;
    bIsAnimating = false;
    SetVisibility(ESlateVisibility::Hidden);
    SetWidgetOpacity(DefaultOpacity);
}

void USEBaseWidget::UpdateTimelineVisuals()
{
    // Override in child classes to update timeline-specific visuals
}

void USEBaseWidget::SetWidgetOpacity(float Opacity)
{
    SetRenderOpacity(Opacity);
}

void USEBaseWidget::FadeIn(float Duration)
{
    if (Duration < 0.0f)
    {
        Duration = FadeInDuration;
    }

    // Start at zero opacity
    SetWidgetOpacity(0.0f);
    SetVisibility(ESlateVisibility::Visible);

    // Create timeline for fade
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [this]()
        {
            SetWidgetOpacity(DefaultOpacity);
            OnShowAnimationFinished();
        },
        Duration,
        false
    );
}

void USEBaseWidget::FadeOut(float Duration)
{
    if (Duration < 0.0f)
    {
        Duration = FadeOutDuration;
    }

    // Start at full opacity
    SetWidgetOpacity(DefaultOpacity);

    // Create timeline for fade
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [this]()
        {
            SetWidgetOpacity(0.0f);
            OnHideAnimationFinished();
        },
        Duration,
        false
    );
}

bool USEBaseWidget::IsTimelineCompatible(ETimelineState RequiredState) const
{
    return RequiredState == ETimelineState::Any || RequiredState == CurrentTimelineState;
}

FLinearColor USEBaseWidget::GetTimelineColor(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::BrightWorld:
            return FLinearColor(1.0f, 0.9f, 0.8f); // Warm, bright color
        case ETimelineState::DarkWorld:
            return FLinearColor(0.2f, 0.3f, 0.4f); // Cool, dark color
        default:
            return FLinearColor::White;
    }
}

FText USEBaseWidget::GetTimelineText(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::BrightWorld:
            return FText::FromString(TEXT("Bright Timeline"));
        case ETimelineState::DarkWorld:
            return FText::FromString(TEXT("Dark Timeline"));
        default:
            return FText::GetEmpty();
    }
}
