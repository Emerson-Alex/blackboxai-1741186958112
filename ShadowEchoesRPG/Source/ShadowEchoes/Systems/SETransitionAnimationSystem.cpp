#include "SETransitionAnimationSystem.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Materials/MaterialInstanceDynamic.h"

const FString USETransitionAnimationSystem::PhaseStart = TEXT("Start");
const FString USETransitionAnimationSystem::PhaseMiddle = TEXT("Middle");
const FString USETransitionAnimationSystem::PhaseEnd = TEXT("End");

USETransitionAnimationSystem::USETransitionAnimationSystem()
{
    bIsAnimating = false;
    bIsPaused = false;
    CurrentProgress = 0.0f;
    CurrentPhase = PhaseStart;
    LastEventProgress = 0.0f;
    CurrentFromState = ETimelineState::None;
    CurrentToState = ETimelineState::None;
}

void USETransitionAnimationSystem::StartTransitionAnimation(ETimelineState FromState, ETimelineState ToState)
{
    if (bIsAnimating)
    {
        LogAnimationError(TEXT("Animation already in progress"));
        return;
    }

    // Store states
    CurrentFromState = FromState;
    CurrentToState = ToState;

    // Get animation data
    FString Key = GetAnimationKey(FromState, ToState);
    const FTransitionAnimationData* AnimData = AnimationDataMap.Find(Key);
    if (!AnimData || !ValidateAnimationData(*AnimData))
    {
        LogAnimationError(TEXT("Invalid animation data"));
        return;
    }

    // Initialize animation state
    bIsAnimating = true;
    bIsPaused = false;
    CurrentProgress = 0.0f;
    CurrentPhase = PhaseStart;
    LastEventProgress = 0.0f;
    TriggeredEvents.Empty();

    // Setup material transition
    SetupMaterialTransition();

    // Play montage if available
    if (AnimData->TransitionMontage)
    {
        if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
        {
            Character->PlayAnimMontage(AnimData->TransitionMontage, 1.0f);
        }
    }

    // Preload necessary assets
    PreloadAnimationAssets();
}

void USETransitionAnimationSystem::UpdateTransitionAnimation(float Progress)
{
    if (!bIsAnimating || bIsPaused)
    {
        return;
    }

    // Store previous state for comparison
    float PreviousProgress = CurrentProgress;
    FString PreviousPhase = CurrentPhase;

    // Update progress
    CurrentProgress = FMath::Clamp(Progress, 0.0f, 1.0f);

    // Process animation phases
    ProcessAnimationPhase(CurrentProgress);

    // Update visual elements
    UpdateCharacterAppearance(CurrentProgress);

    // Trigger events
    TriggerAnimationEvents(CurrentProgress);

    // Notify progress if changed
    if (CurrentProgress != PreviousProgress)
    {
        OnAnimationProgress.Broadcast(CurrentProgress, CurrentPhase);
    }

    // Handle phase transitions
    if (CurrentPhase != PreviousPhase)
    {
        HandlePhaseTransition(CurrentPhase);
    }

    // Check for completion
    if (CurrentProgress >= 1.0f)
    {
        StopTransitionAnimation();
    }
}

void USETransitionAnimationSystem::StopTransitionAnimation()
{
    if (!bIsAnimating)
    {
        return;
    }

    // Stop montage if playing
    if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
    {
        Character->StopAnimMontage();
    }

    // Cleanup
    CleanupMaterialTransition();
    ResetAnimationState();
    UnloadUnusedAssets();
}

void USETransitionAnimationSystem::PauseTransitionAnimation()
{
    if (bIsAnimating && !bIsPaused)
    {
        bIsPaused = true;

        // Pause montage if playing
        if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
        {
            Character->GetMesh()->PauseAnimation();
        }
    }
}

void USETransitionAnimationSystem::ResumeTransitionAnimation()
{
    if (bIsAnimating && bIsPaused)
    {
        bIsPaused = false;

        // Resume montage if paused
        if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
        {
            Character->GetMesh()->ResumeAnimation();
        }
    }
}

