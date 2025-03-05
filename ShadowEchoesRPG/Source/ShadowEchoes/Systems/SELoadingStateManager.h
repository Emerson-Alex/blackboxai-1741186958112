// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SELoadingStateManager.generated.h"

class UTimelineTransitionSystem;

USTRUCT(BlueprintType)
struct FLoadingState
{
    GENERATED_BODY()

    UPROPERTY()
    float Progress;

    UPROPERTY()
    TArray<FSoftObjectPath> PendingAssets;

    UPROPERTY()
    TArray<FSoftObjectPath> LoadedAssets;

    UPROPERTY()
    bool bIsComplete;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingStateChanged, float, Progress);

/**
 * Manages loading states and asset streaming for timeline transitions
 */
UCLASS()
class SHADOWECHOES_API USELoadingStateManager : public UObject
{
    GENERATED_BODY()

public:
    USELoadingStateManager();

    /** Initialize the loading state manager */
    void Initialize(UTimelineTransitionSystem* InTransitionSystem);

    /** State management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void BeginLoading(const TArray<FSoftObjectPath>& Assets);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void CancelLoading();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void PauseLoading();

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void ResumeLoading();

    /** Asset management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void AddAsset(const FSoftObjectPath& Asset);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Loading")
    void RemoveAsset(const FSoftObjectPath& Asset);

    /** Progress tracking */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Loading")
    float GetLoadingProgress() const { return CurrentState.Progress; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Loading")
    bool IsLoadingComplete() const { return CurrentState.bIsComplete; }

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Loading|Events")
    FOnLoadingStateChanged OnLoadingStateChanged;

protected:
    /** Settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Loading")
    float LoadingUpdateInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Loading")
    int32 MaxConcurrentLoads;

    /** State */
    UPROPERTY()
    FLoadingState CurrentState;

    UPROPERTY()
    bool bIsPaused;

    UPROPERTY()
    UTimelineTransitionSystem* TransitionSystem;

    /** Internal functionality */
    void UpdateLoadingState();
    void ProcessNextAsset();
    void OnAssetLoaded();
    float CalculateProgress() const;
    void CleanupLoading();

    /** Timer handle */
    FTimerHandle UpdateTimerHandle;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Loading|Events")
    void BP_OnLoadingStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Loading|Events")
    void BP_OnLoadingComplete();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Loading|Events")
    void BP_OnLoadingCancelled();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Loading|Events")
    void BP_OnLoadingPaused();

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Loading|Events")
    void BP_OnLoadingResumed();
};
