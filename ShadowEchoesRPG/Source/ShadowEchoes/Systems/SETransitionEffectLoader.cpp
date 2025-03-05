#include "SETransitionEffectLoader.h"
#include "Engine/DataTable.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"
#include "Curves/CurveFloat.h"

const FString USETransitionEffectLoader::EffectTablePath = TEXT("/Game/Data/DT_TransitionEffects");
const float USETransitionEffectLoader::MaxMemoryUsageMB = 256.0f;

USETransitionEffectLoader::USETransitionEffectLoader()
{
    CurrentMemoryUsageMB = 0.0f;
    EffectDataTable = nullptr;
}

void USETransitionEffectLoader::PreloadEffects(ETimelineState State)
{
    if (!ValidateEffectState(State))
    {
        return;
    }

    if (LoadingStates.Contains(State))
    {
        return;
    }

    // Check if already loaded
    if (HasEffectsLoaded(State))
    {
        OnEffectLoadComplete.Broadcast(State);
        return;
    }

    // Load data table if needed
    if (!LoadEffectDataTable())
    {
        HandleLoadError(TEXT("Failed to load effect data table"));
        return;
    }

    // Track loading state
    LoadingStates.Add(State);

    // Load effect data
    if (!LoadEffectData(State))
    {
        LoadingStates.Remove(State);
        HandleLoadError(FString::Printf(TEXT("Failed to load effect data for state %d"), static_cast<int32>(State)));
        return;
    }

    // Preload assets
    const FTransitionEffectData* EffectData = LoadedEffects.Find(State);
    if (EffectData)
    {
        PreloadAssets(*EffectData);
    }

    // Update memory tracking
    OptimizeMemoryUsage();

    // Complete loading
    LoadingStates.Remove(State);
    OnEffectLoadComplete.Broadcast(State);
}

void USETransitionEffectLoader::UnloadEffects(ETimelineState State)
{
    if (!ValidateEffectState(State))
    {
        return;
    }

    if (LoadingStates.Contains(State))
    {
        LoadingStates.Remove(State);
    }

    CleanupEffectData(State);
    LoadedEffects.Remove(State);
    UpdateAssetSizeCache();
}

void USETransitionEffectLoader::UnloadUnusedEffects()
{
    TArray<ETimelineState> StatesToUnload;
    
    for (const auto& Pair : LoadedEffects)
    {
        if (!LoadingStates.Contains(Pair.Key))
        {
            StatesToUnload.Add(Pair.Key);
        }
    }

    for (ETimelineState State : StatesToUnload)
    {
        UnloadEffects(State);
    }
}

const FTransitionEffectData* USETransitionEffectLoader::GetEffectData(ETimelineState State) const
{
    return LoadedEffects.Find(State);
}

bool USETransitionEffectLoader::HasEffectsLoaded(ETimelineState State) const
{
    const FTransitionEffectData* EffectData = LoadedEffects.Find(State);
    return EffectData && AreAssetsLoaded(*EffectData);
}

void USETransitionEffectLoader::UpdateEffectCache(TMap<ETimelineState, FTransitionEffectData>& OutCache)
{
    OutCache = LoadedEffects;
}

void USETransitionEffectLoader::SetEffectScale(ETimelineState State, float Scale)
{
    FTransitionEffectData* EffectData = LoadedEffects.Find(State);
    if (EffectData)
    {
        EffectData->Scale = Scale;
    }
}

bool USETransitionEffectLoader::LoadEffectDataTable()
{
    if (EffectDataTable)
    {
        return true;
    }

    EffectDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *EffectTablePath));
    if (!EffectDataTable)
    {
        LogEffectError(TEXT("Failed to load effect data table"));
        return false;
    }

    return true;
}

