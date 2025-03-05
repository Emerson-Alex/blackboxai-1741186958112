#include "SECharacterBase.h"
#include "Systems/SETimelineStateManager.h"
#include "Combat/CombatComponent.h"
#include "Combat/AbilityComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ASECharacterBase::ASECharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create components
    TimelineManager = CreateDefaultSubobject<USETimelineStateManager>(TEXT("TimelineManager"));
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
    AbilityComponent = CreateDefaultSubobject<UAbilityComponent>(TEXT("AbilityComponent"));

    // Initialize stats
    CurrentHealth = 100.0f;
    MaxHealth = 100.0f;
    LastTimelineState = ETimelineState::None;

    // Set network replication
    bReplicates = true;
}

void ASECharacterBase::BeginPlay()
{
    Super::BeginPlay();

    // Bind timeline state change handler
    if (TimelineManager)
    {
        TimelineManager->OnTimelineStateChanged.AddDynamic(this, &ASECharacterBase::HandleTimelineStateChanged);
    }

    // Initialize components
    if (CombatComponent)
    {
        CombatComponent->InitializeWithStats(BaseStats);
    }

    // Set initial health
    CurrentHealth = MaxHealth;
}

void ASECharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Process active effects
    ProcessTimelineEffects(DeltaTime);

    // Update visuals if needed
    if (GetCurrentTimelineState() != LastTimelineState)
    {
        UpdateCharacterVisuals();
        LastTimelineState = GetCurrentTimelineState();
    }
}

float ASECharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    // Apply timeline-specific modifiers
    ActualDamage *= CalculateDamageModifier();

    // Apply damage
    ApplyDamage(ActualDamage, DamageCauser);

    return ActualDamage;
}

bool ASECharacterBase::RequestTimelineTransition(ETimelineState NewState)
{
    if (!TimelineManager || !ValidateTimelineTransition(NewState))
    {
        return false;
    }

    // Check energy requirements
    if (!HasRequiredEnergy(NewState))
    {
        return false;
    }

    // Attempt transition
    if (TimelineManager->RequestTimelineTransition(NewState))
    {
        PlayTimelineTransitionEffects();
        return true;
    }

    return false;
}

ETimelineState ASECharacterBase::GetCurrentTimelineState() const
{
    return TimelineManager ? TimelineManager->GetCurrentState() : ETimelineState::None;
}

float ASECharacterBase::GetTimelineEnergy() const
{
    return TimelineManager ? TimelineManager->GetTimelineEnergy() : 0.0f;
}

void ASECharacterBase::InitializeStats(const FCharacterStats& InitStats)
{
    BaseStats = InitStats;
    MaxHealth = BaseStats.MaxHealth;
    CurrentHealth = MaxHealth;

    if (CombatComponent)
    {
        CombatComponent->InitializeWithStats(BaseStats);
    }

    RecalculateStats();
}

void ASECharacterBase::ApplyDamage(float DamageAmount, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f)
    {
        return;
    }

    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

    // Notify of health change
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    // Check for death
    if (CurrentHealth <= 0.0f && PreviousHealth > 0.0f)
    {
        HandleDeath();
    }
}

