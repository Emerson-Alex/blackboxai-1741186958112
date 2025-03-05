#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETimelineTypes.h"
#include "TimelineIndicatorWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class UButton;
class UMaterialInstanceDynamic;
class USETimelineStateManager;

UCLASS()
class SHADOWECHOES_API UTimelineIndicatorWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UTimelineIndicatorWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    // Timeline State Updates
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void UpdateTimelineState(ETimelineState NewState);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void ShowTransitionEffect(ETimelineState FromState, ETimelineState ToState, float Duration);

protected:
    // UI Components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TimelineIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* StateText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* EnergyBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* LightStateButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* DarkStateButton;

    // Visual Assets
    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Visuals")
    UMaterialInterface* TimelineMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Visuals")
    UTexture2D* LightStateIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Visuals")
    UTexture2D* DarkStateIcon;

    // Animation Settings
    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Animation")
    float TransitionDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Animation")
    UCurveFloat* TransitionCurve;

    // Colors
    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Colors")
    FLinearColor LightStateColor;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Colors")
    FLinearColor DarkStateColor;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Colors")
    FLinearColor DisabledStateColor;

    // Button Events
    UFUNCTION()
    void OnLightStateClicked();

    UFUNCTION()
    void OnDarkStateClicked();

    // State Management
    UFUNCTION()
    void HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState);

    UFUNCTION()
    void HandleEnergyChanged(float NewEnergy, float MaxEnergy);

private:
    // Internal state
    UPROPERTY()
    UMaterialInstanceDynamic* TimelineMaterialInstance;

    UPROPERTY()
    USETimelineStateManager* TimelineManager;

    ETimelineState CurrentState;
    float CurrentEnergy;
    float MaxEnergy;
    bool bIsTransitioning;
    float TransitionProgress;
    ETimelineState TransitionFromState;
    ETimelineState TransitionToState;

    // Initialization
    void InitializeTimelineMaterial();
    void SetupButtonBindings();
    void RegisterTimelineCallbacks();

    // Visual Updates
    void UpdateTimelineIcon();
    void UpdateStateText();
    void UpdateEnergyBar();
    void UpdateButtonStates();
    void UpdateTransitionEffect(float DeltaTime);

    // Helper Functions
    FString GetStateDisplayText(ETimelineState State) const;
    FLinearColor GetStateColor(ETimelineState State) const;
    bool CanTransitionToState(ETimelineState State) const;
    void StartTransitionAnimation(ETimelineState FromState, ETimelineState ToState);
    void EndTransitionAnimation();

    // Material Parameters
    static const FName TimelineBlendParam;
    static const FName StateColorParam;
    static const FName TransitionProgressParam;
};
