// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Systems/TimelineEffectComponent.h"
#include "Systems/TimelineManager.h"
#include "Core/SEGameInstance.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UTimelineEffectComponent::UTimelineEffectComponent()
    : CurrentTimelineState(ETimelineState::BrightWorld)
    , bIsEnabled(true)
    , bAutoRegister(true)
    , bAffectMaterials(true)
    , bPlayVFX(true)
    , bPlaySounds(true)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTimelineEffectComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize effects
    InitializeEffects();

    // Auto-register with timeline manager
    if (bAutoRegister)
    {
        if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetWorld()->GetGameInstance()))
        {
            if (UTimelineManager* TimelineManager = GameInstance->GetTimelineManager())
            {
                TimelineManager->RegisterTimelineEffect(this);
            }
        }
    }
}

void UTimelineEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unregister from timeline manager
    if (USEGameInstance* GameInstance = Cast<USEGameInstance>(GetWorld()->GetGameInstance()))
    {
        if (UTimelineManager* TimelineManager = GameInstance->GetTimelineManager())
        {
            TimelineManager->UnregisterTimelineEffect(this);
        }
    }

    // Cleanup effects
    CleanupEffects();

    Super::EndPlay(EndPlayReason);
}

void UTimelineEffectComponent::OnTimelineStateChanged(ETimelineState NewState)
{
    if (!bIsEnabled || CurrentTimelineState == NewState)
    {
        return;
    }

    CurrentTimelineState = NewState;

    // Update effects
    UpdateMaterials(NewState);
    UpdateVFX(NewState);
    UpdateAmbientSound(NewState);

    // Notify events
    OnTimelineEffectStateChanged.Broadcast(NewState);
    BP_OnTimelineStateChanged(NewState);
}

void UTimelineEffectComponent::OnTimelineTransitionProgress(float Progress)
{
    if (!bIsEnabled)
    {
        return;
    }

    // Update transition effects
    UpdateTransitionEffects(Progress);

    // Notify events
    OnTimelineEffectTransitionProgress.Broadcast(Progress);
    BP_OnTimelineTransitionProgress(Progress);
}

void UTimelineEffectComponent::SetEffectEnabled(bool bEnabled)
{
    if (bIsEnabled != bEnabled)
    {
        bIsEnabled = bEnabled;

        if (bEnabled)
        {
            // Re-initialize effects
            InitializeEffects();
        }
        else
        {
            // Cleanup effects
            CleanupEffects();
        }

        // Notify blueprint
        BP_OnEffectEnabled(bEnabled);
    }
}

void UTimelineEffectComponent::InitializeEffects()
{
    if (!bIsEnabled)
    {
        return;
    }

    // Create material instances
    if (bAffectMaterials)
    {
        CreateMaterialInstances();
    }

    // Setup initial state
    UpdateMaterials(CurrentTimelineState);
    UpdateVFX(CurrentTimelineState);
    UpdateAmbientSound(CurrentTimelineState);
}

void UTimelineEffectComponent::CreateMaterialInstances()
{
    // Collect material instances
    CollectMaterialInstances();

    // Create dynamic instances for transition material
    if (TransitionMaterial)
    {
        for (UMaterialInstanceDynamic* Instance : MaterialInstances)
        {
            Instance->SetScalarParameterValue(TEXT("TransitionProgress"), 0.0f);
        }
    }
}

void UTimelineEffectComponent::UpdateMaterials(ETimelineState State)
{
    if (!bAffectMaterials)
    {
        return;
    }

    // Get appropriate material
    UMaterialInterface* Material = nullptr;
    switch (State)
    {
        case ETimelineState::BrightWorld:
            Material = BrightTimelineMaterial;
            break;
        case ETimelineState::DarkWorld:
            Material = DarkTimelineMaterial;
            break;
        default:
            break;
    }

    // Apply material
    if (Material)
    {
        ApplyMaterialToInstances(Material);
    }
}