bool USETransitionEffectLoader::LoadEffectData(ETimelineState State)
{
    if (!EffectDataTable)
    {
        return false;
    }

    FString RowName = FString::Printf(TEXT("Effect_%d"), static_cast<int32>(State));
    FTransitionEffectTableRow* Row = EffectDataTable->FindRow<FTransitionEffectTableRow>(*RowName, TEXT(""));

    if (!Row)
    {
        LogEffectError(FString::Printf(TEXT("No effect data found for state %d"), static_cast<int32>(State)));
        return false;
    }

    if (!ValidateEffectData(Row->EffectData))
    {
        return false;
    }

    // Check memory limits
    if (WouldExceedMemoryLimit(Row->EffectData))
    {
        OptimizeMemoryUsage();
        if (WouldExceedMemoryLimit(Row->EffectData))
        {
            LogEffectError(TEXT("Insufficient memory for effect data"));
            return false;
        }
    }

    // Store effect data
    LoadedEffects.Add(State, Row->EffectData);
    TrackMemoryUsage(Row->EffectData, true);

    return true;
}

void USETransitionEffectLoader::CleanupEffectData(ETimelineState State)
{
    FTransitionEffectData* EffectData = LoadedEffects.Find(State);
    if (EffectData)
    {
        UnloadAssets(*EffectData);
        TrackMemoryUsage(*EffectData, false);
    }
}

bool USETransitionEffectLoader::ValidateEffectData(const FTransitionEffectData& Data) const
{
    if (!Data.ParticleSystem)
    {
        LogEffectError(TEXT("Missing particle system"));
        return false;
    }

    if (!Data.TransitionMaterial)
    {
        LogEffectError(TEXT("Missing transition material"));
        return false;
    }

    if (Data.Duration <= 0.0f)
    {
        LogEffectError(TEXT("Invalid effect duration"));
        return false;
    }

    return true;
}

void USETransitionEffectLoader::PreloadAssets(const FTransitionEffectData& Data)
{
    if (Data.ParticleSystem)
    {
        Data.ParticleSystem->PreloadContent();
    }

    if (Data.SoundEffect)
    {
        Data.SoundEffect->PreloadAsync();
    }

    if (Data.TransitionMaterial)
    {
        Data.TransitionMaterial->PreloadTextures();
    }

    if (Data.IntensityCurve)
    {
        Data.IntensityCurve->GetCurveData();
    }
}

void USETransitionEffectLoader::UnloadAssets(const FTransitionEffectData& Data)
{
    // Note: UE4 handles actual unloading through garbage collection
    // We just need to remove our references
}

bool USETransitionEffectLoader::AreAssetsLoaded(const FTransitionEffectData& Data) const
{
    if (!Data.ParticleSystem || !Data.ParticleSystem->IsValidLowLevel())
    {
        return false;
    }

    if (Data.SoundEffect && !Data.SoundEffect->IsValidLowLevel())
    {
        return false;
    }

    if (!Data.TransitionMaterial || !Data.TransitionMaterial->IsValidLowLevel())
    {
        return false;
    }

    return true;
}

void USETransitionEffectLoader::TrackMemoryUsage(const FTransitionEffectData& Data, bool bAdd)
{
    float MemoryDelta = 0.0f;

    // Approximate memory usage for each asset type
    if (Data.ParticleSystem)
    {
        MemoryDelta += GetAssetSize(Data.ParticleSystem);
    }

    if (Data.SoundEffect)
    {
        MemoryDelta += GetAssetSize(Data.SoundEffect);
    }

    if (Data.TransitionMaterial)
    {
        MemoryDelta += GetAssetSize(Data.TransitionMaterial);
    }

    if (Data.IntensityCurve)
    {
        MemoryDelta += 0.1f; // Curves are typically small
    }

    if (bAdd)
    {
        CurrentMemoryUsageMB += MemoryDelta;
    }
    else
    {
        CurrentMemoryUsageMB = FMath::Max(0.0f, CurrentMemoryUsageMB - MemoryDelta);
    }
}

bool USETransitionEffectLoader::WouldExceedMemoryLimit(const FTransitionEffectData& Data) const
{
    float RequiredMemory = 0.0f;

    if (Data.ParticleSystem)
    {
        RequiredMemory += GetAssetSize(Data.ParticleSystem);
    }

    if (Data.SoundEffect)
    {
        RequiredMemory += GetAssetSize(Data.SoundEffect);
    }

    if (Data.TransitionMaterial)
    {
        RequiredMemory += GetAssetSize(Data.TransitionMaterial);
    }

    return (CurrentMemoryUsageMB + RequiredMemory) > MaxMemoryUsageMB;
}

