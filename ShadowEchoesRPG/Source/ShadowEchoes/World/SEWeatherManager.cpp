// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "World/SEWeatherManager.h"
#include "Core/SEGameInstance.h"
#include "Systems/TimelineManager.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

USEWeatherManager::USEWeatherManager()
    : WeatherUpdateInterval(5.0f)
    , TimeScale(1.0f)
    , DayLength(1440.0f) // 24 minutes real time = 24 hours game time
    , CurrentWeather(EWeatherType::Clear)
    , CurrentWeatherIntensity(1.0f)
    , CurrentTime(720.0f) // Start at noon
    , CurrentSeason(0)
{
}

void USEWeatherManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    if (GameInstance)
    {
        TimelineManager = GameInstance->GetTimelineManager();
    }

    // Start weather update loop
    FTimerHandle UpdateTimer;
    GetWorld()->GetTimerManager().SetTimer(
        UpdateTimer,
        this,
        &USEWeatherManager::UpdateWeatherEffects,
        WeatherUpdateInterval,
        true
    );

    // Start time of day update
    FTimerHandle TimeTimer;
    GetWorld()->GetTimerManager().SetTimer(
        TimeTimer,
        this,
        &USEWeatherManager::UpdateTimeOfDay,
        1.0f,
        true
    );
}

void USEWeatherManager::SetWeather(EWeatherType NewWeather, float Intensity)
{
    if (NewWeather == CurrentWeather && FMath::IsNearlyEqual(Intensity, CurrentWeatherIntensity))
    {
        return;
    }

    // FromSoftware-style: Weather changes affect gameplay significantly
    CurrentWeather = NewWeather;
    CurrentWeatherIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);

    // Load weather effect data
    UDataTable* WeatherTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WeatherEffects")));
    if (WeatherTable)
    {
        FWeatherEffect* Effect = WeatherTable->FindRow<FWeatherEffect>(*UEnum::GetValueAsString(NewWeather), "");
        if (Effect)
        {
            ApplyWeatherEffects(*Effect);
        }
    }

    // Notify weather change
    OnWeatherChanged.Broadcast(NewWeather, CurrentWeatherIntensity);
    BP_OnWeatherChanged(NewWeather, CurrentWeatherIntensity);
}

void USEWeatherManager::TransitionWeather(EWeatherType TargetWeather, float TransitionTime)
{
    // FromSoftware-style: Weather transitions are dramatic and affect visibility
    FTimerHandle TransitionTimer;
    float StartIntensity = CurrentWeatherIntensity;
    float ElapsedTime = 0.0f;

    auto TransitionUpdate = [this, TargetWeather, TransitionTime, StartIntensity, &ElapsedTime]()
    {
        ElapsedTime += WeatherUpdateInterval;
        float Alpha = FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f);
        float NewIntensity = FMath::Lerp(StartIntensity, 1.0f, Alpha);

        if (Alpha >= 1.0f)
        {
            SetWeather(TargetWeather, 1.0f);
            GetWorld()->GetTimerManager().ClearTimer(TransitionTimer);
        }
        else
        {
            SetWeather(CurrentWeather, NewIntensity);
        }
    };

    GetWorld()->GetTimerManager().SetTimer(
        TransitionTimer,
        TransitionUpdate,
        WeatherUpdateInterval,
        true
    );
}

void USEWeatherManager::SpawnEnvironmentalHazard(const FEnvironmentalHazard& Hazard, const FVector& Location)
{
    if (!ShouldSpawnHazard(Hazard))
    {
        return;
    }

    // FromSoftware-style: Environmental hazards are deadly and require strategy
    ActiveHazards.Add(Hazard.HazardID, Hazard);

    // Apply immediate effects
    TArray<AActor*> OverlappingActors;
    UGameplayStatics::GetAllActorsInRadius(GetWorld(), Location, Hazard.Radius, OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        // Apply damage and status effects
        float DamageAmount = CalculateHazardDamage(Hazard);
        UGameplayStatics::ApplyDamage(Actor, DamageAmount, nullptr, nullptr, nullptr);

        // Apply status effects
        for (const FName& StatusEffect : Hazard.StatusEffects)
        {
            // Apply status effect to actor
        }
    }

    // Set up hazard persistence
    FTimerHandle HazardTimer;
    GetWorld()->GetTimerManager().SetTimer(
        HazardTimer,
        [this, Hazard]()
        {
            RemoveEnvironmentalHazard(Hazard.HazardID);
        },
        Hazard.Duration,
        false
    );

    // Notify hazard spawn
    OnEnvironmentalHazardSpawned.Broadcast(Hazard);
    BP_OnEnvironmentalHazardSpawned(Hazard);
}

void USEWeatherManager::RemoveEnvironmentalHazard(const FName& HazardID)
{
    ActiveHazards.Remove(HazardID);
}

void USEWeatherManager::SetTimeOfDay(float NewTime)
{
    CurrentTime = FMath::Fmod(NewTime, DayLength);
    bool bIsNight = IsNightTime();

    // FromSoftware-style: Time of day affects enemy behavior and difficulty
    OnTimeOfDayChanged.Broadcast(CurrentTime, bIsNight);
    BP_OnTimeOfDayChanged(CurrentTime, bIsNight);
}

