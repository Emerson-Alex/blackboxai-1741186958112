// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SETypes.h"
#include "TimelineEffectComponent.generated.h"

class UTimelineManager;
class UMaterialInstanceDynamic;
class UParticleSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineEffectStateChanged, ETimelineState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimelineEffectTransitionProgress, float, Progress);

/**
 * Component for handling timeline-specific visual and gameplay effects
 */
UCLASS(ClassGroup=(ShadowEchoes), meta=(BlueprintSpawnableComponent))
class SHADOWECHOES_API UTimelineEffectComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTimelineEffectComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /** Timeline state */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void OnTimelineTransitionProgress(float Progress);

    /** Effect settings */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Timeline")
    void SetEffectEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Timeline")
    bool IsEffectEnabled() const { return bIsEnabled; }

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Timeline|Events")
    FOnTimelineEffectStateChanged OnTimelineEffectStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Timeline|Events")
    FOnTimelineEffectTransitionProgress OnTimelineEffectTransitionProgress;

protected:
    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UParticleSystem* BrightTimelineVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UParticleSystem* DarkTimelineVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UParticleSystem* TransitionVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UMaterialInterface* BrightTimelineMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UMaterialInterface* DarkTimelineMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|VFX")
    UMaterialInterface* TransitionMaterial;

    /** Sound settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Audio")
    USoundBase* BrightTimelineAmbient;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Audio")
    USoundBase* DarkTimelineAmbient;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Audio")
    USoundBase* TransitionSound;

    /** Effect settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Settings")
    bool bAutoRegister;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Settings")
    bool bAffectMaterials;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Settings")
    bool bPlayVFX;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Timeline|Settings")
    bool bPlaySounds;

private:
    /** Current state */
    UPROPERTY()
    ETimelineState CurrentTimelineState;

    UPROPERTY()
    bool bIsEnabled;

    /** Effect components */
    UPROPERTY()
    UParticleSystemComponent* ActiveVFX;

    UPROPERTY()
    UAudioComponent* AmbientSound;

    /** Material instances */
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> MaterialInstances;

    /** Initialize effects */
    void InitializeEffects();
    void CreateMaterialInstances();

    /** Update effects */
    void UpdateMaterials(ETimelineState State);
    void UpdateVFX(ETimelineState State);
    void UpdateAmbientSound(ETimelineState State);

    /** Transition effects */
    void UpdateTransitionEffects(float Progress);
    void PlayTransitionVFX();
    void PlayTransitionSound();

    /** Material management */
    void CollectMaterialInstances();
    void ApplyMaterialToInstances(UMaterialInterface* Material);
    void UpdateMaterialTransition(float Progress);

    /** Cleanup */
    void CleanupEffects();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnTimelineTransitionProgress(float Progress);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Timeline|Events")
    void BP_OnEffectEnabled(bool bEnabled);
};