void USETransitionAnimationSystem::SetTransitionAnimationData(ETimelineState FromState, ETimelineState ToState, const FTransitionAnimationData& Data)
{
    if (!ValidateAnimationData(Data))
    {
        LogAnimationError(TEXT("Invalid animation data provided"));
        return;
    }

    FString Key = GetAnimationKey(FromState, ToState);
    AnimationDataMap.Add(Key, Data);
}

const FTransitionAnimationData* USETransitionAnimationSystem::GetTransitionAnimationData(ETimelineState FromState, ETimelineState ToState) const
{
    return AnimationDataMap.Find(GetAnimationKey(FromState, ToState));
}

void USETransitionAnimationSystem::ProcessAnimationPhase(float Progress)
{
    // Define phase thresholds
    const float StartThreshold = 0.25f;
    const float MiddleThreshold = 0.75f;

    // Determine current phase
    if (Progress < StartThreshold)
    {
        CurrentPhase = PhaseStart;
    }
    else if (Progress < MiddleThreshold)
    {
        CurrentPhase = PhaseMiddle;
    }
    else
    {
        CurrentPhase = PhaseEnd;
    }
}

void USETransitionAnimationSystem::TriggerAnimationEvents(float Progress)
{
    // Get animation data
    FString Key = GetAnimationKey(CurrentFromState, CurrentToState);
    const FTransitionAnimationData* AnimData = AnimationDataMap.Find(Key);
    if (!AnimData)
    {
        return;
    }

    // Check for events to trigger
    for (const FName& EventName : AnimData->NotifyEvents)
    {
        if (!TriggeredEvents.Contains(EventName))
        {
            // Trigger event at appropriate progress points
            bool bShouldTrigger = false;
            if (EventName.ToString().Contains(TEXT("Start")) && Progress >= 0.1f)
            {
                bShouldTrigger = true;
            }
            else if (EventName.ToString().Contains(TEXT("Middle")) && Progress >= 0.5f)
            {
                bShouldTrigger = true;
            }
            else if (EventName.ToString().Contains(TEXT("End")) && Progress >= 0.9f)
            {
                bShouldTrigger = true;
            }

            if (bShouldTrigger)
            {
                OnAnimationEvent.Broadcast(EventName);
                TriggeredEvents.Add(EventName);
            }
        }
    }

    LastEventProgress = Progress;
}

void USETransitionAnimationSystem::UpdateCharacterAppearance(float Progress)
{
    // Update material parameters
    UpdateMaterialParameters(Progress);

    // Update mesh effects
    if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
    {
        USkeletalMeshComponent* Mesh = Character->GetMesh();
        if (Mesh)
        {
            // Update timeline-specific parameters
            for (UMaterialInstanceDynamic* Material : MaterialCache.FindOrAdd(Mesh))
            {
                Material->SetScalarParameterValue(TEXT("TransitionProgress"), Progress);
                Material->SetScalarParameterValue(TEXT("TimelineBlend"), Progress);
            }
        }
    }
}

void USETransitionAnimationSystem::HandlePhaseTransition(const FString& NewPhase)
{
    // Trigger phase-specific events
    if (NewPhase == PhaseStart)
    {
        OnAnimationEvent.Broadcast(FName(TEXT("PhaseStart")));
    }
    else if (NewPhase == PhaseMiddle)
    {
        OnAnimationEvent.Broadcast(FName(TEXT("PhaseMiddle")));
    }
    else if (NewPhase == PhaseEnd)
    {
        OnAnimationEvent.Broadcast(FName(TEXT("PhaseEnd")));
    }
}

FString USETransitionAnimationSystem::GetAnimationKey(ETimelineState FromState, ETimelineState ToState) const
{
    return FString::Printf(TEXT("%d_%d"), static_cast<int32>(FromState), static_cast<int32>(ToState));
}

bool USETransitionAnimationSystem::ValidateAnimationData(const FTransitionAnimationData& Data) const
{
    return Data.BlendInTime >= 0.0f && Data.BlendOutTime >= 0.0f;
}

