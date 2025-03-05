#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETimelineTypes.h"
#include "SETimelineStateManager.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API USETimelineStateManager : public UActorComponent
{
    GENERATED_BODY()

public:
    USETimelineStateManager();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Timeline State Management
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool RequestTimelineTransition(ETimelineState NewState);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool CanTransitionTo(ETimelineState NewState) const;

    UFUNCTION(BlueprintPure, Category = "Timeline")
    ETimelineState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Timeline")
    float GetTimelineEnergy() const { return TimelineStats.Energy; }

    // Timeline Effects
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool ApplyTimelineEffect(const FTimelineEffect& Effect);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void RemoveTimelineEffect(const FString& EffectName);

    // Timeline Mastery
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void GainTimelineMastery(float Amount, ETimelineState State);

    // Timeline Energy Management
    UFUNCTION(BlueprintCallable, Category = "Timeline")
    bool ConsumeEnergy(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Timeline")
    void RestoreEnergy(float Amount);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline")
    FOnTimelineStateChanged OnTimelineStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Timeline")
    FOnTimelineEnergyChanged OnTimelineEnergyChanged;

    UPROPERTY(BlueprintAssignable, Category = "Timeline")
    FOnTimelineMasteryGained OnTimelineMasteryGained;

protected:
    // Core timeline state
    UPROPERTY(VisibleAnywhere, Category = "Timeline")
    ETimelineState CurrentState;

    UPROPERTY(EditAnywhere, Category = "Timeline")
    FTimelineStats TimelineStats;

    // Active effects
    UPROPERTY()
    TArray<FTimelineEffect> ActiveEffects;

    // Timeline transition
    UPROPERTY()
    bool bIsTransitioning;

    UPROPERTY(EditAnywhere, Category = "Timeline")
    float TransitionDuration;

    UPROPERTY()
    float CurrentTransitionTime;

    // Internal methods
    void UpdateTimelineEffects(float DeltaTime);
    void UpdateEnergyRegeneration(float DeltaTime);
    bool ValidateTimelineTransition(ETimelineState NewState) const;
    void CompleteTransition();
    void HandleTimelineEffects(ETimelineState OldState, ETimelineState NewState);

    // Cache for performance
    UPROPERTY()
    class UParticleSystemComponent* TransitionVFX;

    UPROPERTY()
    class UAudioComponent* TransitionSFX;

private:
    // Performance optimization
    TMap<FString, int32> EffectIndexCache;
    void UpdateEffectCache();

    // Error handling
    void LogTimelineError(const FString& ErrorMessage) const;
    bool ValidateTimelineState() const;
};
