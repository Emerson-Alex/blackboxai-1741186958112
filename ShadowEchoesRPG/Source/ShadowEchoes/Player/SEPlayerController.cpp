// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SEPlayerController.h"
#include "Input/SEInputConfig.h"
#include "Characters/SECharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

const FName ASEPlayerController::LogCategory = TEXT("ShadowEchoes.PlayerController");

ASEPlayerController::ASEPlayerController()
    : bIsInUIMode(false)
    , bIsInCombatMode(false)
{
    // Enable tick
    PrimaryActorTick.bCanEverTick = true;

    // Default enhanced input settings
    bEnableMouseOverEvents = true;
    bEnableTouchOverEvents = true;
    bShowMouseCursor = false;
}

void ASEPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Cache controlled character
    CacheControlledCharacter();

    // Setup enhanced input
    SetupEnhancedInput();

    // Start with default context
    if (DefaultMappingContext)
    {
        AddInputMappingContext(DefaultMappingContext, DefaultContextPriority);
    }
}

void ASEPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    SetupInputBindings();
}

void ASEPlayerController::SetupEnhancedInput()
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // Clear any existing mappings
        Subsystem->ClearAllMappings();

        // Add default mapping context
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, DefaultContextPriority);
        }
    }
}

void ASEPlayerController::AddInputMappingContext(UInputMappingContext* Context, int32 Priority)
{
    if (Context)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(Context, Priority);
        }
    }
}

void ASEPlayerController::RemoveInputMappingContext(UInputMappingContext* Context)
{
    if (Context)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(Context);
        }
    }
}

void ASEPlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    const FVector2D Value = InputActionValue.Get<FVector2D>();
    
    // Forward/Backward movement
    if (Value.Y != 0.0f)
    {
        const FRotator Rotation = GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        ControlledCharacter->AddMovementInput(Direction, Value.Y);
    }

    // Right/Left movement
    if (Value.X != 0.0f)
    {
        const FRotator Rotation = GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        ControlledCharacter->AddMovementInput(Direction, Value.X);
    }
}

void ASEPlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    const FVector2D Value = InputActionValue.Get<FVector2D>();
    
    AddYawInput(Value.X);
    AddPitchInput(Value.Y);
}

void ASEPlayerController::Input_Jump(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    ControlledCharacter->Jump();
}

void ASEPlayerController::Input_SwitchTimeline(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    ControlledCharacter->SwitchTimelineAbilities(
        ControlledCharacter->GetCurrentTimeline() == ETimelineState::BrightWorld ? 
        ETimelineState::DarkWorld : ETimelineState::BrightWorld
    );
}

void ASEPlayerController::Input_LightAttack(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    if (!bIsInCombatMode)
    {
        EnterCombatMode();
    }

    ControlledCharacter->ExecuteAbility(TEXT("LightAttack"));
}

void ASEPlayerController::Input_HeavyAttack(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    if (!bIsInCombatMode)
    {
        EnterCombatMode();
    }

    ControlledCharacter->ExecuteAbility(TEXT("HeavyAttack"));
}

void ASEPlayerController::Input_Dodge(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    // Calculate dodge direction based on movement input
    FVector DodgeDirection = FVector::ZeroVector;
    if (UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement())
    {
        DodgeDirection = MovementComponent->GetLastInputVector();
        if (DodgeDirection.IsNearlyZero())
        {
            DodgeDirection = -ControlledCharacter->GetActorForwardVector();
        }
    }

    if (UCombatComponent* Combat = ControlledCharacter->FindComponentByClass<UCombatComponent>())
    {
        Combat->ExecuteDodge(DodgeDirection);
    }
}

void ASEPlayerController::Input_Parry(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    if (UCombatComponent* Combat = ControlledCharacter->FindComponentByClass<UCombatComponent>())
    {
        Combat->AttemptParry();
    }
}

void ASEPlayerController::Input_Ability(const FInputActionValue& InputActionValue, int32 AbilityIndex)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    FString AbilityName = FString::Printf(TEXT("Ability_%d"), AbilityIndex);
    ControlledCharacter->ExecuteAbility(FName(*AbilityName));
}

void ASEPlayerController::Input_ToggleInventory(const FInputActionValue& InputActionValue)
{
    SetUIMode(!bIsInUIMode);
    // TODO: Show/hide inventory UI
}

void ASEPlayerController::Input_ToggleQuestLog(const FInputActionValue& InputActionValue)
{
    SetUIMode(!bIsInUIMode);
    // TODO: Show/hide quest log UI
}

void ASEPlayerController::Input_ToggleMap(const FInputActionValue& InputActionValue)
{
    SetUIMode(!bIsInUIMode);
    // TODO: Show/hide map UI
}

void ASEPlayerController::Input_Interact(const FInputActionValue& InputActionValue)
{
    if (!ControlledCharacter || bIsInUIMode)
    {
        return;
    }

    // TODO: Implement interaction system
}

void ASEPlayerController::SetUIMode(bool bEnable)
{
    bIsInUIMode = bEnable;
    bShowMouseCursor = bEnable;

    if (bEnable)
    {
        SetInputMode(FInputModeGameAndUI());
        if (UIMappingContext)
        {
            AddInputMappingContext(UIMappingContext, UIContextPriority);
        }
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
        if (UIMappingContext)
        {
            RemoveInputMappingContext(UIMappingContext);
        }
    }
}

void ASEPlayerController::EnterCombatMode()
{
    if (bIsInCombatMode)
    {
        return;
    }

    bIsInCombatMode = true;
    if (CombatMappingContext)
    {
        AddInputMappingContext(CombatMappingContext, CombatContextPriority);
    }

    // Notify character of combat mode change
    if (ControlledCharacter)
    {
        // TODO: Implement combat mode change notification
    }
}

void ASEPlayerController::ExitCombatMode()
{
    if (!bIsInCombatMode)
    {
        return;
    }

    bIsInCombatMode = false;
    if (CombatMappingContext)
    {
        RemoveInputMappingContext(CombatMappingContext);
    }

    // Notify character of combat mode change
    if (ControlledCharacter)
    {
        // TODO: Implement combat mode change notification
    }
}

USEInputComponent* ASEPlayerController::GetSEInputComponent() const
{
    return Cast<USEInputComponent>(InputComponent);
}

void ASEPlayerController::SetupInputBindings()
{
    USEInputComponent* SEInput = GetSEInputComponent();
    if (!SEInput || !InputConfig)
    {
        return;
    }

    // Movement
    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Movement")), 
        ETriggerEvent::Triggered, this, &ASEPlayerController::Input_Move);

    // Look
    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Look")), 
        ETriggerEvent::Triggered, this, &ASEPlayerController::Input_Look);

    // Actions
    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Jump")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_Jump);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.SwitchTimeline")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_SwitchTimeline);

    // Combat
    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.LightAttack")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_LightAttack);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.HeavyAttack")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_HeavyAttack);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Dodge")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_Dodge);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Parry")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_Parry);

    // UI
    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Inventory")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_ToggleInventory);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.QuestLog")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_ToggleQuestLog);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Map")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_ToggleMap);

    SEInput->BindActionByTag(InputConfig, FGameplayTag::RequestGameplayTag(TEXT("Input.Interact")), 
        ETriggerEvent::Started, this, &ASEPlayerController::Input_Interact);
}

void ASEPlayerController::CacheControlledCharacter()
{
    ControlledCharacter = Cast<ASECharacterBase>(GetCharacter());
}