void UTimelineEffectComponent::UpdateVFX(ETimelineState State)
{
    if (!bPlayVFX)
    {
        return;
    }

    // Clean up existing VFX
    if (ActiveVFX)
    {
        ActiveVFX->DestroyComponent();
        ActiveVFX = nullptr;
    }

    // Get appropriate VFX
    UParticleSystem* VFX = nullptr;
    switch (State)
    {
        case ETimelineState::BrightWorld:
            VFX = BrightTimelineVFX;
            break;
        case ETimelineState::DarkWorld:
            VFX = DarkTimelineVFX;
            break;
        default:
            break;
    }

    // Spawn new VFX
    if (VFX)
    {
        ActiveVFX = UGameplayStatics::SpawnEmitterAttached(
            VFX,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset
        );
    }
}

void UTimelineEffectComponent::UpdateAmbientSound(ETimelineState State)
{
    if (!bPlaySounds)
    {
        return;
    }

    // Clean up existing sound
    if (AmbientSound)
    {
        AmbientSound->Stop();
        AmbientSound->DestroyComponent();
        AmbientSound = nullptr;
    }

    // Get appropriate sound
    USoundBase* Sound = nullptr;
    switch (State)
    {
        case ETimelineState::BrightWorld:
            Sound = BrightTimelineAmbient;
            break;
        case ETimelineState::DarkWorld:
            Sound = DarkTimelineAmbient;
            break;
        default:
            break;
    }

    // Play new sound
    if (Sound)
    {
        AmbientSound = UGameplayStatics::SpawnSoundAttached(
            Sound,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset,
            true,
            1.0f,
            1.0f,
            0.0f
        );
    }
}

void UTimelineEffectComponent::UpdateTransitionEffects(float Progress)
{
    if (!bIsEnabled)
    {
        return;
    }

    // Update material transition
    if (bAffectMaterials)
    {
        UpdateMaterialTransition(Progress);
    }

    // Play transition VFX at start
    if (bPlayVFX && Progress <= 0.1f)
    {
        PlayTransitionVFX();
    }

    // Play transition sound at start
    if (bPlaySounds && Progress <= 0.1f)
    {
        PlayTransitionSound();
    }
}

void UTimelineEffectComponent::PlayTransitionVFX()
{
    if (TransitionVFX)
    {
        UGameplayStatics::SpawnEmitterAttached(
            TransitionVFX,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset
        );
    }
}

void UTimelineEffectComponent::PlayTransitionSound()
{
    if (TransitionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            TransitionSound,
            GetOwner()->GetActorLocation()
        );
    }
}

void UTimelineEffectComponent::CollectMaterialInstances()
{
    MaterialInstances.Empty();

    // Get mesh components
    TArray<UMeshComponent*> MeshComponents;
    GetOwner()->GetComponents<UMeshComponent>(MeshComponents);

    // Create dynamic material instances
    for (UMeshComponent* Mesh : MeshComponents)
    {
        if (Mesh)
        {
            for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
            {
                if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(i))
                {
                    MaterialInstances.Add(DynMaterial);
                }
            }
        }
    }
}

void UTimelineEffectComponent::ApplyMaterialToInstances(UMaterialInterface* Material)
{
    if (!Material)
    {
        return;
    }

    for (UMaterialInstanceDynamic* Instance : MaterialInstances)
    {
        if (Instance)
        {
            Instance->CopyParameterOverrides(Material);
        }
    }
}

void UTimelineEffectComponent::UpdateMaterialTransition(float Progress)
{
    for (UMaterialInstanceDynamic* Instance : MaterialInstances)
    {
        if (Instance)
        {
            Instance->SetScalarParameterValue(TEXT("TransitionProgress"), Progress);
        }
    }
}

void UTimelineEffectComponent::CleanupEffects()
{
    // Clean up VFX
    if (ActiveVFX)
    {
        ActiveVFX->DestroyComponent();
        ActiveVFX = nullptr;
    }

    // Clean up sound
    if (AmbientSound)
    {
        AmbientSound->Stop();
        AmbientSound->DestroyComponent();
        AmbientSound = nullptr;
    }

    // Reset materials
    MaterialInstances.Empty();
}
