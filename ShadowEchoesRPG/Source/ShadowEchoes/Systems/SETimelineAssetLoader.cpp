#include "SETimelineAssetLoader.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Materials/MaterialInterface.h"

const FString USETimelineAssetLoader::AssetTablePath = TEXT("/Game/Data/DT_TimelineAssets");
const float USETimelineAssetLoader::MaxMemoryUsageMB = 512.0f;

USETimelineAssetLoader::USETimelineAssetLoader()
{
    CurrentMemoryUsageMB = 0.0f;
    TimelineAssetTable = nullptr;
}

bool USETimelineAssetLoader::LoadTimelineAssets()
{
    if (!LoadAssetDataTable())
    {
        LogAssetError(TEXT("Failed to load Timeline Asset Data Table"));
        return false;
    }

    // Preload essential states (Light and Dark)
    bool bSuccess = PreloadStateAssets(ETimelineState::Light) && 
                   PreloadStateAssets(ETimelineState::Dark);

    if (!bSuccess)
    {
        LogAssetError(TEXT("Failed to preload essential timeline states"));
        return false;
    }

    return true;
}

bool USETimelineAssetLoader::PreloadStateAssets(ETimelineState State)
{
    if (IsStateLoaded(State))
    {
        return true;
    }

    if (!ValidateAssetLoading(State))
    {
        return false;
    }

    // Check memory constraints before loading
    ManageMemoryUsage();

    if (!LoadStateData(State))
    {
        LogAssetError(FString::Printf(TEXT("Failed to load assets for state: %d"), static_cast<int32>(State)));
        return false;
    }

    LoadedStates.Add(State);
    UpdateAssetCache();

    return true;
}

void USETimelineAssetLoader::UnloadStateAssets(ETimelineState State)
{
    if (!IsStateLoaded(State))
    {
        return;
    }

    CleanupStateData(State);
    LoadedStates.Remove(State);
    LoadedAssets.Remove(State);
    UpdateAssetCache();
}

UParticleSystem* USETimelineAssetLoader::GetTransitionEffect(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->TransitionEffect : nullptr;
}

USoundBase* USETimelineAssetLoader::GetTransitionSound(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->TransitionSound : nullptr;
}

UMaterialInterface* USETimelineAssetLoader::GetEnvironmentMaterial(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->EnvironmentMaterial : nullptr;
}

const TArray<FTimelineEffect>& USETimelineAssetLoader::GetStateEffects(ETimelineState State) const
{
    static TArray<FTimelineEffect> EmptyEffects;
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->StateEffects : EmptyEffects;
}

FLinearColor USETimelineAssetLoader::GetAmbientColor(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->AmbientColor : FLinearColor::Black;
}

float USETimelineAssetLoader::GetFogDensity(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->FogDensity : 0.0f;
}

float USETimelineAssetLoader::GetLightIntensity(ETimelineState State) const
{
    const FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    return AssetData ? AssetData->LightIntensity : 1.0f;
}

void USETimelineAssetLoader::PurgeUnusedAssets()
{
    TArray<ETimelineState> StatesToUnload;
    
    for (const auto& Pair : LoadedAssets)
    {
        if (!LoadedStates.Contains(Pair.Key))
        {
            StatesToUnload.Add(Pair.Key);
        }
    }

    for (ETimelineState State : StatesToUnload)
    {
        UnloadStateAssets(State);
    }

    UpdateAssetCache();
}

bool USETimelineAssetLoader::IsStateLoaded(ETimelineState State) const
{
    return LoadedStates.Contains(State);
}

bool USETimelineAssetLoader::LoadAssetDataTable()
{
    TimelineAssetTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *AssetTablePath));
    
    if (!TimelineAssetTable)
    {
        LogAssetError(TEXT("Failed to load Timeline Asset Data Table"));
        return false;
    }

    return true;
}

