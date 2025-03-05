#include "SETimelinePostProcessComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"

// Static parameter names
const FName USETimelinePostProcessComponent::TransitionProgressParam = TEXT("TransitionProgress");
const FName USETimelinePostProcessComponent::DistortionStrengthParam = TEXT("DistortionStrength");
const FName USETimelinePostProcessComponent::EmissiveIntensityParam = TEXT("EmissiveIntensity");
const FName USETimelinePostProcessComponent::NoiseScaleParam = TEXT("NoiseScale");
const FName USETimelinePostProcessComponent::EdgeSharpnessParam = TEXT("EdgeSharpness");
const FName USETimelinePostProcessComponent::TimeScaleParam = TEXT("TimeScale");
const FName USETimelinePostProcessComponent::LightColorParam = TEXT("LightTimelineColor");
const FName USETimelinePostProcessComponent::DarkColorParam = TEXT("DarkTimelineColor");

USETimelinePostProcessComponent::USETimelinePostProcessComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsTransitioning = false;
    CurrentTime = 0.0f;
    TransitionDuration = 1.0f;
    
    // Default effect settings
    DistortionStrength = 0.1f;
    EmissiveIntensity = 2.0f;
    NoiseScale = 1.0f;
    EdgeSharpness = 8.0f;
    TimeScale = 1.0f;
    
    // Performance tracking
    LastFrameTime = 0.0f;
    FrameCounter = 0;
    AverageFrameTime = 0.0f;
    CurrentQualityLevel = 2; // Start at high quality
    bDynamicQualityEnabled = true;
}

void USETimelinePostProcessComponent::BeginPlay()
{
    Super::BeginPlay();
    
    InitializePostProcess();
    InitializeColorCache();
    PreloadResources();
}

void USETimelinePostProcessComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsTransitioning)
    {
        CurrentTime += DeltaTime;
        float Progress = FMath::Clamp(CurrentTime / TransitionDuration, 0.0f, 1.0f);
        
        UpdateMaterialParameters(Progress);
        UpdatePerformanceMetrics(DeltaTime);
        
        if (Progress >= 1.0f)
        {
            HandleEffectComplete();
        }
    }

    // Adjust quality based on performance if enabled
    if (bDynamicQualityEnabled)
    {
        AdjustQualityDynamic();
    }
}

void USETimelinePostProcessComponent::StartTransitionEffect(ETimelineState FromState, ETimelineState ToState, float Duration)
{
    if (bIsTransitioning)
    {
        StopTransitionEffect();
    }

    CurrentFromState = FromState;
    CurrentToState = ToState;
    TransitionDuration = Duration;
    CurrentTime = 0.0f;
    bIsTransitioning = true;

    // Setup initial parameters
    SetupMaterialParameters();
}

void USETimelinePostProcessComponent::StopTransitionEffect()
{
    if (bIsTransitioning)
    {
        bIsTransitioning = false;
        CleanupEffect();
    }
}

void USETimelinePostProcessComponent::UpdateEffectParameters(float Progress)
{
    if (PostProcessMaterial)
    {
        UpdateMaterialParameters(Progress);
    }
}

void USETimelinePostProcessComponent::SetEffectQuality(int32 QualityLevel)
{
    if (QualityLevel >= 0 && QualityLevel < QualityPresets.Num())
    {
        CurrentQualityLevel = QualityLevel;
        ApplyQualitySettings(QualityLevel);
    }
}

void USETimelinePostProcessComponent::InitializePostProcess()
{
    // Create post process component if needed
    if (!PostProcessComponent)
    {
        PostProcessComponent = NewObject<UPostProcessComponent>(GetOwner());
        PostProcessComponent->RegisterComponent();
    }

    // Load material parameter collection
    TransitionParameters = Cast<UMaterialParameterCollection>(StaticLoadObject(
        UMaterialParameterCollection::StaticClass(),
        nullptr,
        TEXT("/Game/Materials/MPC_TimelineTransition")
    ));

    // Create dynamic material instance
    UMaterial* BaseMaterial = Cast<UMaterial>(StaticLoadObject(
        UMaterial::StaticClass(),
        nullptr,
        TEXT("/Game/Materials/PostProcess/PP_TimelineTransition")
    ));

    if (BaseMaterial)
    {
        PostProcessMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (PostProcessComponent && PostProcessMaterial)
        {
            PostProcessComponent->Settings.WeightedBlendables.Array.Add(
                FWeightedBlendable(1.0f, PostProcessMaterial)
            );
        }
    }
}

void USETimelinePostProcessComponent::SetupMaterialParameters()
{
    if (PostProcessMaterial)
    {
        PostProcessMaterial->SetScalarParameterValue(DistortionStrengthParam, DistortionStrength);
        PostProcessMaterial->SetScalarParameterValue(EmissiveIntensityParam, EmissiveIntensity);
        PostProcessMaterial->SetScalarParameterValue(NoiseScaleParam, NoiseScale);
        PostProcessMaterial->SetScalarParameterValue(EdgeSharpnessParam, EdgeSharpness);
        PostProcessMaterial->SetScalarParameterValue(TimeScaleParam, TimeScale);
        
        PostProcessMaterial->SetVectorParameterValue(LightColorParam, GetStateColor(ETimelineState::Light));
        PostProcessMaterial->SetVectorParameterValue(DarkColorParam, GetStateColor(ETimelineState::Dark));
    }
}

