// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/Widgets/NotificationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"

UNotificationWidget::UNotificationWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentNotificationTime(0.0f)
    , bIsShowingNotification(false)
    , DefaultColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
    , QuestColor(FLinearColor(0.0f, 0.8f, 1.0f, 1.0f))
    , TimelineColor(FLinearColor(1.0f, 0.8f, 0.0f, 1.0f))
    , CombatColor(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f))
    , AchievementColor(FLinearColor(0.8f, 0.4f, 1.0f, 1.0f))
{
    // Enable tick for notification timing
    PrimaryComponentTick.bCanEverTick = true;
}

void UNotificationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeWidgets();

    // Bind animation finished events
    if (ShowAnimation)
    {
        ShowAnimation->OnAnimationFinished.AddDynamic(this, &UNotificationWidget::OnShowAnimationFinished);
    }
    if (HideAnimation)
    {
        HideAnimation->OnAnimationFinished.AddDynamic(this, &UNotificationWidget::OnHideAnimationFinished);
    }
}

void UNotificationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsShowingNotification)
    {
        CurrentNotificationTime -= InDeltaTime;
        if (CurrentNotificationTime <= 0.0f)
        {
            CompleteCurrentNotification();
        }
    }
}

void UNotificationWidget::ShowNotification(const FText& Message, float Duration)
{
    FNotificationInfo Notification;
    Notification.Message = Message;
    Notification.Duration = Duration;
    ShowNotificationWithType(Notification);
}

void UNotificationWidget::ShowNotificationWithType(const FNotificationInfo& Notification)
{
    // Add to queue
    NotificationQueue.Add(Notification);

    // Process queue if not currently showing a notification
    if (!bIsShowingNotification)
    {
        ProcessNotificationQueue();
    }
}

void UNotificationWidget::ClearNotifications()
{
    // Clear queue
    NotificationQueue.Empty();

    // Hide current notification if showing
    if (bIsShowingNotification)
    {
        if (HideAnimation)
        {
            PlayAnimation(HideAnimation);
        }
        else
        {
            SetVisibility(ESlateVisibility::Hidden);
            bIsShowingNotification = false;
        }
    }
}

void UNotificationWidget::OnTimelineStateChanged(ETimelineState NewState)
{
    Super::OnTimelineStateChanged(NewState);

    // Update visuals for current notification if showing
    if (bIsShowingNotification)
    {
        UpdateNotificationStyle(NotificationQueue[0].Type);
    }

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

void UNotificationWidget::InitializeWidgets()
{
    // Initialize visibility
    SetVisibility(ESlateVisibility::Hidden);

    // Set default icon
    if (NotificationIcon && DefaultIcon)
    {
        NotificationIcon->SetBrushFromTexture(DefaultIcon);
    }
}

void UNotificationWidget::ProcessNotificationQueue()
{
    if (NotificationQueue.Num() > 0 && !bIsShowingNotification)
    {
        ShowNextNotification();
    }
}

void UNotificationWidget::ShowNextNotification()
{
    if (NotificationQueue.Num() == 0)
    {
        return;
    }

    // Get next notification
    const FNotificationInfo& Notification = NotificationQueue[0];

    // Set notification text
    if (NotificationText)
    {
        NotificationText->SetText(Notification.Message);
    }

    // Update style
    UpdateNotificationStyle(Notification.Type);

    // Play sound
    USoundBase* Sound = Notification.Sound ? Notification.Sound : GetNotificationSound(Notification.Type);
    if (Sound)
    {
        UGameplayStatics::PlaySound2D(this, Sound);
    }

    // Show widget
    if (ShowAnimation)
    {
        PlayAnimation(ShowAnimation);
    }
    else
    {
        SetVisibility(ESlateVisibility::Visible);
        OnShowAnimationFinished();
    }

    // Start timer
    CurrentNotificationTime = Notification.Duration;
    bIsShowingNotification = true;

    // Notify blueprint
    BP_OnNotificationShown(Notification);
}

void UNotificationWidget::CompleteCurrentNotification()
{
    if (HideAnimation)
    {
        PlayAnimation(HideAnimation);
    }
    else
    {
        OnHideAnimationFinished();
    }
}

void UNotificationWidget::UpdateNotificationStyle(ENotificationType Type)
{
    // Update icon
    if (NotificationIcon)
    {
        if (UTexture2D* Icon = GetNotificationIcon(Type))
        {
            NotificationIcon->SetBrushFromTexture(Icon);
        }
    }

    // Update colors
    FLinearColor Color = GetNotificationColor(Type);
    if (NotificationText)
    {
        NotificationText->SetColorAndOpacity(Color);
    }
    if (NotificationIcon)
    {
        NotificationIcon->SetColorAndOpacity(Color);
    }
    if (BackgroundImage)
    {
        BackgroundImage->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, 0.2f));
    }
}

UTexture2D* UNotificationWidget::GetNotificationIcon(ENotificationType Type) const
{
    switch (Type)
    {
        case ENotificationType::Quest:
            return QuestIcon;
        case ENotificationType::Timeline:
            return TimelineIcon;
        case ENotificationType::Combat:
            return CombatIcon;
        case ENotificationType::Achievement:
            return AchievementIcon;
        default:
            return DefaultIcon;
    }
}

FLinearColor UNotificationWidget::GetNotificationColor(ENotificationType Type) const
{
    switch (Type)
    {
        case ENotificationType::Quest:
            return QuestColor;
        case ENotificationType::Timeline:
            return TimelineColor;
        case ENotificationType::Combat:
            return CombatColor;
        case ENotificationType::Achievement:
            return AchievementColor;
        default:
            return DefaultColor;
    }
}

USoundBase* UNotificationWidget::GetNotificationSound(ENotificationType Type) const
{
    switch (Type)
    {
        case ENotificationType::Quest:
            return QuestSound;
        case ENotificationType::Timeline:
            return TimelineSound;
        case ENotificationType::Combat:
            return CombatSound;
        case ENotificationType::Achievement:
            return AchievementSound;
        default:
            return DefaultSound;
    }
}

void UNotificationWidget::OnShowAnimationFinished()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UNotificationWidget::OnHideAnimationFinished()
{
    // Hide widget
    SetVisibility(ESlateVisibility::Hidden);
    bIsShowingNotification = false;

    // Remove completed notification
    if (NotificationQueue.Num() > 0)
    {
        NotificationQueue.RemoveAt(0);
    }

    // Notify blueprint
    BP_OnNotificationHidden();

    // Process next notification
    ProcessNotificationQueue();
}