void USETransitionAnimationSystem::ResetAnimationState()
{
    bIsAnimating = false;
    bIsPaused = false;
    CurrentProgress = 0.0f;
    CurrentPhase = PhaseStart;
    LastEventProgress = 0.0f;
    TriggeredEvents.Empty();
    CurrentFromState = ETimelineState::None;
    CurrentToState = ETimelineState::None;
}

void USETransitionAnimationSystem::UpdateMaterialParameters(float Progress)
{
    if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
    {
        USkeletalMeshComponent* Mesh = Character->GetMesh();
        if (!Mesh)
        {
            return;
        }

        TArray<UMaterialInstanceDynamic*>& Materials = MaterialCache.FindOrAdd(Mesh);
        for (UMaterialInstanceDynamic* Material : Materials)
        {
            // Update transition parameters
            Material->SetScalarParameterValue(TEXT("TransitionProgress"), Progress);
            
            // Update timeline-specific parameters
            FLinearColor FromColor = GetStateColor(CurrentFromState);
            FLinearColor ToColor = GetStateColor(CurrentToState);
            FLinearColor BlendedColor = FMath::Lerp(FromColor, ToColor, Progress);
            
            Material->SetVectorParameterValue(TEXT("TimelineColor"), BlendedColor);
            Material->SetScalarParameterValue(TEXT("TimelineIntensity"), 1.0f + Progress);
        }
    }
}

void USETransitionAnimationSystem::SetupMaterialTransition()
{
    if (ACharacter* Character = Cast<ACharacter>(GetOuter()))
    {
        USkeletalMeshComponent* Mesh = Character->GetMesh();
        if (!Mesh)
        {
            return;
        }

        // Create dynamic materials if needed
        TArray<UMaterialInstanceDynamic*>& Materials = MaterialCache.FindOrAdd(Mesh);
        Materials.Empty();

        for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
        {
            if (UMaterialInterface* Material = Mesh->GetMaterial(i))
            {
                UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
                if (DynMaterial)
                {
                    Mesh->SetMaterial(i, DynMaterial);
                    Materials.Add(DynMaterial);
                }
            }
        }
    }
}

void USETransitionAnimationSystem::CleanupMaterialTransition()
{
    MaterialCache.Empty();
}

void USETransitionAnimationSystem::PreloadAnimationAssets()
{
    FString Key = GetAnimationKey(CurrentFromState, CurrentToState);
    const FTransitionAnimationData* AnimData = AnimationDataMap.Find(Key);
    if (AnimData)
    {
        // Ensure montage is loaded
        if (AnimData->TransitionMontage)
        {
            AnimData->TransitionMontage->PreloadAllFrames();
        }

        // Ensure curve is loaded
        if (AnimData->TransitionCurve)
        {
            AnimData->TransitionCurve->GetCurveData();
        }
    }
}

void USETransitionAnimationSystem::UnloadUnusedAssets()
{
    // Clear unused entries from material cache
    for (auto It = MaterialCache.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
}

void USETransitionAnimationSystem::UpdateMaterialCache()
{
    // Remove invalid entries
    for (auto It = MaterialCache.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
        {
            It.RemoveCurrent();
        }
    }
}

void USETransitionAnimationSystem::LogAnimationError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Warning, TEXT("Timeline Animation Error: %s"), *ErrorMessage);
}

bool USETransitionAnimationSystem::ValidateAnimationState() const
{
    if (!GetOuter())
    {
        LogAnimationError(TEXT("Invalid outer object"));
        return false;
    }

    if (!Cast<ACharacter>(GetOuter()))
    {
        LogAnimationError(TEXT("Outer object is not a character"));
        return false;
    }

    return true;
}

FLinearColor USETransitionAnimationSystem::GetStateColor(ETimelineState State) const
{
    switch (State)
    {
        case ETimelineState::Light:
            return FLinearColor(1.0f, 0.9f, 0.7f); // Warm light color
        case ETimelineState::Dark:
            return FLinearColor(0.2f, 0.0f, 0.4f); // Deep purple
        default:
            return FLinearColor::White;
    }
}
