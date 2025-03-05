#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/SETimelineTypes.h"
#include "Engine/DataTable.h"
#include "SETimelineAssetLoader.generated.h"

USTRUCT(BlueprintType)
struct FTimelineAssetData : public FTableRowBase
{
    GENERATED_BODY()

    // Visual assets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    class UParticleSystem* TransitionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    class USoundBase* TransitionSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    class UMaterialInterface* EnvironmentMaterial;

    // Timeline-specific parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    FLinearColor AmbientColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    float FogDensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    float LightIntensity;

    // Gameplay effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Assets")
    TArray<FTimelineEffect> StateEffects;
};

UCLASS(BlueprintType)
class SHADOWECHOES_API USETimelineAssetLoader : public UObject
{
    GENERATED_BODY()

public:
    USETimelineAssetLoader();

    // Asset loading
    UFUNCTION(BlueprintCallable, Category = "Timeline Assets")
    bool LoadTimelineAssets();

    UFUNCTION(BlueprintCallable, Category = "Timeline Assets")
    bool PreloadStateAssets(ETimelineState State);

    UFUNCTION(BlueprintCallable, Category = "Timeline Assets")
    void UnloadStateAssets(ETimelineState State);

    // Asset retrieval
    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    UParticleSystem* GetTransitionEffect(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    USoundBase* GetTransitionSound(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    UMaterialInterface* GetEnvironmentMaterial(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    const TArray<FTimelineEffect>& GetStateEffects(ETimelineState State) const;

    // Environment parameters
    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    FLinearColor GetAmbientColor(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    float GetFogDensity(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Assets")
    float GetLightIntensity(ETimelineState State) const;

    // Asset management
    UFUNCTION(BlueprintCallable, Category = "Timeline Assets")
    void PurgeUnusedAssets();

    UFUNCTION(BlueprintCallable, Category = "Timeline Assets")
    bool IsStateLoaded(ETimelineState State) const;

protected:
    // Asset data tables
    UPROPERTY()
    UDataTable* TimelineAssetTable;

    // Cached assets
    UPROPERTY()
    TMap<ETimelineState, FTimelineAssetData> LoadedAssets;

    // Loading state
    UPROPERTY()
    TSet<ETimelineState> LoadedStates;

    // Internal methods
    bool LoadAssetDataTable();
    bool LoadStateData(ETimelineState State);
    void CleanupStateData(ETimelineState State);
    bool ValidateAssetData(const FTimelineAssetData& AssetData) const;

    // Performance optimization
    void UpdateAssetCache();
    void ManageMemoryUsage();

    // Error handling
    void LogAssetError(const FString& ErrorMessage) const;
    bool ValidateAssetLoading(ETimelineState State) const;

private:
    // Constants
    static const FString AssetTablePath;
    static const float MaxMemoryUsageMB;

    // Cache for quick lookups
    TMap<ETimelineState, int32> AssetIndexCache;
    
    // Memory tracking
    float CurrentMemoryUsageMB;
    void TrackMemoryUsage(const FTimelineAssetData& AssetData, bool bAdd);
};