void USETimelinePostProcessComponent::UpdateMaterialParameters(float Progress)
{
    if (PostProcessMaterial)
    {
        PostProcessMaterial->SetScalarParameterValue(TransitionProgressParam, Progress);
        
        // Interpolate effect parameters
        float CurrentDistortion = InterpolateParameter(DistortionStrength * 0.5f, DistortionStrength, Progress);
        float CurrentEmissive = InterpolateParameter(EmissiveIntensity * 0.8f, EmissiveIntensity, Progress);
        
        PostProcessMaterial->SetScalarParameterValue(DistortionStrengthParam, CurrentDistortion);
        PostProcessMaterial->SetScalarParameterValue(EmissiveIntensityParam, CurrentEmissive);
    }
}

void USETimelinePostProcessComponent::ApplyQualitySettings(int32 Level)
{
    if (Level >= 0 && Level < QualityPresets.Num())
    {
        const FVector2D& Settings = QualityPresets[Level];
        NoiseScale = Settings.X;
        DistortionStrength *= Settings.Y;
        
        if (PostProcessMaterial)
        {
            PostProcessMaterial->SetScalarParameterValue(NoiseScaleParam, NoiseScale);
            PostProcessMaterial->SetScalarParameterValue(DistortionStrengthParam, DistortionStrength);
        }
    }
}

FLinearColor USETimelinePostProcessComponent::GetStateColor(ETimelineState State) const
{
    const FLinearColor* CachedColor = ColorCache.Find(State);
    if (CachedColor)
    {
        return *CachedColor;
    }

    switch (State)
    {
        case ETimelineState::Light:
            return FLinearColor(1.0f, 0.9f, 0.7f);
        case ETimelineState::Dark:
            return FLinearColor(0.2f, 0.0f, 0.4f);
        default:
            return FLinearColor::White;
    }
}

void USETimelinePostProcessComponent::HandleEffectComplete()
{
    bIsTransitioning = false;
    OnEffectComplete.Broadcast(CurrentToState);
    CleanupEffect();
}

void USETimelinePostProcessComponent::CleanupEffect()
{
    if (PostProcessMaterial)
    {
        PostProcessMaterial->SetScalarParameterValue(TransitionProgressParam, 0.0f);
        PostProcessMaterial->SetScalarParameterValue(DistortionStrengthParam, 0.0f);
    }
}

void USETimelinePostProcessComponent::MonitorPerformance()
{
    FrameCounter++;
    if (FrameCounter >= 60) // Update every 60 frames
    {
        AverageFrameTime = LastFrameTime / 60.0f;
        FrameCounter = 0;
        LastFrameTime = 0.0f;
    }
}

void USETimelinePostProcessComponent::InitializeColorCache()
{
    ColorCache.Add(ETimelineState::Light, FLinearColor(1.0f, 0.9f, 0.7f));
    ColorCache.Add(ETimelineState::Dark, FLinearColor(0.2f, 0.0f, 0.4f));
}

void USETimelinePostProcessComponent::UpdatePerformanceMetrics(float DeltaTime)
{
    LastFrameTime += DeltaTime;
    MonitorPerformance();
}

void USETimelinePostProcessComponent::AdjustQualityDynamic()
{
    if (!bDynamicQualityEnabled || AverageFrameTime == 0.0f)
    {
        return;
    }

    const float TargetFrameTime = 1.0f / 60.0f; // Target 60 FPS
    const float Threshold = 0.1f; // 10% tolerance

    if (AverageFrameTime > TargetFrameTime * (1.0f + Threshold) && CurrentQualityLevel > 0)
    {
        // Reduce quality
        SetEffectQuality(CurrentQualityLevel - 1);
    }
    else if (AverageFrameTime < TargetFrameTime * (1.0f - Threshold) && CurrentQualityLevel < QualityPresets.Num() - 1)
    {
        // Increase quality
        SetEffectQuality(CurrentQualityLevel + 1);
    }
}

void USETimelinePostProcessComponent::PreloadResources()
{
    // Ensure materials and textures are loaded
    if (TransitionParameters)
    {
        TransitionParameters->AddToRoot();
    }
}

void USETimelinePostProcessComponent::ReleaseResources()
{
    if (TransitionParameters)
    {
        TransitionParameters->RemoveFromRoot();
    }
}

void USETimelinePostProcessComponent::ValidateResources() const
{
    if (!PostProcessMaterial || !TransitionParameters)
    {
        UE_LOG(LogTemp, Warning, TEXT("Timeline post-process resources not properly initialized"));
    }
}

float USETimelinePostProcessComponent::InterpolateParameter(float Start, float End, float Progress) const
{
    return FMath::Lerp(Start, End, Progress);
}

FLinearColor USETimelinePostProcessComponent::InterpolateColor(const FLinearColor& Start, const FLinearColor& End, float Progress) const
{
    return FLinearColor::LerpUsingHSV(Start, End, Progress);
}
