// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "CombatWidget.generated.h"

class UAbilitySlotWidget;
class UProgressBar;
class UTextBlock;
class UHorizontalBox;
class UImage;
class UMaterialInstanceDynamic;

/**
 * Widget for displaying combat information and abilities
 */
UCLASS()
class SHADOWECHOES_API UCombatWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UCombatWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;

    /** Health management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    /** Timeline energy management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateTimelineEnergy(float CurrentEnergy, float MaxEnergy);

    /** Ability management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void UpdateAbilities(const TArray<FAbilityInfo>& Abilities);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void ClearAbilities();

    /** Combat state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void ShowWidget();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void HideWidget();

    /** Timeline state */
    virtual void OnTimelineStateChanged(ETimelineState NewState) override;

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* HealthText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UProgressBar* TimelineEnergyBar;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* TimelineEnergyText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* AbilityContainer;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UImage* TimelineIcon;

    /** Widget classes */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Widgets")
    TSubclassOf<UAbilitySlotWidget> AbilitySlotClass;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UMaterialInterface* HealthBarMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UMaterialInterface* EnergyBarMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor HealthBarColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor LowHealthColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor EnergyBarColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor LowEnergyColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* BrightTimelineIcon;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    UTexture2D* DarkTimelineIcon;

    /** Sound effects */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* LowHealthSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Audio")
    USoundBase* LowEnergySound;

private:
    /** Active ability slots */
    UPROPERTY()
    TArray<UAbilitySlotWidget*> AbilitySlots;

    /** Material instances */
    UPROPERTY()
    UMaterialInstanceDynamic* HealthBarMID;

    UPROPERTY()
    UMaterialInstanceDynamic* EnergyBarMID;

    /** Current values */
    float CurrentHealthPercent;
    float CurrentEnergyPercent;
    bool bIsLowHealth;
    bool bIsLowEnergy;

    /** Initialize UI */
    void InitializeWidgets();
    void CreateMaterialInstances();

    /** Update visuals */
    void UpdateHealthVisuals();
    void UpdateEnergyVisuals();
    void UpdateTimelineIcon();

    /** Ability slot management */
    UAbilitySlotWidget* CreateAbilitySlot();
    void UpdateAbilitySlotEnergy();

    /** Low resource warnings */
    void CheckLowHealth();
    void CheckLowEnergy();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnEnergyChanged(float CurrentEnergy, float MaxEnergy);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnLowHealth(bool bIsLow);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnLowEnergy(bool bIsLow);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnAbilitiesUpdated();
};
