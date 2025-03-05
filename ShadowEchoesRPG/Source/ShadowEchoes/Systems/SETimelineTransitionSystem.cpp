#include "SETimelineTransitionSystem.h"
#include "SETransitionAnimationSystem.h"
#include "SETransitionEffectLoader.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

USETimelineTransitionSystem::USETimelineTransitionSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.016f; // ~60 FPS for smooth transitions

    // Initialize default values
    bIsTransitioning = false;
    bIsPaused = false;
    TransitionProgress = 0.0f;
    CurrentState = ETimelineState::None;
    TargetState = ETimelineState::None;
    TransitionDuration = 1.0f;
    MinTransitionDuration = 0.5f;
    MaxTransitionDuration = 2.0f;
    ElapsedTransitionTime = 0.0f;
    bTransitionEffectsInitialized = false;
}

void USETimelineTransitionSystem::BeginPlay()
{
    Super::BeginPlay();

    // Create required subsystems
    AnimationSystem = NewObject<USETransitionAnimationSystem>(this);
    EffectLoader = NewObject<USETransitionEffectLoader>(this);

    // Initialize transition effects
    InitializeTransitionEffects();
}

void USETimelineTransitionSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsTransitioning && !bIsPaused)
    {
        UpdateTransition(DeltaTime);
    }
}

bool USETimelineTransitionSystem::StartTransition(ETimelineState TargetState)
{
    if (!ValidateTransitionRequest(TargetState))
    {
        return false;
    }

    if (!CheckTransitionRequirements(TargetState))
    {
        HandleTransitionFailure(TEXT("Transition requirements not met"));
        return false;
    }

    // Store transition data
    this->TargetState = TargetState;
    ElapsedTransitionTime = 0.0f;
    TransitionProgress = 0.0f;
    bIsTransitioning = true;
    bIsPaused = false;

    // Preload assets for target state
    PreloadTransitionAssets(TargetState);

    // Initialize and play effects
    PlayTransitionEffects();

    // Notify listeners
    OnTransitionStarted.Broadcast(CurrentState, TargetState);

    return true;
}

void USETimelineTransitionSystem::CancelTransition()
{
    if (!bIsTransitioning)
    {
        return;
    }

    StopTransitionEffects();
    CleanupTransition();
    bIsTransitioning = false;
    TransitionProgress = 0.0f;

    // Notify of cancellation
    HandleTransitionFailure(TEXT("Transition cancelled"));
}

void USETimelineTransitionSystem::PauseTransition()
{
    if (bIsTransitioning)
    {
        bIsPaused = true;
        
        // Pause effects
        if (TransitionVFX)
        {
            TransitionVFX->SetPaused(true);
        }
        if (TransitionSFX)
        {
            TransitionSFX->SetPaused(true);
        }
    }
}

void USETimelineTransitionSystem::ResumeTransition()
{
    if (bIsTransitioning && bIsPaused)
    {
        bIsPaused = false;
        
        // Resume effects
        if (TransitionVFX)
        {
            TransitionVFX->SetPaused(false);
        }
        if (TransitionSFX)
        {
            TransitionSFX->SetPaused(false);
        }
    }
}

void USETimelineTransitionSystem::SetTransitionDuration(float NewDuration)
{
    TransitionDuration = FMath::Clamp(NewDuration, MinTransitionDuration, MaxTransitionDuration);
}

void USETimelineTransitionSystem::SetTransitionEffects(UParticleSystem* VFX, USoundBase* SFX)
{
    if (!bIsTransitioning)
    {
        if (VFX)
        {
            FTransitionEffectData& EffectData = EffectCache.FindOrAdd(TargetState);
            EffectData.ParticleSystem = VFX;
        }
        if (SFX)
        {
            FTransitionEffectData& EffectData = EffectCache.FindOrAdd(TargetState);
            EffectData.SoundEffect = SFX;
        }
    }
}

void USETimelineTransitionSystem::UpdateTransition(float DeltaTime)
{
    ElapsedTransitionTime += DeltaTime;
    float PreviousProgress = TransitionProgress;
    TransitionProgress = FMath::Clamp(ElapsedTransitionTime / TransitionDuration, 0.0f, 1.0f);

    // Update visual effects
    UpdateTransitionEffects(TransitionProgress);

    // Notify progress
    if (TransitionProgress != PreviousProgress)
    {
        OnTransitionProgress.Broadcast(TransitionProgress);
    }

    // Check for completion
    if (TransitionProgress >= 1.0f)
    {
        CompleteTransition();
    }
}

void USETimelineTransitionSystem::CompleteTransition()
{
    if (!bIsTransitioning)
    {
        return;
    }

    // Store states for notification
    ETimelineState FromState = CurrentState;
    ETimelineState ToState = TargetState;

    // Update state
    CurrentState = TargetState;
    
    // Cleanup
    StopTransitionEffects();
    CleanupTransition();

    // Reset flags
    bIsTransitioning = false;
    TransitionProgress = 0.0f;
    ElapsedTransitionTime = 0.0f;

    // Notify completion
    OnTransitionCompleted.Broadcast(FromState, ToState);

    // Cleanup unused assets
    UnloadUnusedAssets();
}