void ASECharacterBase::ApplyHealing(float HealAmount)
{
    if (CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualHeal = HealAmount * CalculateHealingModifier();
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + ActualHeal);
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float ASECharacterBase::GetHealthPercent() const
{
    return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

bool ASECharacterBase::ActivateAbility(const FName& AbilityName)
{
    if (!AbilityComponent)
    {
        return false;
    }

    // Check timeline state requirements
    const FAbilityData* AbilityData = AbilityComponent->GetAbilityData(AbilityName);
    if (!AbilityData || (AbilityData->RequiredState != ETimelineState::None && 
                        AbilityData->RequiredState != GetCurrentTimelineState()))
    {
        return false;
    }

    return AbilityComponent->ActivateAbility(AbilityName);
}

void ASECharacterBase::LearnAbility(const FAbilityData& AbilityData)
{
    if (AbilityComponent)
    {
        AbilityComponent->LearnAbility(AbilityData);
    }
}

bool ASECharacterBase::ApplyTimelineEffect(const FTimelineEffect& Effect)
{
    if (!ValidateTimelineTransition(Effect.RequiredState))
    {
        return false;
    }

    // Add to active effects
    FActiveEffect NewEffect;
    NewEffect.Effect = Effect;
    NewEffect.RemainingDuration = Effect.Duration;
    ActiveEffects.Add(NewEffect);

    // Apply immediate effect
    ApplyEffectModifiers(NewEffect);

    return true;
}

void ASECharacterBase::RemoveTimelineEffect(const FString& EffectName)
{
    ActiveEffects.RemoveAll([EffectName](const FActiveEffect& Effect) {
        return Effect.Effect.EffectName == EffectName;
    });

    // Recalculate stats after removing effect
    RecalculateStats();
}

void ASECharacterBase::HandleTimelineStateChanged(ETimelineState NewState, ETimelineState OldState)
{
    // Update visuals
    UpdateTimelineVisuals();

    // Notify listeners
    OnTimelineStateChanged.Broadcast(NewState, OldState);

    // Update stats for new state
    RecalculateStats();
}

void ASECharacterBase::UpdateCharacterVisuals()
{
    // Update material parameters
    ETimelineState CurrentState = GetCurrentTimelineState();
    if (USkeletalMeshComponent* Mesh = GetMesh())
    {
        for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
        {
            if (UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(i)))
            {
                // Update timeline-specific parameters
                DynMaterial->SetScalarParameterValue(TEXT("TimelineState"), static_cast<float>(CurrentState));
            }
        }
    }
}

void ASECharacterBase::HandleDeath()
{
    // Notify listeners
    OnCharacterDeath.Broadcast();

    // Disable input and collision
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Play death animation/effects
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(DeathMontage);
    }
}

void ASECharacterBase::OnRep_CurrentHealth()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void ASECharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASECharacterBase, CurrentHealth);
    DOREPLIFETIME(ASECharacterBase, MaxHealth);
}

void ASECharacterBase::ProcessTimelineEffects(float DeltaTime)
{
    bool bNeedsRecalculation = false;

    // Update effect durations and remove expired ones
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
    {
        FActiveEffect& Effect = ActiveEffects[i];
        Effect.RemainingDuration -= DeltaTime;

        if (Effect.RemainingDuration <= 0.0f)
        {
            ActiveEffects.RemoveAt(i);
            bNeedsRecalculation = true;
        }
    }

    if (bNeedsRecalculation)
    {
        RecalculateStats();
    }
}

float ASECharacterBase::CalculateDamageModifier() const
{
    float Modifier = 1.0f;

    // Apply timeline state modifiers
    switch (GetCurrentTimelineState())
    {
        case ETimelineState::Light:
            Modifier *= 1.0f + BaseStats.LightDamageBonus;
            break;
        case ETimelineState::Dark:
            Modifier *= 1.0f + BaseStats.DarkDamageBonus;
            break;
        default:
            break;
    }

    // Apply active effect modifiers
    for (const FActiveEffect& Effect : ActiveEffects)
    {
        if (Effect.Effect.RequiredState == GetCurrentTimelineState() ||
            Effect.Effect.RequiredState == ETimelineState::None)
        {
            Modifier *= Effect.Effect.Power;
        }
    }

    return Modifier;
}

float ASECharacterBase::CalculateHealingModifier() const
{
    float Modifier = 1.0f;

    // Timeline-specific healing modifiers
    if (GetCurrentTimelineState() == ETimelineState::Light)
    {
        Modifier *= 1.25f; // Light timeline healing bonus
    }

    // Apply effect modifiers
    for (const FActiveEffect& Effect : ActiveEffects)
    {
        if (Effect.Effect.RequiredState == GetCurrentTimelineState() ||
            Effect.Effect.RequiredState == ETimelineState::None)
        {
            if (Effect.Effect.EffectName.Contains(TEXT("Heal")))
            {
                Modifier *= Effect.Effect.Power;
            }
        }
    }

    return Modifier;
}

bool ASECharacterBase::IsVulnerableToState(ETimelineState State) const
{
    ETimelineState CurrentState = GetCurrentTimelineState();
    
    // Light is vulnerable to Dark and vice versa
    return (CurrentState == ETimelineState::Light && State == ETimelineState::Dark) ||
           (CurrentState == ETimelineState::Dark && State == ETimelineState::Light);
}

