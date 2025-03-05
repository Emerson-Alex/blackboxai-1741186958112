#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/SETimelineTypes.h"

class ASECharacterBase;
class UCombatComponent;
class UAbilityComponent;
class USETimelineStateManager;
class USETimelineTransitionSystem;
class USETransitionAnimationSystem;
class USETransitionEffectLoader;

/**
 * Test suite for Timeline transition system functionality
 */
class FTimelineTransitionTests
{
public:
    static bool RunAllTests();

private:
    // Test Cases
    static bool TestTimelineStateTransitions();
    static bool TestTimelineEnergyManagement();
    static bool TestCombatTimelineEffects();
    static bool TestAbilityTimelineInteractions();
    static bool TestTransitionAnimations();
    static bool TestTransitionEffects();
    static bool TestTimelineUI();

    // Helper Methods
    static ASECharacterBase* CreateTestCharacter();
    static void SetupTestEnvironment();
    static void CleanupTestEnvironment();
    static bool ValidateTimelineState(ASECharacterBase* Character, ETimelineState ExpectedState);
    static bool ValidateTransitionEffects(ASECharacterBase* Character);
    static bool ValidateTimelineEnergy(ASECharacterBase* Character, float ExpectedEnergy);
    static bool ValidateCombatModifiers(ASECharacterBase* Character, ETimelineState State);
    static bool ValidateAbilityAvailability(ASECharacterBase* Character, ETimelineState State);
    static bool ValidateUIState(ASECharacterBase* Character);

    // Test Components
    static UCombatComponent* GetCombatComponent(ASECharacterBase* Character);
    static UAbilityComponent* GetAbilityComponent(ASECharacterBase* Character);
    static USETimelineStateManager* GetTimelineManager(ASECharacterBase* Character);
    static USETimelineTransitionSystem* GetTransitionSystem(ASECharacterBase* Character);
    static USETransitionAnimationSystem* GetAnimationSystem(ASECharacterBase* Character);
    static USETransitionEffectLoader* GetEffectLoader(ASECharacterBase* Character);

    // Test Data
    static const float TestEnergyAmount;
    static const float TestTransitionCost;
    static const float TestDamageAmount;
    static const float TestTimelineBonus;
    static const float TestTransitionDuration;
};

// Automation Test Specs
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineStateTransitionTest, "ShadowEchoes.Timeline.StateTransitions", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineEnergyTest, "ShadowEchoes.Timeline.EnergyManagement", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineCombatTest, "ShadowEchoes.Timeline.CombatEffects", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineAbilityTest, "ShadowEchoes.Timeline.AbilityInteractions", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineAnimationTest, "ShadowEchoes.Timeline.TransitionAnimations", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineEffectTest, "ShadowEchoes.Timeline.TransitionEffects", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelineUITest, "ShadowEchoes.Timeline.UIUpdates", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
