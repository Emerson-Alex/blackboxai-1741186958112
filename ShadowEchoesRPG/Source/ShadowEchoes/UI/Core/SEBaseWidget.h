// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SETypes.h"
#include "SEBaseWidget.generated.h"

/**
 * Base widget class for Shadow Echoes RPG
 * Provides common functionality for timeline state handling and widget management
 */
UCLASS(Abstract)
class SHADOWECHOES_API USEBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    USEBaseWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Timeline state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    ETimelineState GetCurrentTimelineState() const { return CurrentTimelineState; }

    /** Widget state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void Show();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void Hide();

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    bool IsShown() const { return bIsShown; }

    /** Animation control */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void PlayShowAnimation();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void PlayHideAnimation();

    /** Sound effects */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void PlayWidgetSound(USoundBase* Sound);

protected:
    /** Animation settings */
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ShowAnim;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* HideAnim;

    /** Sound settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* ShowSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* HideSound;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    float DefaultOpacity;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    float FadeInDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    float FadeOutDuration;

private:
    /** Current state */
    ETimelineState CurrentTimelineState;
    bool bIsShown;
    bool bIsAnimating;

    /** Animation handlers */
    UFUNCTION()
    virtual void OnShowAnimationFinished();

    UFUNCTION()
    virtual void OnHideAnimationFinished();

    /** Initialize widget */
    virtual void InitializeWidget();

    /** Update visuals */
    virtual void UpdateTimelineVisuals();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnShow();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnHide();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnShowAnimationFinished();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI")
    void BP_OnHideAnimationFinished();

    /** Helper functions */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void SetWidgetOpacity(float Opacity);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void FadeIn(float Duration = -1.0f);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    void FadeOut(float Duration = -1.0f);

    /** Timeline helpers */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    bool IsTimelineCompatible(ETimelineState RequiredState) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    FLinearColor GetTimelineColor(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    FText GetTimelineText(ETimelineState State) const;
};