void USETransitionEffectLoader::UpdateAssetSizeCache()
{
    AssetSizeCache.Empty();

    for (const auto& Pair : LoadedEffects)
    {
        const FTransitionEffectData& Data = Pair.Value;
        
        if (Data.ParticleSystem)
        {
            AssetSizeCache.Add(Data.ParticleSystem->GetFName(), GetAssetSize(Data.ParticleSystem));
        }

        if (Data.SoundEffect)
        {
            AssetSizeCache.Add(Data.SoundEffect->GetFName(), GetAssetSize(Data.SoundEffect));
        }

        if (Data.TransitionMaterial)
        {
            AssetSizeCache.Add(Data.TransitionMaterial->GetFName(), GetAssetSize(Data.TransitionMaterial));
        }
    }
}

float USETransitionEffectLoader::GetAssetSize(UObject* Asset) const
{
    if (!Asset)
    {
        return 0.0f;
    }

    const float* CachedSize = AssetSizeCache.Find(Asset->GetFName());
    if (CachedSize)
    {
        return *CachedSize;
    }

    // Approximate sizes based on asset type
    if (Cast<UParticleSystem>(Asset))
    {
        return 5.0f; // 5MB
    }
    else if (Cast<USoundBase>(Asset))
    {
        return 2.0f; // 2MB
    }
    else if (Cast<UMaterialInterface>(Asset))
    {
        return 1.0f; // 1MB
    }

    return 0.5f; // Default size for unknown assets
}

void USETransitionEffectLoader::LogEffectError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Warning, TEXT("Timeline Effect Error: %s"), *ErrorMessage);
}

bool USETransitionEffectLoader::ValidateEffectState(ETimelineState State) const
{
    if (State == ETimelineState::None)
    {
        LogEffectError(TEXT("Cannot load effects for None state"));
        return false;
    }

    return true;
}

void USETransitionEffectLoader::HandleAssetLoaded()
{
    // Handle async loading completion
    PrioritizeEffectLoading();
}

void USETransitionEffectLoader::HandleLoadError(const FString& ErrorMessage)
{
    LogEffectError(ErrorMessage);
    OnEffectLoadError.Broadcast(ETimelineState::None, ErrorMessage);
}

void USETransitionEffectLoader::OptimizeMemoryUsage()
{
    if (CurrentMemoryUsageMB >= MaxMemoryUsageMB * 0.9f)
    {
        // Unload least recently used effects
        TArray<ETimelineState> LoadedStates;
        LoadedEffects.GetKeys(LoadedStates);

        // Sort by priority (keep essential states)
        LoadedStates.Sort([](ETimelineState A, ETimelineState B) {
            // Keep Light and Dark states
            if (A == ETimelineState::Light || A == ETimelineState::Dark)
            {
                return false;
            }
            if (B == ETimelineState::Light || B == ETimelineState::Dark)
            {
                return true;
            }
            return static_cast<int32>(A) < static_cast<int32>(B);
        });

        // Unload until we're under 75% of max memory
        for (ETimelineState State : LoadedStates)
        {
            if (State != ETimelineState::Light && State != ETimelineState::Dark)
            {
                UnloadEffects(State);
            }

            if (CurrentMemoryUsageMB < MaxMemoryUsageMB * 0.75f)
            {
                break;
            }
        }
    }
}

void USETransitionEffectLoader::PrioritizeEffectLoading()
{
    // Prioritize loading essential states
    if (LoadingStates.Contains(ETimelineState::Light) || LoadingStates.Contains(ETimelineState::Dark))
    {
        // Pause loading of non-essential states
        TArray<ETimelineState> NonEssentialStates;
        for (ETimelineState State : LoadingStates)
        {
            if (State != ETimelineState::Light && State != ETimelineState::Dark)
            {
                NonEssentialStates.Add(State);
            }
        }

        for (ETimelineState State : NonEssentialStates)
        {
            LoadingStates.Remove(State);
        }
    }
}
