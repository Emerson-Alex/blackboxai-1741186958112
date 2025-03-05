// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "NotificationWidget.generated.h"

class UTextBlock;
class UImage;
class UWidgetAnimation;
class USoundBase;

UENUM(BlueprintType)
enum class ENotificationType : uint8
{
    Default     UMETA(DisplayName = "Default"),
    Quest       UMETA(DisplayName = "Quest"),
    Timeline    UMETA(DisplayName = "Timeline"),
    Combat      UMETA(DisplayName = "Combat"),
    Achievement UMETA(DisplayName = "Achievement")
};

USTRUCT(BlueprintType)
struct FNotificationInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    FText Message;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    ENotificationType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    USoundBase* Sound;

    FNotificationInfo()
        : Type(ENotificationType::Default)
        , Duration(3.0f)
        , Sound(nullptr)
    {
    }
};

/**
 * Widget for displaying game notifications and messages
 */
UCLASS()
class SHADOWECHOES_API UNotificationWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UNotificationWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Show notification */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ShowNotification(const FText& Message, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ShowNotificationWithType(const FNotificationInfo& Notification);

    /** Queue management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ClearNotifications();

    /** Timeline state */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* NotificationText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* NotificationIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* BackgroundImage;

    /** Animations */
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ShowAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HideAnimation;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* DefaultIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* QuestIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* TimelineIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* CombatIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* AchievementIcon;

    /** Colors */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor DefaultColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor QuestColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor TimelineColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor CombatColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor AchievementColor;

    /** Sound effects */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* DefaultSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* QuestSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* TimelineSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* CombatSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* AchievementSound;

private:
    /** Notification queue */
    TArray<FNotificationInfo> NotificationQueue;
    float CurrentNotificationTime;
    bool bIsShowingNotification;

    /** Initialize UI */
    void InitializeWidgets();

    /** Queue processing */
    void ProcessNotificationQueue();
    void ShowNextNotification();
    void CompleteCurrentNotification();

    /** Visual updates */
    void UpdateNotificationStyle(ENotificationType Type);
    UTexture2D* GetNotificationIcon(ENotificationType Type) const;
    FLinearColor GetNotificationColor(ENotificationType Type) const;
    USoundBase* GetNotificationSound(ENotificationType Type) const;

    /** Animation handlers */
    UFUNCTION()
    void OnShowAnimationFinished();

    UFUNCTION()
    void OnHideAnimationFinished();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Notifications")
    void BP_OnNotificationShown(const FNotificationInfo& Notification);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Notifications")
    void BP_OnNotificationHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Notifications")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
