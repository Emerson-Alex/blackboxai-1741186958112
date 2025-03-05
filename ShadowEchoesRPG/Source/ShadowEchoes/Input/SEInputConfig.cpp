// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SEInputConfig.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "GameFramework/Actor.h"

const UInputAction* USEInputConfig::FindInputActionForTag(const FGameplayTag& InputTag) const
{
    for (const FSEInputAction& Action : InputActions)
    {
        if (Action.InputTag == InputTag)
        {
            return Action.InputAction;
        }
    }
    return nullptr;
}

FGameplayTag USEInputConfig::FindTagForInputAction(const UInputAction* InputAction) const
{
    for (const FSEInputAction& Action : InputActions)
    {
        if (Action.InputAction == InputAction)
        {
            return Action.InputTag;
        }
    }
    return FGameplayTag();
}

void USEInputComponent::AddInputBindings(
    const USEInputConfig* InputConfig,
    const TArray<FSEInputBinding>& Bindings,
    UObject* BindingObject)
{
    if (!InputConfig || !BindingObject)
    {
        return;
    }

    for (const FSEInputBinding& Binding : Bindings)
    {
        if (!Binding.InputAction || Binding.FunctionName.IsNone())
        {
            continue;
        }

        // Find the function to bind
        UFunction* Function = BindingObject->FindFunction(Binding.FunctionName);
        if (!Function)
        {
            continue;
        }

        // Determine the appropriate binding based on the function signature
        if (Function->NumParms == 0)
        {
            // Simple trigger binding
            BindAction(Binding.InputAction, static_cast<ETriggerEvent>(Binding.TriggerEvent),
                BindingObject, Binding.FunctionName);
        }
        else
        {
            // Value-based binding (for axis inputs)
            BindAction(Binding.InputAction, static_cast<ETriggerEvent>(Binding.TriggerEvent),
                BindingObject, Binding.FunctionName);
        }
    }
}

// Example input contexts that will be created as Blueprint assets
/*
[Input Context: Default]
Mappings:
- Action: IA_Move
  Key: W
  Modifiers: None
  Triggers: Ongoing
- Action: IA_Move
  Key: S
  Modifiers: None
  Triggers: Ongoing
- Action: IA_Move
  Key: A
  Modifiers: None
  Triggers: Ongoing
- Action: IA_Move
  Key: D
  Modifiers: None
  Triggers: Ongoing
- Action: IA_Look
  Key: Mouse XY
  Modifiers: None
  Triggers: Ongoing
- Action: IA_Jump
  Key: Space
  Modifiers: None
  Triggers: Pressed
- Action: IA_SwitchTimeline
  Key: Tab
  Modifiers: None
  Triggers: Pressed

[Input Context: Combat]
Mappings:
- Action: IA_LightAttack
  Key: Left Mouse Button
  Modifiers: None
  Triggers: Pressed
- Action: IA_HeavyAttack
  Key: Right Mouse Button
  Modifiers: None
  Triggers: Pressed
- Action: IA_Dodge
  Key: Space
  Modifiers: None
  Triggers: Pressed
- Action: IA_Parry
  Key: Q
  Modifiers: None
  Triggers: Pressed
- Action: IA_Ability1
  Key: 1
  Modifiers: None
  Triggers: Pressed
- Action: IA_Ability2
  Key: 2
  Modifiers: None
  Triggers: Pressed
- Action: IA_Ability3
  Key: 3
  Modifiers: None
  Triggers: Pressed
- Action: IA_Ability4
  Key: 4
  Modifiers: None
  Triggers: Pressed

[Input Context: UI]
Mappings:
- Action: IA_Inventory
  Key: I
  Modifiers: None
  Triggers: Pressed
- Action: IA_QuestLog
  Key: J
  Modifiers: None
  Triggers: Pressed
- Action: IA_Map
  Key: M
  Modifiers: None
  Triggers: Pressed
- Action: IA_Interact
  Key: E
  Modifiers: None
  Triggers: Pressed
*/

// Example input action definitions that will be created as Blueprint assets
/*
[Input Actions]
IA_Move:
  ValueType: Vector2D
  Triggers: Ongoing
  Modifiers: None

IA_Look:
  ValueType: Vector2D
  Triggers: Ongoing
  Modifiers: None

IA_Jump:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_SwitchTimeline:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_LightAttack:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_HeavyAttack:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Dodge:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Parry:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Ability1:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Ability2:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Ability3:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Ability4:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Inventory:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_QuestLog:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Map:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None

IA_Interact:
  ValueType: Digital
  Triggers: Pressed
  Modifiers: None
*/
