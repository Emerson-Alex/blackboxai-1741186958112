// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Systems/SELoadingStateManager.h"
#include "Systems/SETimelineTransitionSystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "TimerManager.h"

USELoadingStateManager::USELoadingStateManager()
    : LoadingUpdateInterval(0.1f)
    , MaxConcurrentLoads(5)
    , bIsPaused(false)
{
    // Initialize loading state
    CurrentState.Progress = 0.0f;
    CurrentState.bIsComplete = false;
}

void USELoadingStateManager::Initialize(UTimelineTransitionSystem* InTransitionSystem)
{
    TransitionSystem = InTransitionSystem;

    // Set up timer for loading updates
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            UpdateTimerHandle,
            this,
            &USELoadingStateManager::UpdateLoadingState,
            LoadingUpdateInterval,
            true
        );
    }
}

void USELoadingStateManager::BeginLoading(const TArray<FSoftObjectPath>& Assets)
{
    // Reset current state
    CurrentState.Progress = 0.0f;
    CurrentState.bIsComplete = false;
    CurrentState.PendingAssets = Assets;
    CurrentState.LoadedAssets.Empty();

    bIsPaused = false;

    // Start loading process
    ProcessNextAsset();

    // Notify loading started
    BP_OnLoadingStarted();
}

void USELoadingStateManager::CancelLoading()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateTimerHandle);
    }

    // Cancel pending loads
    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
    for (const FSoftObjectPath& Asset : CurrentState.PendingAssets)
    {
        StreamableManager.Unload(Asset);
    }

    // Reset state
    CurrentState.PendingAssets.Empty();
    CurrentState.LoadedAssets.Empty();
    CurrentState.Progress = 0.0f;
    CurrentState.bIsComplete = false;

    // Notify cancellation
    BP_OnLoadingCancelled();
}

void USELoadingStateManager::PauseLoading()
{
    if (bIsPaused)
    {
        return;
    }

    bIsPaused = true;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(UpdateTimerHandle);
    }

    // Notify pause
    BP_OnLoadingPaused();
}

void USELoadingStateManager::ResumeLoading()
{
    if (!bIsPaused)
    {
        return;
    }

    bIsPaused = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().UnPauseTimer(UpdateTimerHandle);
    }

    // Continue loading
    ProcessNextAsset();

    // Notify resume
    BP_OnLoadingResumed();
}

void USELoadingStateManager::AddAsset(const FSoftObjectPath& Asset)
{
    if (!CurrentState.PendingAssets.Contains(Asset))
    {
        CurrentState.PendingAssets.Add(Asset);
        
        // Update progress calculation
        float NewProgress = CalculateProgress();
        if (NewProgress != CurrentState.Progress)
        {
            CurrentState.Progress = NewProgress;
            OnLoadingStateChanged.Broadcast(CurrentState.Progress);
        }
    }
}

void USELoadingStateManager::RemoveAsset(const FSoftObjectPath& Asset)
{
    // Remove from pending if not loaded
    if (CurrentState.PendingAssets.Remove(Asset) > 0)
    {
        // Update progress calculation
        float NewProgress = CalculateProgress();
        if (NewProgress != CurrentState.Progress)
        {
            CurrentState.Progress = NewProgress;
            OnLoadingStateChanged.Broadcast(CurrentState.Progress);
        }
    }

    // Unload if already loaded
    if (CurrentState.LoadedAssets.Contains(Asset))
    {
        FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
        StreamableManager.Unload(Asset);
        CurrentState.LoadedAssets.Remove(Asset);
    }
}

void USELoadingStateManager::UpdateLoadingState()
{
    if (bIsPaused || CurrentState.bIsComplete)
    {
        return;
    }

    // Calculate new progress
    float NewProgress = CalculateProgress();
    
    if (NewProgress != CurrentState.Progress)
    {
        CurrentState.Progress = NewProgress;
        OnLoadingStateChanged.Broadcast(CurrentState.Progress);
    }

    // Check if loading is complete
    if (CurrentState.PendingAssets.Num() == 0 && 
        CurrentState.LoadedAssets.Num() > 0)
    {
        CurrentState.bIsComplete = true;
        CleanupLoading();
    }
    else
    {
        // Process next batch of assets
        ProcessNextAsset();
    }
}

void USELoadingStateManager::ProcessNextAsset()
{
    if (bIsPaused || CurrentState.bIsComplete)
    {
        return;
    }

    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

    // Calculate how many more assets we can load
    int32 CurrentlyLoading = CurrentState.LoadedAssets.Num();
    int32 AvailableSlots = MaxConcurrentLoads - CurrentlyLoading;

    // Start loading next batch of assets
    for (int32 i = 0; i < AvailableSlots && i < CurrentState.PendingAssets.Num(); ++i)
    {
        FSoftObjectPath& Asset = CurrentState.PendingAssets[i];
        
        StreamableManager.RequestAsyncLoad(
            Asset,
            FStreamableDelegate::CreateUObject(this, &USELoadingStateManager::OnAssetLoaded)
        );
    }
}

void USELoadingStateManager::OnAssetLoaded()
{
    // Move asset from pending to loaded
    if (CurrentState.PendingAssets.Num() > 0)
    {
        FSoftObjectPath LoadedAsset = CurrentState.PendingAssets[0];
        CurrentState.LoadedAssets.Add(LoadedAsset);
        CurrentState.PendingAssets.RemoveAt(0);

        // Update progress
        float NewProgress = CalculateProgress();
        if (NewProgress != CurrentState.Progress)
        {
            CurrentState.Progress = NewProgress;
            OnLoadingStateChanged.Broadcast(CurrentState.Progress);
        }

        // Process next asset if available
        ProcessNextAsset();
    }

    // Check if loading is complete
    if (CurrentState.PendingAssets.Num() == 0)
    {
        CurrentState.bIsComplete = true;
        CleanupLoading();
    }
}

float USELoadingStateManager::CalculateProgress() const
{
    int32 TotalAssets = CurrentState.LoadedAssets.Num() + CurrentState.PendingAssets.Num();
    if (TotalAssets == 0)
    {
        return 1.0f;
    }

    return static_cast<float>(CurrentState.LoadedAssets.Num()) / static_cast<float>(TotalAssets);
}

void USELoadingStateManager::CleanupLoading()
{
    // Notify completion
    BP_OnLoadingComplete();

    // Update transition system
    if (TransitionSystem)
    {
        TransitionSystem->UpdateTransitionState();
    }

    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(UpdateTimerHandle);
    }
}
