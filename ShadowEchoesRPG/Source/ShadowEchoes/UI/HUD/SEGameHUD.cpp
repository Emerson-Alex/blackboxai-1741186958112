// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "UI/HUD/SEGameHUD.h"
#include "UI/Widgets/CombatWidget.h"
#include "UI/Widgets/QuestLogWidget.h"
#include "UI/Widgets/TimelineIndicatorWidget.h"
#include "UI/Widgets/NotificationWidget.h"
#include "UI/Widgets/DamageNumberWidget.h"
#include "Blueprint/UserWidget.h"
#include "Core/SEGameInstance.h"

ASEGameHUD::ASEGameHUD()
{
    // Set default widget classes
    static ConstructorHelpers::FClassFinder<UCombatWidget> CombatWidgetBP(TEXT("/Game/UI/Widgets/WBP_CombatWidget"));
    if (CombatWidgetBP.Succeeded())
    {
        CombatWidgetClass = CombatWidgetBP.Class;
    }

    static ConstructorHelpers::FClassFinder<UQuestLogWidget> QuestLogWidgetBP(TEXT("/Game/UI/Widgets/WBP_QuestLogWidget"));
    if (QuestLogWidgetBP.Succeeded())
    {
        QuestLogWidgetClass = QuestLogWidgetBP.Class;
    }

    static ConstructorHelpers::FClassFinder<UTimelineIndicatorWidget> TimelineIndicatorWidgetBP(TEXT("/Game/UI/Widgets/WBP_TimelineIndicatorWidget"));
    if (TimelineIndicatorWidgetBP.Succeeded())
    {
        TimelineIndicatorWidgetClass = TimelineIndicatorWidgetBP.Class;
    }

    static ConstructorHelpers::FClassFinder<UNotificationWidget> NotificationWidgetBP(TEXT("/Game/UI/Widgets/WBP_NotificationWidget"));
    if (NotificationWidgetBP.Succeeded())
    {
        NotificationWidgetClass = NotificationWidgetBP.Class;
    }

    static ConstructorHelpers::FClassFinder<UDamageNumberWidget> DamageNumberWidgetBP(TEXT("/Game/UI/Widgets/WBP_DamageNumberWidget"));
    if (DamageNumberWidgetBP.Succeeded())
    {
        DamageNumberWidgetClass = DamageNumberWidgetBP.Class;
    }
}

void ASEGameHUD::BeginPlay()
{
    Super::BeginPlay();

    CreateWidgets();
    SetupCallbacks();
}

void ASEGameHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clean up damage number pool
    for (UDamageNumberWidget* Widget : DamageNumberPool)
    {
        if (Widget)
        {
            Widget->RemoveFromParent();
        }
    }
    DamageNumberPool.Empty();

    Super::EndPlay(EndPlayReason);
}

void ASEGameHUD::ShowCombatUI()
{
    if (CombatWidget)
    {
        CombatWidget->Show();
        BP_OnCombatUIShown();
    }
}

void ASEGameHUD::HideCombatUI()
{
    if (CombatWidget)
    {
        CombatWidget->Hide();
        BP_OnCombatUIHidden();
    }
}

void ASEGameHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (CombatWidget)
    {
        CombatWidget->UpdateHealth(CurrentHealth, MaxHealth);
    }
}

void ASEGameHUD::UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy)
{
    if (CombatWidget)
    {
        CombatWidget->UpdateTimelineEnergy(CurrentEnergy, MaxEnergy);
    }
}

void ASEGameHUD::UpdateAbilities(const TArray<FAbilityInfo>& Abilities)
{
    if (CombatWidget)
    {
        CombatWidget->UpdateAbilities(Abilities);
    }
}

void ASEGameHUD::ShowQuestLog()
{
    if (QuestLogWidget)
    {
        QuestLogWidget->Show();
        QuestLogWidget->RefreshQuestList();
        BP_OnQuestLogShown();
    }
}

void ASEGameHUD::HideQuestLog()
{
    if (QuestLogWidget)
    {
        QuestLogWidget->Hide();
        BP_OnQuestLogHidden();
    }
}

void ASEGameHUD::RefreshQuestLog()
{
    if (QuestLogWidget)
    {
        QuestLogWidget->RefreshQuestList();
    }
}

void ASEGameHUD::OnTimelineStateChanged(ETimelineState NewState)
{
    // Update all widgets
    if (CombatWidget)
    {
        CombatWidget->OnTimelineStateChanged(NewState);
    }

    if (QuestLogWidget)
    {
        QuestLogWidget->OnTimelineStateChanged(NewState);
    }

    if (TimelineIndicatorWidget)
    {
        TimelineIndicatorWidget->OnTimelineStateChanged(NewState);
    }

    // Notify blueprint
    BP_OnTimelineStateChanged(NewState);
}

