// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Core/SETypes.h"
#include "SEGameHUD.generated.h"

class UCombatWidget;
class UQuestLogWidget;
class UTimelineIndicatorWidget;
class UNotificationWidget;
class UDamageNumberWidget;
class UAbilitySlotWidget;

/**
 * Main HUD class for Shadow Echoes RPG
 * Manages all UI widgets and their interactions
 */
UCLASS()
class SHADOWECHOES_API ASEGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    ASEGameHUD();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Widget access */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UCombatWidget* GetCombatWidget() const { return CombatWidget; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UQuestLogWidget* GetQuestLogWidget() const { return QuestLogWidget; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UTimelineIndicatorWidget* GetTimelineIndicatorWidget() const { return TimelineIndicatorWidget; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    UNotificationWidget* GetNotificationWidget() const { return NotificationWidget; }

    /** Combat UI */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void ShowCombatUI();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void HideCombatUI();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateAbilities(const TArray<FAbilityInfo>& Abilities);

    /** Quest UI */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void ShowQuestLog();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void HideQuestLog();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Quests")
    void RefreshQuestLog();

    /** Timeline UI */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Timeline")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Notifications */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ShowNotification(const FText& Message, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ShowQuestNotification(const FText& Message);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Notifications")
    void ShowTimelineNotification(const FText& Message);

    /** Damage numbers */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void SpawnDamageNumber(float Damage, const FVector& WorldLocation, EDamageNumberType Type = EDamageNumberType::Normal);

protected:
    /** Widget classes */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UCombatWidget> CombatWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UQuestLogWidget> QuestLogWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UTimelineIndicatorWidget> TimelineIndicatorWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UNotificationWidget> NotificationWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UDamageNumberWidget> DamageNumberWidgetClass;

private:
    /** Active widgets */
    UPROPERTY()
    UCombatWidget* CombatWidget;

    UPROPERTY()
    UQuestLogWidget* QuestLogWidget;

    UPROPERTY()
    UTimelineIndicatorWidget* TimelineIndicatorWidget;

    UPROPERTY()
    UNotificationWidget* NotificationWidget;

    /** Damage number pool */
    UPROPERTY()
    TArray<UDamageNumberWidget*> DamageNumberPool;

    /** Initialize widgets */
    void CreateWidgets();
    void SetupCallbacks();

    /** Widget management */
    void AddWidgetToViewport(class UUserWidget* Widget, int32 ZOrder = 0);
    void RemoveWidgetFromViewport(class UUserWidget* Widget);

    /** Damage number management */
    UDamageNumberWidget* GetFreeDamageNumberWidget();
    void ReturnDamageNumberToPool(UDamageNumberWidget* Widget);
    FVector2D ProjectWorldLocationToScreen(const FVector& WorldLocation);

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnCombatUIShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnCombatUIHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnQuestLogShown();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnQuestLogHidden();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