void ASECharacterBase::PlayTimelineTransitionEffects()
{
    // Spawn transition VFX
    if (TimelineTransitionVFX)
    {
        UGameplayStatics::SpawnEmitterAttached(
            TimelineTransitionVFX,
            GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget
        );
    }

    // Play transition sound
    if (TimelineTransitionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            TimelineTransitionSound,
            GetActorLocation()
        );
    }
}

void ASECharacterBase::UpdateTimelineVisuals()
{
    // Update character materials based on timeline state
    if (USkeletalMeshComponent* Mesh = GetMesh())
    {
        for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
        {
            if (UMaterialInstanceDynamic* DynMaterial = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(i)))
            {
                float TimelineBlend = (GetCurrentTimelineState() == ETimelineState::Dark) ? 1.0f : 0.0f;
                DynMaterial->SetScalarParameterValue(TEXT("TimelineBlend"), TimelineBlend);
            }
        }
    }
}

bool ASECharacterBase::ValidateTimelineTransition(ETimelineState NewState) const
{
    if (!TimelineManager)
    {
        return false;
    }

    if (NewState == ETimelineState::None || NewState == GetCurrentTimelineState())
    {
        return false;
    }

    return true;
}

bool ASECharacterBase::HasRequiredEnergy(ETimelineState NewState) const
{
    if (!TimelineManager)
    {
        return false;
    }

    float RequiredEnergy = TimelineManager->GetTimelineStats().TransitionEnergyCost;
    return GetTimelineEnergy() >= RequiredEnergy;
}

void ASECharacterBase::RecalculateStats()
{
    if (!CombatComponent)
    {
        return;
    }

    // Start with base stats
    FCharacterStats ModifiedStats = BaseStats;

    // Apply timeline state modifiers
    switch (GetCurrentTimelineState())
    {
        case ETimelineState::Light:
            ModifiedStats.AttackPower *= (1.0f + BaseStats.LightDamageBonus);
            ModifiedStats.Defense *= 1.15f;
            break;
        case ETimelineState::Dark:
            ModifiedStats.AttackPower *= (1.0f + BaseStats.DarkDamageBonus);
            ModifiedStats.CriticalChance += 0.1f;
            break;
        default:
            break;
    }

    // Apply active effect modifiers
    for (const FActiveEffect& Effect : ActiveEffects)
    {
        if (Effect.Effect.RequiredState == GetCurrentTimelineState() ||
            Effect.Effect.RequiredState == ETimelineState::None)
        {
            ApplyEffectModifiers(Effect);
        }
    }

    // Update combat component with modified stats
    CombatComponent->UpdateStats(ModifiedStats);
}

float ASECharacterBase::GetTotalStatModifier(EStatType StatType) const
{
    float Modifier = 1.0f;

    // Apply active effect modifiers for the specific stat
    for (const FActiveEffect& Effect : ActiveEffects)
    {
        if (Effect.Effect.RequiredState == GetCurrentTimelineState() ||
            Effect.Effect.RequiredState == ETimelineState::None)
        {
            // Apply stat-specific modifiers
            switch (StatType)
            {
                case EStatType::AttackPower:
                    if (Effect.Effect.EffectName.Contains(TEXT("Attack")))
                    {
                        Modifier *= Effect.Effect.Power;
                    }
                    break;
                case EStatType::Defense:
                    if (Effect.Effect.EffectName.Contains(TEXT("Defense")))
                    {
                        Modifier *= Effect.Effect.Power;
                    }
                    break;
                // Add other stat types as needed
            }
        }
    }

    return Modifier;
}

void ASECharacterBase::CleanupExpiredEffects()
{
    ActiveEffects.RemoveAll([](const FActiveEffect& Effect) {
        return Effect.RemainingDuration <= 0.0f;
    });
}

void ASECharacterBase::ApplyEffectModifiers(const FActiveEffect& Effect)
{
    // Apply effect-specific stat modifications
    if (Effect.Effect.EffectName.Contains(TEXT("Attack")))
    {
        BaseStats.AttackPower *= Effect.Effect.Power;
    }
    else if (Effect.Effect.EffectName.Contains(TEXT("Defense")))
    {
        BaseStats.Defense *= Effect.Effect.Power;
    }
    // Add other effect types as needed
}