bool USETimelineAssetLoader::LoadStateData(ETimelineState State)
{
    if (!TimelineAssetTable)
    {
        return false;
    }

    FString RowName = FString::Printf(TEXT("Timeline_%d"), static_cast<int32>(State));
    FTimelineAssetData* AssetData = TimelineAssetTable->FindRow<FTimelineAssetData>(*RowName, TEXT(""));

    if (!AssetData || !ValidateAssetData(*AssetData))
    {
        return false;
    }

    // Track memory before loading
    TrackMemoryUsage(*AssetData, true);

    LoadedAssets.Add(State, *AssetData);
    return true;
}

void USETimelineAssetLoader::CleanupStateData(ETimelineState State)
{
    FTimelineAssetData* AssetData = LoadedAssets.Find(State);
    if (AssetData)
    {
        TrackMemoryUsage(*AssetData, false);
    }
}

bool USETimelineAssetLoader::ValidateAssetData(const FTimelineAssetData& AssetData) const
{
    // Validate required assets
    if (!AssetData.TransitionEffect || !AssetData.TransitionSound || !AssetData.EnvironmentMaterial)
    {
        LogAssetError(TEXT("Missing required assets in Timeline Asset Data"));
        return false;
    }

    // Validate parameters
    if (AssetData.FogDensity < 0.0f || AssetData.LightIntensity < 0.0f)
    {
        LogAssetError(TEXT("Invalid parameter values in Timeline Asset Data"));
        return false;
    }

    return true;
}

void USETimelineAssetLoader::UpdateAssetCache()
{
    AssetIndexCache.Empty(LoadedAssets.Num());
    int32 Index = 0;
    
    for (const auto& Pair : LoadedAssets)
    {
        AssetIndexCache.Add(Pair.Key, Index++);
    }
}

void USETimelineAssetLoader::ManageMemoryUsage()
{
    // If we're approaching memory limit, unload least recently used states
    if (CurrentMemoryUsageMB >= MaxMemoryUsageMB * 0.9f)
    {
        TArray<ETimelineState> LoadedStatesCopy = LoadedStates.Array();
        LoadedStatesCopy.Sort([this](const ETimelineState& A, const ETimelineState& B) {
            return AssetIndexCache[A] < AssetIndexCache[B];
        });

        // Unload states until we're under 75% of max memory
        for (ETimelineState State : LoadedStatesCopy)
        {
            if (State != ETimelineState::Light && State != ETimelineState::Dark)
            {
                UnloadStateAssets(State);
            }

            if (CurrentMemoryUsageMB < MaxMemoryUsageMB * 0.75f)
            {
                break;
            }
        }
    }
}

void USETimelineAssetLoader::TrackMemoryUsage(const FTimelineAssetData& AssetData, bool bAdd)
{
    // Approximate memory usage for assets (in MB)
    float MemoryDelta = 0.0f;

    // Particle systems (~5MB each)
    if (AssetData.TransitionEffect)
    {
        MemoryDelta += 5.0f;
    }

    // Sound assets (~1MB each)
    if (AssetData.TransitionSound)
    {
        MemoryDelta += 1.0f;
    }

    // Materials (~2MB each)
    if (AssetData.EnvironmentMaterial)
    {
        MemoryDelta += 2.0f;
    }

    // Effects (~0.1MB each)
    MemoryDelta += AssetData.StateEffects.Num() * 0.1f;

    if (bAdd)
    {
        CurrentMemoryUsageMB += MemoryDelta;
    }
    else
    {
        CurrentMemoryUsageMB -= MemoryDelta;
    }

    CurrentMemoryUsageMB = FMath::Max(0.0f, CurrentMemoryUsageMB);
}

void USETimelineAssetLoader::LogAssetError(const FString& ErrorMessage) const
{
    UE_LOG(LogTemp, Error, TEXT("Timeline Asset Error: %s"), *ErrorMessage);
}

bool USETimelineAssetLoader::ValidateAssetLoading(ETimelineState State) const
{
    if (State == ETimelineState::None)
    {
        LogAssetError(TEXT("Cannot load assets for None state"));
        return false;
    }

    if (!TimelineAssetTable)
    {
        LogAssetError(TEXT("Asset table not loaded"));
        return false;
    }

    return true;
}