bool USEWeatherManager::IsNightTime() const
{
    return (CurrentTime < 360.0f || CurrentTime > 1080.0f); // Night between 6 PM and 6 AM
}

void USEWeatherManager::AdvanceSeason()
{
    CurrentSeason = (CurrentSeason + 1) % 4;

    // FromSoftware-style: Seasons dramatically change the world
    BP_OnSeasonChanged(CurrentSeason);

    // Update weather probabilities based on season
    CheckWeatherTransitions();
}

void USEWeatherManager::OnTimelineStateChanged(ETimelineState NewState)
{
    // FromSoftware-style: Timeline affects weather patterns
    SpawnTimelineSpecificEffects(NewState);
    BP_OnTimelineStateChanged(NewState);
}

void USEWeatherManager::UpdateWeatherEffects()
{
    // Update active weather effects
    if (CurrentWeather != EWeatherType::Clear)
    {
        UDataTable* WeatherTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Data/DT_WeatherEffects")));
        if (WeatherTable)
        {
            FWeatherEffect* Effect = WeatherTable->FindRow<FWeatherEffect>(*UEnum::GetValueAsString(CurrentWeather), "");
            if (Effect)
            {
                ApplyWeatherEffects(*Effect);
            }
        }
    }

    // Process environmental hazards
    ProcessEnvironmentalHazards();

    // Check for weather transitions
    CheckWeatherTransitions();
}

void USEWeatherManager::ProcessEnvironmentalHazards()
{
    // FromSoftware-style: Environmental hazards require constant attention
    for (auto& Pair : ActiveHazards)
    {
        const FEnvironmentalHazard& Hazard = Pair.Value;
        float DamageAmount = CalculateHazardDamage(Hazard);

        // Apply periodic damage and effects
        // This would be handled by the gameplay effect system
    }
}

void USEWeatherManager::UpdateTimeOfDay()
{
    float PreviousTime = CurrentTime;
    CurrentTime = FMath::Fmod(CurrentTime + (TimeScale / DayLength), DayLength);

    bool bWasNight = (PreviousTime < 360.0f || PreviousTime > 1080.0f);
    bool bIsNight = IsNightTime();

    // FromSoftware-style: Day/night transitions affect enemy spawns and behavior
    if (bWasNight != bIsNight)
    {
        OnTimeOfDayChanged.Broadcast(CurrentTime, bIsNight);
        BP_OnTimeOfDayChanged(CurrentTime, bIsNight);
    }
}

void USEWeatherManager::CheckWeatherTransitions()
{
    // FromSoftware-style: Weather changes are meaningful and strategic
    if (!TimelineManager)
    {
        return;
    }

    ETimelineState CurrentTimeline = TimelineManager->GetCurrentState();
    float TransitionChance = FMath::FRand();

    // Complex weather transition logic based on:
    // - Current timeline
    // - Time of day
    // - Season
    // - Previous weather
    // This would determine the next weather state
}

void USEWeatherManager::ApplyWeatherEffects(const FWeatherEffect& Effect)
{
    // FromSoftware-style: Weather affects gameplay mechanics
    for (const auto& GameplayEffect : Effect.GameplayEffects)
    {
        // Apply effect based on intensity
        float EffectValue = GameplayEffect.Value * CurrentWeatherIntensity;

        // Apply to relevant systems (movement, combat, visibility, etc.)
    }

    // Spawn weather-specific encounters
    if (FMath::FRand() < CurrentWeatherIntensity)
    {
        for (const FName& EncounterID : Effect.SpecialEncounters)
        {
            // Spawn special encounter
        }
    }
}

void USEWeatherManager::SpawnTimelineSpecificEffects(ETimelineState State)
{
    // FromSoftware-style: Timeline-specific weather creates unique challenges
    switch (State)
    {
        case ETimelineState::DarkWorld:
            if (CurrentWeather == EWeatherType::Clear)
            {
                SetWeather(EWeatherType::ShadowMist, 0.5f);
            }
            break;

        case ETimelineState::BrightWorld:
            if (CurrentWeather == EWeatherType::Clear)
            {
                SetWeather(EWeatherType::RadiantGlow, 0.5f);
            }
            break;

        default:
            break;
    }
}

float USEWeatherManager::CalculateHazardDamage(const FEnvironmentalHazard& Hazard) const
{
    // FromSoftware-style: Environmental damage is significant and requires preparation
    float BaseDamage = Hazard.DamagePerSecond * WeatherUpdateInterval;

    // Apply modifiers based on:
    // - Current weather
    // - Time of day
    // - Timeline state
    // - Player resistances
    
    return BaseDamage;
}

bool USEWeatherManager::ShouldSpawnHazard(const FEnvironmentalHazard& Hazard) const
{
    // FromSoftware-style: Hazard spawning is strategic and meaningful
    if (!TimelineManager)
    {
        return false;
    }

    // Check timeline compatibility
    ETimelineState CurrentTimeline = TimelineManager->GetCurrentState();
    
    // Complex spawn logic based on:
    // - Current weather
    // - Time of day
    // - Active hazards
    // - Area state
    
    return true;
}
