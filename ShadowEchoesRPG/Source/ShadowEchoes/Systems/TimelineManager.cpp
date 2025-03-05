// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Systems/TimelineManager.h"
#include "Systems/TimelineEffectComponent.h"
#include "Core/SEGameInstance.h"
#include "Kismet/GameplayStatics.h"

UTimelineManager::UTimelineManager()
    : TransitionDuration(1.0f)
    , MinTransitionEnergy(25.0f)
    , EnergyRegenRate(10.0f)
    , MasteryGainRate(0.1f)
    , CurrentTimelineState(ETimelineState::BrightWorld)
    , TimelineEnergy(100.0f)
    , MaxTimelineEnergy(100.0f)
    , BrightTimelineMastery(0.0f)
    , DarkTimelineMastery(0.0f)
    , bIsTransitioning(false)
    , TransitionProgress(0.0f)
    , TargetTimelineState(ETimelineState::BrightWorld)
{
}

void UTimelineManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    InitializeTimeline();
}

void UTimelineManager::SetTimelineState(ETimelineState NewState)
{
    if (CurrentTimelineState != NewState)
    {
        ETimelineState OldState = CurrentTimelineState;
        CurrentTimelineState = NewState;

        // Notify effects
        NotifyEffectsOfStateChange(NewState);

        // Notify game instance
        if (GameInstance)
        {
            GameInstance->OnTimelineStateChanged.Broadcast(NewState);
        }

        // Notify blueprint
        BP_OnTimelineStateChanged(NewState);
    }
}

void UTimelineManager::StartTimelineTransition(ETimelineState TargetState)
{
    if (bIsTransitioning || CurrentTimelineState == TargetState)
    {
        return;
    }

    // Check energy requirement
    if (!HasEnoughEnergy(MinTransitionEnergy))
    {
        return;
    }

    // Consume energy
    ConsumeTimelineEnergy(MinTransitionEnergy);

    // Start transition
    bIsTransitioning = true;
    TransitionProgress = 0.0f;
    TargetTimelineState = TargetState;

    // Play effects
    if (TransitionStartSound)
    {
        UGameplayStatics::PlaySound2D(this, TransitionStartSound);
    }

    // Notify events
    OnTimelineTransitionStarted.Broadcast(CurrentTimelineState, TargetState);
    BP_OnTimelineTransitionStarted(CurrentTimelineState, TargetState);
}

void UTimelineManager::CancelTimelineTransition()
{
    if (bIsTransitioning)
    {
        bIsTransitioning = false;
        TransitionProgress = 0.0f;
        UpdateTransitionEffects(0.0f);
    }
}

void UTimelineManager::RegisterTimelineEffect(UTimelineEffectComponent* Effect)
{
    if (Effect && !RegisteredEffects.Contains(Effect))
    {
        RegisteredEffects.Add(Effect);
        Effect->OnTimelineStateChanged(CurrentTimelineState);
    }
}

void UTimelineManager::UnregisterTimelineEffect(UTimelineEffectComponent* Effect)
{
    if (Effect)
    {
        RegisteredEffects.Remove(Effect);
    }
}

void UTimelineManager::ConsumeTimelineEnergy(float Amount)
{
    if (HasEnoughEnergy(Amount))
    {
        TimelineEnergy -= Amount;
        ClampTimelineEnergy();
        BP_OnTimelineEnergyChanged(TimelineEnergy, MaxTimelineEnergy);
    }
}

void UTimelineManager::RestoreTimelineEnergy(float Amount)
{
    TimelineEnergy += Amount;
    ClampTimelineEnergy();
    BP_OnTimelineEnergyChanged(TimelineEnergy, MaxTimelineEnergy);
}

void UTimelineManager::AddTimelineMastery(ETimelineState Timeline, float Amount)
{
    float* Mastery = nullptr;
    switch (Timeline)
    {
        case ETimelineState::BrightWorld:
            Mastery = &BrightTimelineMastery;
            break;
        case ETimelineState::DarkWorld:
            Mastery = &DarkTimelineMastery;
            break;
        default:
            return;
    }

    if (Mastery)
    {
        *Mastery = FMath::Clamp(*Mastery + Amount, 0.0f, 1.0f);
        BP_OnTimelineMasteryGained(Timeline, Amount);
    }
}

float UTimelineManager::GetTimelineMastery(ETimelineState Timeline) const
{
    switch (Timeline)
    {
        case ETimelineState::BrightWorld:
            return BrightTimelineMastery;
        case ETimelineState::DarkWorld:
            return DarkTimelineMastery;
        default:
            return 0.0f;
    }
}

void UTimelineManager::InitializeTimeline()
{
    // Set initial values
    TimelineEnergy = MaxTimelineEnergy;
    BrightTimelineMastery = 0.0f;
    DarkTimelineMastery = 0.0f;
    bIsTransitioning = false;
    TransitionProgress = 0.0f;

    // Notify initial state
    NotifyEffectsOfStateChange(CurrentTimelineState);
}

void UTimelineManager::UpdateTimelineEffects()
{
    if (bIsTransitioning)
    {
        // Update transition progress
        TransitionProgress += GetWorld()->GetDeltaSeconds() / TransitionDuration;
        TransitionProgress = FMath::Clamp(TransitionProgress, 0.0f, 1.0f);

        // Update effects
        UpdateTransitionEffects(TransitionProgress);

        // Check for completion
        if (TransitionProgress >= 1.0f)
        {
            CompleteTransition();
        }
    }
}

void UTimelineManager::UpdateTimelineEnergy(float DeltaTime)
{
    // Regenerate energy over time
    if (TimelineEnergy < MaxTimelineEnergy)
    {
        float RegenAmount = EnergyRegenRate * DeltaTime;

        // Apply mastery bonus
        float CurrentMastery = GetTimelineMastery(CurrentTimelineState);
        RegenAmount *= (1.0f + CurrentMastery);

        RestoreTimelineEnergy(RegenAmount);
    }
}

void UTimelineManager::CompleteTransition()
{
    bIsTransitioning = false;
    TransitionProgress = 0.0f;

    // Set new state
    SetTimelineState(TargetTimelineState);

    // Play completion sound
    if (TransitionCompleteSound)
    {
        UGameplayStatics::PlaySound2D(this, TransitionCompleteSound);
    }

    // Notify events
    OnTimelineTransitionCompleted.Broadcast(TargetTimelineState);
    BP_OnTimelineTransitionCompleted(TargetTimelineState);
}

void UTimelineManager::UpdateTransitionEffects(float Progress)
{
    // Update registered effects
    NotifyEffectsOfTransitionProgress(Progress);

    // Update post-process effect
    if (TransitionPostProcess)
    {
        // TODO: Update post-process material parameters
    }
}

bool UTimelineManager::HasEnoughEnergy(float Amount) const
{
    return TimelineEnergy >= Amount;
}

void UTimelineManager::ClampTimelineEnergy()
{
    TimelineEnergy = FMath::Clamp(TimelineEnergy, 0.0f, MaxTimelineEnergy);
}

void UTimelineManager::NotifyEffectsOfStateChange(ETimelineState NewState)
{
    for (UTimelineEffectComponent* Effect : RegisteredEffects)
    {
        if (Effect)
        {
            Effect->OnTimelineStateChanged(NewState);
        }
    }
}

void UTimelineManager::NotifyEffectsOfTransitionProgress(float Progress)
{
    for (UTimelineEffectComponent* Effect : RegisteredEffects)
    {
        if (Effect)
        {
            Effect->OnTimelineTransitionProgress(Progress);
        }
    }
}
