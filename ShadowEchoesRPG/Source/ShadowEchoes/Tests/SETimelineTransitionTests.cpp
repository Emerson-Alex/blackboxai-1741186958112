#include "SETimelineTransitionTests.h"
#include "Characters/SECharacterBase.h"
#include "Combat/CombatComponent.h"
#include "Combat/AbilityComponent.h"
#include "Systems/SETimelineStateManager.h"
#include "Systems/SETimelineTransitionSystem.h"
#include "Systems/SETransitionAnimationSystem.h"
#include "Systems/SETransitionEffectLoader.h"
#include "UI/Widgets/TimelineIndicatorWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/AutomationTest.h"

// Test Constants
const float FTimelineTransitionTests::TestEnergyAmount = 100.0f;
const float FTimelineTransitionTests::TestTransitionCost = 25.0f;
const float FTimelineTransitionTests::TestDamageAmount = 50.0f;
const float FTimelineTransitionTests::TestTimelineBonus = 1.25f;
const float FTimelineTransitionTests::TestTransitionDuration = 1.0f;

bool FTimelineTransitionTests::RunAllTests()
{
    bool bAllTestsPassed = true;

    SetupTestEnvironment();

    bAllTestsPassed &= TestTimelineStateTransitions();
    bAllTestsPassed &= TestTimelineEnergyManagement();
    bAllTestsPassed &= TestCombatTimelineEffects();
    bAllTestsPassed &= TestAbilityTimelineInteractions();
    bAllTestsPassed &= TestTransitionAnimations();
    bAllTestsPassed &= TestTransitionEffects();
    bAllTestsPassed &= TestTimelineUI();

    CleanupTestEnvironment();

    return bAllTestsPassed;
}