void USETimelineTransitionSystem::HandleTransitionFailure(const FString& Reason)
{
    LogTransitionError(Reason);
    OnTransitionFailed.Broadcast(Reason);
    CleanupTransition();
}

void USETimelineTransitionSystem::CleanupTransition()
{
    StopTransitionEffects();
    bIsTransitioning = false;
    bIsPaused = false;
    TransitionProgress = 0.0f;
    ElapsedTransitionTime = 0.0f;
}

void USETimelineTransitionSystem::InitializeTransitionEffects()
{
    if (bTransitionEffectsInitialized)
    {
        return;
    }

    // Create particle system component
    TransitionVFX = NewObject<UParticleSystemComponent>(this);
    TransitionVFX->bAutoActivate = false;
    TransitionVFX->RegisterComponent();

    // Create audio component
    TransitionSFX = NewObject<UAudioComponent>(this);
    TransitionSFX->bAutoActivate = false;
    TransitionSFX->RegisterComponent();

    bTransitionEffectsInitialized = true;
}

void USETimelineTransitionSystem::UpdateTransitionEffects(float Progress)
{
    if (!bTransitionEffectsInitialized)
    {
        return;
    }

    // Update VFX parameters
    if (TransitionVFX && TransitionVFX->IsActive())
    {
        TransitionVFX->SetFloatParameter(TEXT("TransitionProgress"), Progress);
        TransitionVFX->SetVectorParameter(TEXT("EffectColor"), GetTransitionEffectColor());
    }

    // Update animation system
    if (AnimationSystem)
    {
        AnimationSystem->UpdateTransitionAnimation(Progress);
    }
}

void USETimelineTransitionSystem::PlayTransitionEffects()
{
    if (!bTransitionEffectsInitialized)
    {
        InitializeTransitionEffects();
    }

    // Get effect data for target state
    const FTransitionEffectData* EffectData = EffectCache.Find(TargetState);
    if (!EffectData)
    {
        return;
    }

    // Setup and play VFX
    if (TransitionVFX && EffectData->ParticleSystem)
    {
        TransitionVFX->SetTemplate(EffectData->ParticleSystem);
        TransitionVFX->SetRelativeLocation(GetTransitionEffectLocation());
        TransitionVFX->SetRelativeRotation(GetTransitionEffectRotation());
        TransitionVFX->SetRelativeScale3D(FVector(CalculateEffectScale()));
        TransitionVFX->Activate(true);
    }

    // Setup and play SFX
    if (TransitionSFX && EffectData->SoundEffect)
    {
        TransitionSFX->SetSound(EffectData->SoundEffect);
        TransitionSFX->Play();
    }
}

void USETimelineTransitionSystem::StopTransitionEffects()
{
    if (TransitionVFX)
    {
        TransitionVFX->Deactivate();
    }
    if (TransitionSFX)
    {
        TransitionSFX->Stop();
    }
}

bool USETimelineTransitionSystem::ValidateTransitionRequest(ETimelineState NewState) const
{
    if (bIsTransitioning)
    {
        LogTransitionError(TEXT("Transition already in progress"));
        return false;
    }

    if (NewState == CurrentState)
    {
        LogTransitionError(TEXT("Cannot transition to current state"));
        return false;
    }

    if (NewState == ETimelineState::None)
    {
        LogTransitionError(TEXT("Cannot transition to None state"));
        return false;
    }

    return true;
}

bool USETimelineTransitionSystem::CheckTransitionRequirements(ETimelineState NewState) const
{
    // Add any gameplay-specific requirements here
    return true;
}

void USETimelineTransitionSystem::PreloadTransitionAssets(ETimelineState TargetState)
{
    if (EffectLoader)
    {
        EffectLoader->PreloadEffects(TargetState);
    }
}

void USETimelineTransitionSystem::UnloadUnusedAssets()
{
    if (EffectLoader)
    {
        EffectLoader->UnloadUnusedEffects();
    }
}

FVector USETimelineTransitionSystem::GetTransitionEffectLocation() const
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        return Character->GetMesh()->GetComponentLocation() + FVector(0, 0, 100.0f);
    }
    return FVector::ZeroVector;
}

FRotator USETimelineTransitionSystem::GetTransitionEffectRotation() const
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        return Character->GetActorRotation();
    }
    return FRotator::ZeroRotator;
}

float USETimelineTransitionSystem::CalculateEffectScale() const
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        return Character->GetMesh()->GetComponentScale().X;
    }
    return 1.0f;
}

void USETimelineTransitionSystem::LogTransitionError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Warning, TEXT("Timeline Transition Error: %s"), *ErrorMessage);
}

bool USETimelineTransitionSystem::ValidateTransitionState() const
{
    if (!bTransitionEffectsInitialized)
    {
        LogTransitionError(TEXT("Transition effects not initialized"));
        return false;
    }

    if (!TransitionVFX || !TransitionSFX)
    {
        LogTransitionError(TEXT("Missing effect components"));
        return false;
    }

    return true;
}

void USETimelineTransitionSystem::UpdateEffectCache()
{
    if (EffectLoader)
    {
        EffectLoader->UpdateEffectCache(EffectCache);
    }
}
