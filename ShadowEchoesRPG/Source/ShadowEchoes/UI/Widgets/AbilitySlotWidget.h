// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "AbilitySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class UButton;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityActivated, const FName&, AbilityID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownComplete, const FName&, AbilityID);

/**
 * Widget for displaying and managing individual ability slots
 */
UCLASS()
class SHADOWECHOES_API UAbilitySlotWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UAbilitySlotWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Ability setup */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Abilities")
    void SetAbility(const FAbilityInfo& Ability);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Abilities")
    void ClearAbility();

    /** Ability state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Abilities")
    void StartCooldown();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Abilities")
    void UpdateEnergyCost(float CurrentEnergy);

    /** Timeline state */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;

    /** Getters */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Abilities")
    bool HasAbility() const { return !AbilityID.IsNone(); }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Abilities")
    bool IsOnCooldown() const { return bIsOnCooldown; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Abilities")
    float GetCooldownProgress() const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI|Abilities")
    bool CanActivate() const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|UI|Abilities")
    FOnAbilityActivated OnAbilityActivated;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|UI|Abilities")
    FOnCooldownComplete OnCooldownComplete;

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* AbilityIcon;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* AbilityNameText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* CooldownBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* EnergyBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* ActivateButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TimelineIcon;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UMaterialInterface* CooldownMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor AvailableColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor UnavailableColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor CooldownColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* BrightTimelineIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* DarkTimelineIcon;

    /** Sound effects */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* ActivateSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* CooldownCompleteSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* InsufficientEnergySound;

private:
    /** Ability data */
    FName AbilityID;
    float CooldownDuration;
    float CurrentCooldown;
    float EnergyCost;
    ETimelineState RequiredTimeline;
    bool bIsOnCooldown;

    /** Material instance */
    UPROPERTY()
    UMaterialInstanceDynamic* CooldownMID;

    /** Initialize UI */
    void InitializeWidgets();
    void CreateMaterialInstance();

    /** Update visuals */
    void UpdateVisuals();
    void UpdateTimelineIcon();
    void UpdateCooldownDisplay();
    void UpdateEnergyDisplay(float CurrentEnergy);
    void UpdateAvailability(bool bCanActivate);

    /** Button handlers */
    UFUNCTION()
    void OnActivateButtonClicked();

    /** Cooldown handlers */
    void CompleteCooldown();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnAbilitySet(const FAbilityInfo& Ability);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnAbilityCleared();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnCooldownStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnCooldownCompleted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Abilities")
    void BP_OnInsufficientEnergy();
};
