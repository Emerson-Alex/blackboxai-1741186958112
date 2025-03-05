#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETimelineTypes.h"
#include "SETimelinePostProcessComponent.generated.h"

class UMaterialParameterCollection;
class UMaterialInstanceDynamic;
class UPostProcessComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostProcessEffectComplete, ETimelineState, NewState);

/**
 * Component responsible for managing post-process effects during timeline transitions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API USETimelinePostProcessComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USETimelinePostProcessComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Start transition post-process effects */
    UFUNCTION(BlueprintCallable, Category = "Timeline|PostProcess")
    void StartTransitionEffect(ETimelineState FromState, ETimelineState ToState, float Duration);

    /** Stop current transition effects */
    UFUNCTION(BlueprintCallable, Category = "Timeline|PostProcess")
    void StopTransitionEffect();

    /** Update effect parameters */
    UFUNCTION(BlueprintCallable, Category = "Timeline|PostProcess")
    void UpdateEffectParameters(float Progress);

    /** Set effect quality level */
    UFUNCTION(BlueprintCallable, Category = "Timeline|PostProcess")
    void SetEffectQuality(int32 QualityLevel);

protected:
    /** Post-process component reference */
    UPROPERTY()
    UPostProcessComponent* PostProcessComponent;

    /** Material parameter collection */
    UPROPERTY()
    UMaterialParameterCollection* TransitionParameters;

    /** Dynamic material instance for post-process effect */
    UPROPERTY()
    UMaterialInstanceDynamic* PostProcessMaterial;

    /** Current transition states */
    UPROPERTY()
    ETimelineState CurrentFromState;

    UPROPERTY()
    ETimelineState CurrentToState;

    /** Transition timing */
    float TransitionDuration;
    float CurrentTime;
    bool bIsTransitioning;

    /** Effect settings */
    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Effects")
    float DistortionStrength;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Effects")
    float EmissiveIntensity;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Effects")
    float NoiseScale;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Effects")
    float EdgeSharpness;

    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Effects")
    float TimeScale;

    /** Quality settings */
    UPROPERTY(EditDefaultsOnly, Category = "Timeline|Quality")
    TArray<FVector2D> QualityPresets;

    /** Event delegates */
    UPROPERTY(BlueprintAssignable, Category = "Timeline|Events")
    FOnPostProcessEffectComplete OnEffectComplete;

private:
    /** Initialize component */
    void InitializePostProcess();

    /** Setup material parameters */
    void SetupMaterialParameters();

    /** Update material parameters */
    void UpdateMaterialParameters(float Progress);

    /** Apply quality settings */
    void ApplyQualitySettings(int32 Level);

    /** Get state-specific color */
    FLinearColor GetStateColor(ETimelineState State) const;

    /** Handle effect completion */
    void HandleEffectComplete();

    /** Cleanup resources */
    void CleanupEffect();

    /** Performance monitoring */
    void MonitorPerformance();

    /** Debug helpers */
    void LogEffectStatus() const;
    void ValidateEffectSettings() const;

    /** Constants */
    static const FName TransitionProgressParam;
    static const FName DistortionStrengthParam;
    static const FName EmissiveIntensityParam;
    static const FName NoiseScaleParam;
    static const FName EdgeSharpnessParam;
    static const FName TimeScaleParam;
    static const FName LightColorParam;
    static const FName DarkColorParam;

    /** Cache for performance optimization */
    UPROPERTY()
    TMap<ETimelineState, FLinearColor> ColorCache;

    /** Performance tracking */
    float LastFrameTime;
    int32 FrameCounter;
    float AverageFrameTime;

    /** Effect quality state */
    int32 CurrentQualityLevel;
    bool bDynamicQualityEnabled;

    /** Initialize color cache */
    void InitializeColorCache();

    /** Update performance metrics */
    void UpdatePerformanceMetrics(float DeltaTime);

    /** Dynamic quality adjustment */
    void AdjustQualityDynamic();

    /** Resource management */
    void PreloadResources();
    void ReleaseResources();
    void ValidateResources() const;

    /** Effect parameter interpolation */
    float InterpolateParameter(float Start, float End, float Progress) const;
    FLinearColor InterpolateColor(const FLinearColor& Start, const FLinearColor& End, float Progress) const;
};
