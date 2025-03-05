#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/SETimelineTypes.h"
#include "SETransitionAnimationSystem.generated.h"

USTRUCT(BlueprintType)
struct FTransitionAnimationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* TransitionMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float BlendInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float BlendOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UCurveFloat* TransitionCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TArray<FName> NotifyEvents;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionAnimationEvent, const FName&, EventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTransitionAnimationProgress, float, Progress, const FString&, PhaseName);

UCLASS(BlueprintType)
class SHADOWECHOES_API USETransitionAnimationSystem : public UObject
{
    GENERATED_BODY()

public:
    USETransitionAnimationSystem();

    // Animation Control
    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void StartTransitionAnimation(ETimelineState FromState, ETimelineState ToState);

    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void UpdateTransitionAnimation(float Progress);

    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void StopTransitionAnimation();

    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void PauseTransitionAnimation();

    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void ResumeTransitionAnimation();

    // Animation Data Management
    UFUNCTION(BlueprintCallable, Category = "Timeline Animation")
    void SetTransitionAnimationData(ETimelineState FromState, ETimelineState ToState, const FTransitionAnimationData& Data);

    UFUNCTION(BlueprintPure, Category = "Timeline Animation")
    const FTransitionAnimationData* GetTransitionAnimationData(ETimelineState FromState, ETimelineState ToState) const;

    // Animation State Queries
    UFUNCTION(BlueprintPure, Category = "Timeline Animation")
    bool IsAnimating() const { return bIsAnimating; }

    UFUNCTION(BlueprintPure, Category = "Timeline Animation")
    float GetCurrentProgress() const { return CurrentProgress; }

    UFUNCTION(BlueprintPure, Category = "Timeline Animation")
    FString GetCurrentPhase() const { return CurrentPhase; }

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline Animation")
    FOnTransitionAnimationEvent OnAnimationEvent;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Animation")
    FOnTransitionAnimationProgress OnAnimationProgress;

protected:
    // Animation state
    UPROPERTY()
    bool bIsAnimating;

    UPROPERTY()
    bool bIsPaused;

    UPROPERTY()
    float CurrentProgress;

    UPROPERTY()
    FString CurrentPhase;

    UPROPERTY()
    ETimelineState CurrentFromState;

    UPROPERTY()
    ETimelineState CurrentToState;

    // Animation data storage
    UPROPERTY()
    TMap<FString, FTransitionAnimationData> AnimationDataMap;

    // Internal methods
    void ProcessAnimationPhase(float Progress);
    void TriggerAnimationEvents(float Progress);
    void UpdateCharacterAppearance(float Progress);
    void HandlePhaseTransition(const FString& NewPhase);

private:
    // Animation phases
    static const FString PhaseStart;
    static const FString PhaseMiddle;
    static const FString PhaseEnd;

    // Animation parameters
    float LastEventProgress;
    TArray<FName> TriggeredEvents;

    // Helper functions
    FString GetAnimationKey(ETimelineState FromState, ETimelineState ToState) const;
    bool ValidateAnimationData(const FTransitionAnimationData& Data) const;
    void ResetAnimationState();

    // Material transition
    void UpdateMaterialParameters(float Progress);
    void SetupMaterialTransition();
    void CleanupMaterialTransition();

    // Performance optimization
    void PreloadAnimationAssets();
    void UnloadUnusedAssets();
    
    // Cache
    UPROPERTY()
    TMap<USkeletalMeshComponent*, TArray<UMaterialInstanceDynamic*>> MaterialCache;
    void UpdateMaterialCache();

    // Debug
    void LogAnimationError(const FString& ErrorMessage) const;
    bool ValidateAnimationState() const;
};
