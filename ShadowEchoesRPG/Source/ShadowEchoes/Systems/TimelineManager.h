// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/SETypes.h"
#include "TimelineManager.generated.h"

class UTimelineEffectComponent;
class USEGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimelineTransitionStarted, ETimelineState, FromState, ETimelineState, ToState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineTransitionCompleted, ETimelineState, NewState);

/**
 * Manages timeline state transitions and effects
 */
UCLASS()
class SHADOWECHOES_API UTimelineManager : public UObject
{
    GENERATED_BODY()

public:
    UTimelineManager();

    /** Initialize the timeline system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Timeline state management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void SetTimelineState(ETimelineState NewState);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    ETimelineState GetCurrentTimelineState() const { return CurrentTimelineState; }

    /** Timeline transitions */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void StartTimelineTransition(ETimelineState TargetState);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void CancelTimelineTransition();

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    bool IsTransitioning() const { return bIsTransitioning; }

    /** Timeline effects */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void RegisterTimelineEffect(UTimelineEffectComponent* Effect);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void UnregisterTimelineEffect(UTimelineEffectComponent* Effect);

    /** Timeline energy */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void ConsumeTimelineEnergy(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void RestoreTimelineEnergy(float Amount);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    float GetTimelineEnergy() const { return TimelineEnergy; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    float GetMaxTimelineEnergy() const { return MaxTimelineEnergy; }

    /** Timeline mastery */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void AddTimelineMastery(ETimelineState Timeline, float Amount);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    float GetTimelineMastery(ETimelineState Timeline) const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Timeline|Events")
    FOnTimelineTransitionStarted OnTimelineTransitionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Timeline|Events")
    FOnTimelineTransitionCompleted OnTimelineTransitionCompleted;

protected:
    /** Timeline settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline")
    float TransitionDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline")
    float MinTransitionEnergy;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline")
    float EnergyRegenRate;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline")
    float MasteryGainRate;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UParticleSystem* TransitionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UMaterialInterface* TransitionPostProcess;

    /** Sound settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Audio")
    USoundBase* TransitionStartSound;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Audio")
    USoundBase* TransitionCompleteSound;

private:
    /** Current state */
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    UPROPERTY()
    float TimelineEnergy;

    UPROPERTY()
    float MaxTimelineEnergy;

    UPROPERTY()
    float BrightTimelineMastery;

    UPROPERTY()
    float DarkTimelineMastery;

    /** Transition state */
    bool bIsTransitioning;
    float TransitionProgress;
    ETimelineState TargetTimelineState;

    /** Effect components */
    UPROPERTY()
    TArray<UTimelineEffectComponent*> RegisteredEffects;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    /** Initialize systems */
    void InitializeTimeline();

    /** Update timeline */
    void UpdateTimelineEffects();
    void UpdateTimelineEnergy(float DeltaTime);

    /** Transition helpers */
    void CompleteTransition();
    void UpdateTransitionEffects(float Progress);

    /** Energy helpers */
    bool HasEnoughEnergy(float Amount) const;
    void ClampTimelineEnergy();

    /** Effect management */
    void NotifyEffectsOfStateChange(ETimelineState NewState);
    void NotifyEffectsOfTransitionProgress(float Progress);

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineTransitionStarted(ETimelineState FromState, ETimelineState ToState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineTransitionCompleted(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineEnergyChanged(float NewEnergy, float MaxEnergy);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineMasteryGained(ETimelineState Timeline, float Amount);
};
