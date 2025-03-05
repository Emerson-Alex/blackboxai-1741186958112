#include "SETimelineStateManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"

USETimelineStateManager::USETimelineStateManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.016f; // ~60 FPS for smooth transitions

    // Initialize default values
    CurrentState = ETimelineState::None;
    bIsTransitioning = false;
    TransitionDuration = 1.0f;
    CurrentTransitionTime = 0.0f;

    // Initialize timeline stats with default values
    TimelineStats.Energy = 100.0f;
    TimelineStats.MaxEnergy = 100.0f;
    TimelineStats.EnergyRegenRate = 1.0f;
    TimelineStats.TransitionEnergyCost = 25.0f;
}

void USETimelineStateManager::BeginPlay()
{
    Super::BeginPlay();

    // Validate initial state
    if (!ValidateTimelineState())
    {
        LogTimelineError(TEXT("Invalid initial timeline state configuration"));
        return;
    }

    // Initialize effect cache for performance
    UpdateEffectCache();
}

void USETimelineStateManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Handle timeline transition
    if (bIsTransitioning)
    {
        CurrentTransitionTime += DeltaTime;
        if (CurrentTransitionTime >= TransitionDuration)
        {
            CompleteTransition();
        }
    }

    // Update active effects
    UpdateTimelineEffects(DeltaTime);

    // Handle energy regeneration
    UpdateEnergyRegeneration(DeltaTime);
}

bool USETimelineStateManager::RequestTimelineTransition(ETimelineState NewState)
{
    // Validate transition request
    if (!ValidateTimelineTransition(NewState))
    {
        return false;
    }

    // Check if we have enough energy
    if (!ConsumeEnergy(TimelineStats.TransitionEnergyCost))
    {
        LogTimelineError(TEXT("Insufficient energy for timeline transition"));
        return false;
    }

    // Store old state for effects
    ETimelineState OldState = CurrentState;

    // Begin transition
    bIsTransitioning = true;
    CurrentTransitionTime = 0.0f;
    
    // Handle visual and audio effects
    if (TransitionVFX)
    {
        TransitionVFX->Activate(true);
    }
    if (TransitionSFX)
    {
        TransitionSFX->Play();
    }

    // Handle timeline-specific effects
    HandleTimelineEffects(OldState, NewState);

    // Broadcast state change
    OnTimelineStateChanged.Broadcast(NewState, OldState);

    return true;
}

bool USETimelineStateManager::CanTransitionTo(ETimelineState NewState) const
{
    // Can't transition to same state
    if (NewState == CurrentState)
    {
        return false;
    }

    // Can't transition while already transitioning
    if (bIsTransitioning)
    {
        return false;
    }

    // Check energy requirements
    if (TimelineStats.Energy < TimelineStats.TransitionEnergyCost)
    {
        return false;
    }

    return true;
}

bool USETimelineStateManager::ApplyTimelineEffect(const FTimelineEffect& Effect)
{
    // Validate effect
    if (Effect.Duration <= 0.0f || Effect.Power <= 0.0f)
    {
        LogTimelineError(TEXT("Invalid timeline effect parameters"));
        return false;
    }

    // Check if effect is compatible with current state
    if (Effect.RequiredState != ETimelineState::None && Effect.RequiredState != CurrentState)
    {
        return false;
    }

    // Add to active effects
    ActiveEffects.Add(Effect);
    UpdateEffectCache();

    // Apply visual effects if provided
    if (Effect.VisualEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(
            Effect.VisualEffect,
            GetOwner()->GetRootComponent()
        );
    }

    // Play sound if provided
    if (Effect.SoundEffect)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), Effect.SoundEffect);
    }

    return true;
}

void USETimelineStateManager::RemoveTimelineEffect(const FString& EffectName)
{
    // Use cached index for faster removal
    const int32* IndexPtr = EffectIndexCache.Find(EffectName);
    if (IndexPtr)
    {
        ActiveEffects.RemoveAt(*IndexPtr);
        UpdateEffectCache();
    }
}

