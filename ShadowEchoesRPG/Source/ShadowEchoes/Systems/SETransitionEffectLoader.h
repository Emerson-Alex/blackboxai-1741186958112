#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/SETimelineTypes.h"
#include "Engine/DataTable.h"
#include "SETransitionEffectLoader.generated.h"

USTRUCT(BlueprintType)
struct FTransitionEffectData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* ParticleSystem;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class USoundBase* SoundEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UMaterialInterface* TransitionMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UCurveFloat* IntensityCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FLinearColor EffectColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    bool bLooping;
};

USTRUCT(BlueprintType)
struct FTransitionEffectTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FTransitionEffectData EffectData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    ETimelineState RequiredState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TArray<FName> Tags;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectLoadComplete, ETimelineState, State);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEffectLoadError, ETimelineState, State, const FString&, ErrorMessage);

UCLASS(BlueprintType)
class SHADOWECHOES_API USETransitionEffectLoader : public UObject
{
    GENERATED_BODY()

public:
    USETransitionEffectLoader();

    // Effect Loading
    UFUNCTION(BlueprintCallable, Category = "Timeline Effects")
    void PreloadEffects(ETimelineState State);

    UFUNCTION(BlueprintCallable, Category = "Timeline Effects")
    void UnloadEffects(ETimelineState State);

    UFUNCTION(BlueprintCallable, Category = "Timeline Effects")
    void UnloadUnusedEffects();

    // Effect Retrieval
    UFUNCTION(BlueprintPure, Category = "Timeline Effects")
    const FTransitionEffectData* GetEffectData(ETimelineState State) const;

    UFUNCTION(BlueprintPure, Category = "Timeline Effects")
    bool HasEffectsLoaded(ETimelineState State) const;

    // Effect Management
    UFUNCTION(BlueprintCallable, Category = "Timeline Effects")
    void UpdateEffectCache(UPARAM(ref) TMap<ETimelineState, FTransitionEffectData>& OutCache);

    UFUNCTION(BlueprintCallable, Category = "Timeline Effects")
    void SetEffectScale(ETimelineState State, float Scale);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Timeline Effects")
    FOnEffectLoadComplete OnEffectLoadComplete;

    UPROPERTY(BlueprintAssignable, Category = "Timeline Effects")
    FOnEffectLoadError OnEffectLoadError;

protected:
    // Data storage
    UPROPERTY()
    UDataTable* EffectDataTable;

    UPROPERTY()
    TMap<ETimelineState, FTransitionEffectData> LoadedEffects;

    UPROPERTY()
    TSet<ETimelineState> LoadingStates;

    // Internal methods
    bool LoadEffectDataTable();
    bool LoadEffectData(ETimelineState State);
    void CleanupEffectData(ETimelineState State);
    bool ValidateEffectData(const FTransitionEffectData& Data) const;

    // Asset management
    void PreloadAssets(const FTransitionEffectData& Data);
    void UnloadAssets(const FTransitionEffectData& Data);
    bool AreAssetsLoaded(const FTransitionEffectData& Data) const;

private:
    // Constants
    static const FString EffectTablePath;
    static const float MaxMemoryUsageMB;

    // Memory tracking
    float CurrentMemoryUsageMB;
    void TrackMemoryUsage(const FTransitionEffectData& Data, bool bAdd);
    bool WouldExceedMemoryLimit(const FTransitionEffectData& Data) const;

    // Cache management
    TMap<FName, float> AssetSizeCache;
    void UpdateAssetSizeCache();
    float GetAssetSize(UObject* Asset) const;

    // Error handling
    void LogEffectError(const FString& ErrorMessage) const;
    bool ValidateEffectState(ETimelineState State) const;

    // Async loading
    void HandleAssetLoaded();
    void HandleLoadError(const FString& ErrorMessage);
    
    // Performance optimization
    void OptimizeMemoryUsage();
    void PrioritizeEffectLoading();

    // Debug helpers
    void DumpEffectLoadingStatus() const;
    void ValidateEffectIntegrity() const;
};
