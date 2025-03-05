// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "SEInputConfig.generated.h"

/**
 * FSEInputAction - Struct to hold input action data with gameplay tags
 */
USTRUCT(BlueprintType)
struct FSEInputAction
{
    GENERATED_BODY()

    /** The input action */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAction = nullptr;

    /** Gameplay tag associated with this input */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "Input"))
    FGameplayTag InputTag;
};

/**
 * USEInputConfig - Data asset to configure input actions with gameplay tags
 */
UCLASS()
class SHADOWECHOES_API USEInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Array of input actions with their associated gameplay tags */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FSEInputAction> InputActions;

    /** Get input action from gameplay tag */
    const UInputAction* FindInputActionForTag(const FGameplayTag& InputTag) const;

    /** Get gameplay tag from input action */
    FGameplayTag FindTagForInputAction(const UInputAction* InputAction) const;
};

/**
 * FSEInputBinding - Struct to hold input binding data
 */
USTRUCT(BlueprintType)
struct FSEInputBinding
{
    GENERATED_BODY()

    /** The input action to bind */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> InputAction = nullptr;

    /** The function to call when input is triggered */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName FunctionName;

    /** Input event trigger type */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TEnumAsByte<EInputEvent> TriggerEvent;

    FSEInputBinding()
        : TriggerEvent(IE_Pressed)
    {}
};

/**
 * USEInputComponent - Enhanced input component with additional functionality
 */
UCLASS()
class SHADOWECHOES_API USEInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    /** Add input binding with gameplay tag */
    template<class UserClass, typename FuncType>
    void BindActionByTag(const USEInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
    {
        if (const UInputAction* IA = InputConfig->FindInputActionForTag(InputTag))
        {
            BindAction(IA, TriggerEvent, Object, Func);
        }
    }

    /** Add input bindings from config */
    void AddInputBindings(const USEInputConfig* InputConfig, const TArray<FSEInputBinding>& Bindings, UObject* BindingObject);
};
