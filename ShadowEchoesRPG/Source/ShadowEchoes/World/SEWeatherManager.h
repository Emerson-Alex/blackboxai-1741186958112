// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SEWeatherManager.generated.h"

class USEGameInstance;
class UTimelineManager;

UENUM(BlueprintType)
enum class EWeatherType : uint8
{
    Clear           UMETA(DisplayName = "Clear"),
    VoidStorm       UMETA(DisplayName = "Void Storm"),
    LightRain       UMETA(DisplayName = "Light Rain"),
    TimelineFlux    UMETA(DisplayName = "Timeline Flux"),
    ShadowMist      UMETA(DisplayName = "Shadow Mist"),
    RadiantGlow     UMETA(DisplayName = "Radiant Glow"),
    RealityStorm    UMETA(DisplayName = "Reality Storm")
};

USTRUCT(BlueprintType)
struct FWeatherEffect
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    EWeatherType WeatherType;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    float Intensity;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    float Duration;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    TArray<FName> AffectedAreas;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    ETimelineState PreferredTimeline;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    TMap<FString, float> GameplayEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    TArray<FName> SpecialEncounters;
};

USTRUCT(BlueprintType)
struct FEnvironmentalHazard
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    FName HazardID;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    FText Name;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    float DamagePerSecond;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    float Duration;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    float Radius;

    UPROPERTY(EditDefaultsOnly, Category = "Weather")
    TArray<FName> StatusEffects;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherChanged, EWeatherType, NewWeather, float, Intensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnvironmentalHazardSpawned, const FEnvironmentalHazard&, Hazard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimeOfDayChanged, float, TimeOfDay, bool, IsNight);

/**
 * Manages weather, environmental effects, and time of day
 */
UCLASS()
class SHADOWECHOES_API USEWeatherManager : public UObject
{
    GENERATED_BODY()

public:
    USEWeatherManager();

    /** Initialize the weather system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Weather control */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void SetWeather(EWeatherType NewWeather, float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void TransitionWeather(EWeatherType TargetWeather, float TransitionTime);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Weather")
    EWeatherType GetCurrentWeather() const { return CurrentWeather; }

    /** Environmental hazards */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void SpawnEnvironmentalHazard(const FEnvironmentalHazard& Hazard, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void RemoveEnvironmentalHazard(const FName& HazardID);

    /** Time of day */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void SetTimeOfDay(float NewTime);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Weather")
    float GetTimeOfDay() const { return CurrentTime; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Weather")
    bool IsNightTime() const;

    /** Season system */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void AdvanceSeason();

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Weather")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Weather|Events")
    FOnWeatherChanged OnWeatherChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Weather|Events")
    FOnEnvironmentalHazardSpawned OnEnvironmentalHazardSpawned;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Weather|Events")
    FOnTimeOfDayChanged OnTimeOfDayChanged;

protected:
    /** Weather settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Weather")
    float WeatherUpdateInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Weather")
    float TimeScale;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Weather")
    float DayLength;

private:
    /** Current state */
    UPROPERTY()
    EWeatherType CurrentWeather;

    UPROPERTY()
    float CurrentWeatherIntensity;

    UPROPERTY()
    float CurrentTime;

    UPROPERTY()
    int32 CurrentSeason;

    UPROPERTY()
    TMap<FName, FEnvironmentalHazard> ActiveHazards;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    void UpdateWeatherEffects();
    void ProcessEnvironmentalHazards();
    void UpdateTimeOfDay();
    void CheckWeatherTransitions();
    void ApplyWeatherEffects(const FWeatherEffect& Effect);
    void SpawnTimelineSpecificEffects(ETimelineState State);
    float CalculateHazardDamage(const FEnvironmentalHazard& Hazard) const;
    bool ShouldSpawnHazard(const FEnvironmentalHazard& Hazard) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Weather|Events")
    void BP_OnWeatherChanged(EWeatherType NewWeather, float Intensity);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Weather|Events")
    void BP_OnEnvironmentalHazardSpawned(const FEnvironmentalHazard& Hazard);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Weather|Events")
    void BP_OnTimeOfDayChanged(float TimeOfDay, bool bIsNight);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Weather|Events")
    void BP_OnSeasonChanged(int32 NewSeason);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Weather|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