bool FTimelineTransitionTests::TestTimelineStateTransitions()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    // Test initial state
    bTestPassed &= ValidateTimelineState(TestCharacter, ETimelineState::None);

    // Test transition to Light state
    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (TimelineManager)
    {
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        // Wait for transition
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateTimelineState(TestCharacter, ETimelineState::Light);

        // Test transition to Dark state
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateTimelineState(TestCharacter, ETimelineState::Dark);
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestTimelineEnergyManagement()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (TimelineManager)
    {
        // Test initial energy
        TimelineManager->SetTimelineEnergy(TestEnergyAmount);
        bTestPassed &= ValidateTimelineEnergy(TestCharacter, TestEnergyAmount);

        // Test energy consumption during transition
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateTimelineEnergy(TestCharacter, TestEnergyAmount - TestTransitionCost);

        // Test energy regeneration
        FPlatformProcess::Sleep(1.0f); // Wait for energy regen tick
        float CurrentEnergy = TimelineManager->GetTimelineEnergy();
        bTestPassed &= (CurrentEnergy > (TestEnergyAmount - TestTransitionCost));
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestCombatTimelineEffects()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    UCombatComponent* Combat = GetCombatComponent(TestCharacter);
    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (Combat && TimelineManager)
    {
        // Test Light state combat modifiers
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateCombatModifiers(TestCharacter, ETimelineState::Light);

        // Test Dark state combat modifiers
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateCombatModifiers(TestCharacter, ETimelineState::Dark);
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestAbilityTimelineInteractions()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    UAbilityComponent* Ability = GetAbilityComponent(TestCharacter);
    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (Ability && TimelineManager)
    {
        // Test Light state ability availability
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateAbilityAvailability(TestCharacter, ETimelineState::Light);

        // Test Dark state ability availability
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= ValidateAbilityAvailability(TestCharacter, ETimelineState::Dark);
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestTransitionAnimations()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    USETransitionAnimationSystem* AnimSystem = GetAnimationSystem(TestCharacter);
    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (AnimSystem && TimelineManager)
    {
        // Test transition animation from None to Light
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        bTestPassed &= AnimSystem->IsAnimating();
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= !AnimSystem->IsAnimating();

        // Test transition animation from Light to Dark
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        bTestPassed &= AnimSystem->IsAnimating();
        FPlatformProcess::Sleep(TestTransitionDuration);
        bTestPassed &= !AnimSystem->IsAnimating();
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestTransitionEffects()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (TimelineManager)
    {
        // Test transition effects for Light state
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        bTestPassed &= ValidateTransitionEffects(TestCharacter);
        FPlatformProcess::Sleep(TestTransitionDuration);

        // Test transition effects for Dark state
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        bTestPassed &= ValidateTransitionEffects(TestCharacter);
        FPlatformProcess::Sleep(TestTransitionDuration);
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

bool FTimelineTransitionTests::TestTimelineUI()
{
    ASECharacterBase* TestCharacter = CreateTestCharacter();
    if (!TestCharacter)
    {
        return false;
    }

    bool bTestPassed = true;

    USETimelineStateManager* TimelineManager = GetTimelineManager(TestCharacter);
    if (TimelineManager)
    {
        // Test UI updates for Light state
        TimelineManager->RequestTimelineTransition(ETimelineState::Light);
        bTestPassed &= ValidateUIState(TestCharacter);
        FPlatformProcess::Sleep(TestTransitionDuration);

        // Test UI updates for Dark state
        TimelineManager->RequestTimelineTransition(ETimelineState::Dark);
        bTestPassed &= ValidateUIState(TestCharacter);
        FPlatformProcess::Sleep(TestTransitionDuration);
    }

    TestCharacter->Destroy();
    return bTestPassed;
}

ASECharacterBase* FTimelineTransitionTests::CreateTestCharacter()
{
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return World->SpawnActor<ASECharacterBase>(ASECharacterBase::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}

void FTimelineTransitionTests::SetupTestEnvironment()
{
    // Setup test environment if needed
}

void FTimelineTransitionTests::CleanupTestEnvironment()
{
    // Cleanup test environment if needed
}

bool FTimelineTransitionTests::ValidateTimelineState(ASECharacterBase* Character, ETimelineState ExpectedState)
{
    if (!Character)
    {
        return false;
    }

    USETimelineStateManager* TimelineManager = GetTimelineManager(Character);
    return TimelineManager && TimelineManager->GetCurrentState() == ExpectedState;
}

bool FTimelineTransitionTests::ValidateTransitionEffects(ASECharacterBase* Character)
{
    if (!Character)
    {
        return false;
    }

    USETransitionEffectLoader* EffectLoader = GetEffectLoader(Character);
    return EffectLoader && EffectLoader->HasEffectsLoaded(Character->GetTimelineState());
}

bool FTimelineTransitionTests::ValidateTimelineEnergy(ASECharacterBase* Character, float ExpectedEnergy)
{
    if (!Character)
    {
        return false;
    }

    USETimelineStateManager* TimelineManager = GetTimelineManager(Character);
    return TimelineManager && FMath::IsNearlyEqual(TimelineManager->GetTimelineEnergy(), ExpectedEnergy, 0.01f);
}

bool FTimelineTransitionTests::ValidateCombatModifiers(ASECharacterBase* Character, ETimelineState State)
{
    if (!Character)
    {
        return false;
    }

    UCombatComponent* Combat = GetCombatComponent(Character);
    if (!Combat)
    {
        return false;
    }

    // Validate combat modifiers based on timeline state
    float ExpectedModifier = (State == ETimelineState::Light) ? TestTimelineBonus : (2.0f - TestTimelineBonus);
    return FMath::IsNearlyEqual(Combat->GetTimelineAttackModifier(), ExpectedModifier, 0.01f);
}

bool FTimelineTransitionTests::ValidateAbilityAvailability(ASECharacterBase* Character, ETimelineState State)
{
    if (!Character)
    {
        return false;
    }

    UAbilityComponent* Ability = GetAbilityComponent(Character);
    if (!Ability)
    {
        return false;
    }

    // Get abilities available in current state
    TArray<FName> AvailableAbilities = Ability->GetAvailableAbilities(State);
    
    // Validate that state-specific abilities are available
    return AvailableAbilities.Num() > 0;
}

bool FTimelineTransitionTests::ValidateUIState(ASECharacterBase* Character)
{
    if (!Character)
    {
        return false;
    }

    APlayerController* PC = Cast<APlayerController>(Character->GetController());
    if (!PC)
    {
        return false;
    }

    UTimelineIndicatorWidget* TimelineWidget = Cast<UTimelineIndicatorWidget>(PC->GetHUD()->GetWidget(UTimelineIndicatorWidget::StaticClass()));
    return TimelineWidget != nullptr;
}

// Component Getters
UCombatComponent* FTimelineTransitionTests::GetCombatComponent(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<UCombatComponent>() : nullptr;
}

UAbilityComponent* FTimelineTransitionTests::GetAbilityComponent(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<UAbilityComponent>() : nullptr;
}

USETimelineStateManager* FTimelineTransitionTests::GetTimelineManager(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<USETimelineStateManager>() : nullptr;
}

USETimelineTransitionSystem* FTimelineTransitionTests::GetTransitionSystem(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<USETimelineTransitionSystem>() : nullptr;
}

USETransitionAnimationSystem* FTimelineTransitionTests::GetAnimationSystem(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<USETransitionAnimationSystem>() : nullptr;
}

USETransitionEffectLoader* FTimelineTransitionTests::GetEffectLoader(ASECharacterBase* Character)
{
    return Character ? Character->FindComponentByClass<USETransitionEffectLoader>() : nullptr;
}

// Automation Test Implementations
bool FTimelineStateTransitionTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::RunAllTests();
}

bool FTimelineEnergyTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestTimelineEnergyManagement();
}

bool FTimelineCombatTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestCombatTimelineEffects();
}

bool FTimelineAbilityTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestAbilityTimelineInteractions();
}

bool FTimelineAnimationTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestTransitionAnimations();
}

bool FTimelineEffectTest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestTransitionEffects();
}

bool FTimelineUITest::RunTest(const FString& Parameters)
{
    return FTimelineTransitionTests::TestTimelineUI();
}
