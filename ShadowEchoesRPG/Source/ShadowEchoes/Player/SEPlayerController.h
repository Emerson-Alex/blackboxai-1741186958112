// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "SEPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class USEInputConfig;
class ASECharacterBase;

/**
 * ASEPlayerController - Handles player input and game control
 * Manages input mapping contexts and gameplay actions
 */
UCLASS()
class SHADOWECHOES_API ASEPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASEPlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

protected:
    /** Input config data asset */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<USEInputConfig> InputConfig;

    /** Default input mapping context */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    /** Combat input mapping context */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> CombatMappingContext;

    /** UI input mapping context */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> UIMappingContext;

    /** Input priority for different contexts */
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    float DefaultContextPriority = 0.f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    float CombatContextPriority = 1.f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    float UIContextPriority = 2.f;

    /** Cached reference to controlled character */
    UPROPERTY()
    TObjectPtr<ASECharacterBase> ControlledCharacter;

    /** Initialize enhanced input mappings */
    virtual void SetupEnhancedInput();

    /** Add input mapping context */
    void AddInputMappingContext(UInputMappingContext* Context, int32 Priority);

    /** Remove input mapping context */
    void RemoveInputMappingContext(UInputMappingContext* Context);

    /** Input Handlers */
    void Input_Move(const FInputActionValue& InputActionValue);
    void Input_Look(const FInputActionValue& InputActionValue);
    void Input_Jump(const FInputActionValue& InputActionValue);
    void Input_SwitchTimeline(const FInputActionValue& InputActionValue);
    void Input_LightAttack(const FInputActionValue& InputActionValue);
    void Input_HeavyAttack(const FInputActionValue& InputActionValue);
    void Input_Dodge(const FInputActionValue& InputActionValue);
    void Input_Parry(const FInputActionValue& InputActionValue);
    void Input_Ability(const FInputActionValue& InputActionValue, int32 AbilityIndex);
    void Input_ToggleInventory(const FInputActionValue& InputActionValue);
    void Input_ToggleQuestLog(const FInputActionValue& InputActionValue);
    void Input_ToggleMap(const FInputActionValue& InputActionValue);
    void Input_Interact(const FInputActionValue& InputActionValue);

    /** UI State Management */
    void SetUIMode(bool bEnable);
    bool IsInUIMode() const { return bIsInUIMode; }

    /** Combat State Management */
    void EnterCombatMode();
    void ExitCombatMode();
    bool IsInCombatMode() const { return bIsInCombatMode; }

private:
    /** Whether UI mode is active */
    bool bIsInUIMode;

    /** Whether combat mode is active */
    bool bIsInCombatMode;

    /** Input component cast helper */
    class USEInputComponent* GetSEInputComponent() const;

    /** Setup input bindings */
    void SetupInputBindings();

    /** Cache controlled character reference */
    void CacheControlledCharacter();

    /** Logger category */
    static const FName LogCategory;
};