void ASEGameHUD::ShowNotification(const FText& Message, float Duration)
{
    if (NotificationWidget)
    {
        NotificationWidget->ShowNotification(Message, Duration);
    }
}

void ASEGameHUD::ShowQuestNotification(const FText& Message)
{
    if (NotificationWidget)
    {
        FNotificationInfo Notification;
        Notification.Message = Message;
        Notification.Type = ENotificationType::Quest;
        NotificationWidget->ShowNotificationWithType(Notification);
    }
}

void ASEGameHUD::ShowTimelineNotification(const FText& Message)
{
    if (NotificationWidget)
    {
        FNotificationInfo Notification;
        Notification.Message = Message;
        Notification.Type = ENotificationType::Timeline;
        NotificationWidget->ShowNotificationWithType(Notification);
    }
}

void ASEGameHUD::SpawnDamageNumber(float Damage, const FVector& WorldLocation, EDamageNumberType Type)
{
    UDamageNumberWidget* DamageWidget = GetFreeDamageNumberWidget();
    if (DamageWidget)
    {
        // Set damage and type
        DamageWidget->SetDamageNumber(Damage, Type);

        // Position widget at world location
        FVector2D ScreenPosition = ProjectWorldLocationToScreen(WorldLocation);
        DamageWidget->SetStartPosition(ScreenPosition);

        // Set random upward movement direction
        float RandomAngle = FMath::RandRange(-30.0f, 30.0f);
        FVector2D Direction = FVector2D(FMath::Sin(FMath::DegreesToRadians(RandomAngle)), -FMath::Cos(FMath::DegreesToRadians(RandomAngle)));
        DamageWidget->SetMovementDirection(Direction);
    }
}

void ASEGameHUD::CreateWidgets()
{
    if (CombatWidgetClass)
    {
        CombatWidget = CreateWidget<UCombatWidget>(GetOwningPlayerController(), CombatWidgetClass);
        AddWidgetToViewport(CombatWidget, 1);
    }

    if (QuestLogWidgetClass)
    {
        QuestLogWidget = CreateWidget<UQuestLogWidget>(GetOwningPlayerController(), QuestLogWidgetClass);
        AddWidgetToViewport(QuestLogWidget, 2);
    }

    if (TimelineIndicatorWidgetClass)
    {
        TimelineIndicatorWidget = CreateWidget<UTimelineIndicatorWidget>(GetOwningPlayerController(), TimelineIndicatorWidgetClass);
        AddWidgetToViewport(TimelineIndicatorWidget, 3);
    }

    if (NotificationWidgetClass)
    {
        NotificationWidget = CreateWidget<UNotificationWidget>(GetOwningPlayerController(), NotificationWidgetClass);
        AddWidgetToViewport(NotificationWidget, 4);
    }
}

void ASEGameHUD::SetupCallbacks()
{
    // Register for timeline state changes
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetGameInstance()))
    {
        GameInstance->OnTimelineStateChanged.AddDynamic(this, &ASEGameHUD::OnTimelineStateChanged);
    }
}

void ASEGameHUD::AddWidgetToViewport(UUserWidget* Widget, int32 ZOrder)
{
    if (Widget)
    {
        Widget->AddToViewport(ZOrder);
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void ASEGameHUD::RemoveWidgetFromViewport(UUserWidget* Widget)
{
    if (Widget)
    {
        Widget->RemoveFromParent();
    }
}

UDamageNumberWidget* ASEGameHUD::GetFreeDamageNumberWidget()
{
    // Try to find an inactive widget in the pool
    for (UDamageNumberWidget* Widget : DamageNumberPool)
    {
        if (Widget && !Widget->IsShown())
        {
            return Widget;
        }
    }

    // Create new widget if needed
    if (DamageNumberWidgetClass)
    {
        UDamageNumberWidget* NewWidget = CreateWidget<UDamageNumberWidget>(GetOwningPlayerController(), DamageNumberWidgetClass);
        if (NewWidget)
        {
            DamageNumberPool.Add(NewWidget);
            AddWidgetToViewport(NewWidget, 5);
            return NewWidget;
        }
    }

    return nullptr;
}

void ASEGameHUD::ReturnDamageNumberToPool(UDamageNumberWidget* Widget)
{
    if (Widget)
    {
        Widget->Hide();
    }
}

FVector2D ASEGameHUD::ProjectWorldLocationToScreen(const FVector& WorldLocation)
{
    FVector2D ScreenLocation;
    if (ProjectWorldLocationToScreen(WorldLocation, ScreenLocation))
    {
        return ScreenLocation;
    }
    return FVector2D::ZeroVector;
}
