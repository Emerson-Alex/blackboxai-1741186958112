#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETimelineTypes.h"
#include "SETimelineTransitionSystem.generated.h"

class USETransitionAnimationSystem;
class USETransitionEffectLoader;
class UParticleSystemComponent;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransitionStarted, ETimelineState, FromState, ETimelineState, ToState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransitionCompleted, ETimelineState, FromState, ETimelineState, ToState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionFailed, const FString&, Reason);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API USETimelineTransitionSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    USETimelineTransitionSystem();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Transition Control
    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    bool StartTransition(ETimelineState TargetState);

    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    void CancelTransition();

    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    void PauseTransition();

    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    void ResumeTransition();

    // State Queries
    UFUNCTION(BlueprintPure, Category = "Timeline Transition")
    bool IsTransitioning() const { return bIsTransitioning; }

    UFUNCTION(BlueprintPure, Category = "Timeline Transition")
    float GetTransitionProgress() const { return TransitionProgress; }

    UFUNCTION(BlueprintPure, Category = "Timeline Transition")
    ETimelineState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Timeline Transition")
    ETimelineState GetTargetState() const { return TargetState; }

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    void SetTransitionDuration(float NewDuration);

    UFUNCTION(BlueprintCallable, Category = "Timeline Transition")
    void SetTransitionEffects(UParticleSystem* VFX, USoundBase* SFX);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline Transition")
    FOnTransitionStarted OnTransitionStarted;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Transition")
    FOnTransitionCompleted OnTransitionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Transition")
    FOnTransitionProgress OnTransitionProgress;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Transition")
    FOnTransitionFailed OnTransitionFailed;

protected:
    // Core transition state
    UPROPERTY(VisibleAnywhere, Category = "Timeline Transition")
    bool bIsTransitioning;

    UPROPERTY(VisibleAnywhere, Category = "Timeline Transition")
    bool bIsPaused;

    UPROPERTY(VisibleAnywhere, Category = "Timeline Transition")
    float TransitionProgress;

    UPROPERTY(VisibleAnywhere, Category = "Timeline Transition")
    ETimelineState CurrentState;

    UPROPERTY(VisibleAnywhere, Category = "Timeline Transition")
    ETimelineState TargetState;

    // Configuration
    UPROPERTY(EditAnywhere, Category = "Timeline Transition")
    float TransitionDuration;

    UPROPERTY(EditAnywhere, Category = "Timeline Transition")
    float MinTransitionDuration;

    UPROPERTY(EditAnywhere, Category = "Timeline Transition")
    float MaxTransitionDuration;

    // Components
    UPROPERTY()
    USETransitionAnimationSystem* AnimationSystem;

    UPROPERTY()
    USETransitionEffectLoader* EffectLoader;

    UPROPERTY()
    UParticleSystemComponent* TransitionVFX;

    UPROPERTY()
    UAudioComponent* TransitionSFX;

    // Internal methods
    void UpdateTransition(float DeltaTime);
    void CompleteTransition();
    void HandleTransitionFailure(const FString& Reason);
    void CleanupTransition();

    // Effect management
    void InitializeTransitionEffects();
    void UpdateTransitionEffects(float Progress);
    void PlayTransitionEffects();
    void StopTransitionEffects();

    // Validation
    bool ValidateTransitionRequest(ETimelineState NewState) const;
    bool CheckTransitionRequirements(ETimelineState NewState) const;

private:
    // State tracking
    float ElapsedTransitionTime;
    bool bTransitionEffectsInitialized;

    // Performance optimization
    void PreloadTransitionAssets(ETimelineState TargetState);
    void UnloadUnusedAssets();

    // Effect parameters
    FVector GetTransitionEffectLocation() const;
    FRotator GetTransitionEffectRotation() const;
    float CalculateEffectScale() const;

    // Error handling
    void LogTransitionError(const FString& ErrorMessage) const;
    bool ValidateTransitionState() const;

    // Cache
    TMap<ETimelineState, FTransitionEffectData> EffectCache;
    void UpdateEffectCache();
};