void USETimelineStateManager::GainTimelineMastery(float Amount, ETimelineState State)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    switch (State)
    {
        case ETimelineState::Light:
            TimelineStats.LightMastery = FMath::Min(TimelineStats.LightMastery + Amount, 100.0f);
            break;
        case ETimelineState::Dark:
            TimelineStats.DarkMastery = FMath::Min(TimelineStats.DarkMastery + Amount, 100.0f);
            break;
        default:
            return;
    }

    OnTimelineMasteryGained.Broadcast(Amount);
}

bool USETimelineStateManager::ConsumeEnergy(float Amount)
{
    if (Amount <= 0.0f || Amount > TimelineStats.Energy)
    {
        return false;
    }

    TimelineStats.Energy -= Amount;
    OnTimelineEnergyChanged.Broadcast(TimelineStats.Energy);
    return true;
}

void USETimelineStateManager::RestoreEnergy(float Amount)
{
    if (Amount <= 0.0f)
    {
        return;
    }

    TimelineStats.Energy = FMath::Min(TimelineStats.Energy + Amount, TimelineStats.MaxEnergy);
    OnTimelineEnergyChanged.Broadcast(TimelineStats.Energy);
}

void USETimelineStateManager::UpdateTimelineEffects(float DeltaTime)
{
    // Use index-based for loop for safe removal during iteration
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
    {
        FTimelineEffect& Effect = ActiveEffects[i];
        Effect.Duration -= DeltaTime;

        if (Effect.Duration <= 0.0f)
        {
            ActiveEffects.RemoveAt(i);
            UpdateEffectCache();
        }
    }
}

void USETimelineStateManager::UpdateEnergyRegeneration(float DeltaTime)
{
    if (TimelineStats.Energy < TimelineStats.MaxEnergy)
    {
        float RegenAmount = TimelineStats.EnergyRegenRate * DeltaTime;
        RestoreEnergy(RegenAmount);
    }
}

bool USETimelineStateManager::ValidateTimelineTransition(ETimelineState NewState) const
{
    // Basic validation
    if (NewState == ETimelineState::None || NewState == CurrentState)
    {
        return false;
    }

    // Check if already transitioning
    if (bIsTransitioning)
    {
        return false;
    }

    // Validate energy requirements
    if (TimelineStats.Energy < TimelineStats.TransitionEnergyCost)
    {
        return false;
    }

    return true;
}

void USETimelineStateManager::CompleteTransition()
{
    bIsTransitioning = false;
    CurrentTransitionTime = 0.0f;

    // Clean up effects
    if (TransitionVFX)
    {
        TransitionVFX->Deactivate();
    }
    if (TransitionSFX)
    {
        TransitionSFX->Stop();
    }
}

void USETimelineStateManager::HandleTimelineEffects(ETimelineState OldState, ETimelineState NewState)
{
    // Remove incompatible effects
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
    {
        const FTimelineEffect& Effect = ActiveEffects[i];
        if (Effect.RequiredState != ETimelineState::None && Effect.RequiredState != NewState)
        {
            ActiveEffects.RemoveAt(i);
        }
    }
    UpdateEffectCache();
}

void USETimelineStateManager::UpdateEffectCache()
{
    EffectIndexCache.Empty(ActiveEffects.Num());
    for (int32 i = 0; i < ActiveEffects.Num(); ++i)
    {
        EffectIndexCache.Add(ActiveEffects[i].EffectName, i);
    }
}

void USETimelineStateManager::LogTimelineError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Error, TEXT("Timeline Error: %s"), *ErrorMessage);
}

bool USETimelineStateManager::ValidateTimelineState() const
{
    // Validate timeline stats
    if (TimelineStats.MaxEnergy <= 0.0f ||
        TimelineStats.EnergyRegenRate <= 0.0f ||
        TimelineStats.TransitionEnergyCost <= 0.0f)
    {
        return false;
    }

    // Validate transition settings
    if (TransitionDuration <= 0.0f)
    {
        return false;
    }

    return true;
}
