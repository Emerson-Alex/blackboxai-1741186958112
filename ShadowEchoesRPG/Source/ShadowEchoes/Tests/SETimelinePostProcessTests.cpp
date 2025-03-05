#include "SETimelinePostProcessTests.h"
#include "Components/SETimelinePostProcessComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PostProcessComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelinePostProcessTest, "ShadowEchoes.Timeline.PostProcess", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTimelinePostProcessTest::RunTest(const FString& Parameters)
{
    // Create test actor
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World)
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    AActor* TestActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (!TestActor)
    {
        return false;
    }

    // Add post-process component
    USETimelinePostProcessComponent* PostProcessComponent = NewObject<USETimelinePostProcessComponent>(TestActor);
    TestActor->AddOwnedComponent(PostProcessComponent);
    PostProcessComponent->RegisterComponent();

    bool bTestsPassed = true;

    // Test initialization
    bTestsPassed &= TestInitialization(PostProcessComponent);

    // Test transition effects
    bTestsPassed &= TestTransitionEffects(PostProcessComponent);

    // Test quality settings
    bTestsPassed &= TestQualitySettings(PostProcessComponent);

    // Test performance monitoring
    bTestsPassed &= TestPerformanceMonitoring(PostProcessComponent);

    // Cleanup
    TestActor->Destroy();

    return bTestsPassed;
}

bool FTimelinePostProcessTest::TestInitialization(USETimelinePostProcessComponent* Component)
{
    if (!Component)
    {
        return false;
    }

    bool bTestPassed = true;

    // Verify post-process component creation
    UPostProcessComponent* PPComponent = Component->GetOwner()->FindComponentByClass<UPostProcessComponent>();
    bTestPassed &= TestTrue(TEXT("Post-process component exists"), PPComponent != nullptr);

    // Verify material parameter collection loading
    const UMaterialParameterCollection* MPC = Cast<UMaterialParameterCollection>(StaticLoadObject(
        UMaterialParameterCollection::StaticClass(),
        nullptr,
        TEXT("/Game/Materials/MPC_TimelineTransition")
    ));
    bTestPassed &= TestTrue(TEXT("Material parameter collection loaded"), MPC != nullptr);

    // Verify default parameters
    bTestPassed &= TestEqual(TEXT("Initial transition state"), Component->IsTransitioning(), false);
    bTestPassed &= TestEqual(TEXT("Initial transition time"), Component->GetCurrentTime(), 0.0f);

    return bTestPassed;
}

bool FTimelinePostProcessTest::TestTransitionEffects(USETimelinePostProcessComponent* Component)
{
    if (!Component)
    {
        return false;
    }

    bool bTestPassed = true;

    // Test starting transition
    Component->StartTransitionEffect(ETimelineState::Light, ETimelineState::Dark, 1.0f);
    bTestPassed &= TestTrue(TEXT("Transition started"), Component->IsTransitioning());

    // Test transition parameters
    float Progress = 0.5f;
    Component->UpdateEffectParameters(Progress);
    
    // Verify material parameter updates
    UMaterialInstanceDynamic* Material = Component->GetPostProcessMaterial();
    if (Material)
    {
        float CurrentProgress;
        Material->GetScalarParameterValue(FName("TransitionProgress"), CurrentProgress);
        bTestPassed &= TestEqual(TEXT("Transition progress updated"), CurrentProgress, Progress);
    }

    // Test stopping transition
    Component->StopTransitionEffect();
    bTestPassed &= TestFalse(TEXT("Transition stopped"), Component->IsTransitioning());

    return bTestPassed;
}

bool FTimelinePostProcessTest::TestQualitySettings(USETimelinePostProcessComponent* Component)
{
    if (!Component)
    {
        return false;
    }

    bool bTestPassed = true;

    // Test high quality settings
    Component->SetEffectQuality(2);
    bTestPassed &= TestEqual(TEXT("High quality level set"), Component->GetCurrentQualityLevel(), 2);

    // Test medium quality settings
    Component->SetEffectQuality(1);
    bTestPassed &= TestEqual(TEXT("Medium quality level set"), Component->GetCurrentQualityLevel(), 1);

    // Test low quality settings
    Component->SetEffectQuality(0);
    bTestPassed &= TestEqual(TEXT("Low quality level set"), Component->GetCurrentQualityLevel(), 0);

    // Test invalid quality level
    Component->SetEffectQuality(-1);
    bTestPassed &= TestNotEqual(TEXT("Invalid quality level rejected"), Component->GetCurrentQualityLevel(), -1);

    return bTestPassed;
}

bool FTimelinePostProcessTest::TestPerformanceMonitoring(USETimelinePostProcessComponent* Component)
{
    if (!Component)
    {
        return false;
    }

    bool bTestPassed = true;

    // Enable dynamic quality adjustment
    Component->EnableDynamicQuality(true);
    bTestPassed &= TestTrue(TEXT("Dynamic quality enabled"), Component->IsDynamicQualityEnabled());

    // Simulate performance updates
    for (int32 i = 0; i < 60; ++i)
    {
        Component->UpdatePerformanceMetrics(1.0f / 30.0f); // Simulate 30 FPS
    }

    // Verify quality adjustment
    bTestPassed &= TestTrue(TEXT("Quality adjusted for performance"), Component->GetCurrentQualityLevel() < 2);

    // Test performance metrics
    float AverageFrameTime = Component->GetAverageFrameTime();
    bTestPassed &= TestTrue(TEXT("Frame time tracked"), AverageFrameTime > 0.0f);

    return bTestPassed;
}

// Additional test implementations
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelinePostProcessQualityTest, "ShadowEchoes.Timeline.PostProcess.Quality", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTimelinePostProcessQualityTest::RunTest(const FString& Parameters)
{
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World)
    {
        return false;
    }

    AActor* TestActor = World->SpawnActor<AActor>();
    USETimelinePostProcessComponent* Component = NewObject<USETimelinePostProcessComponent>(TestActor);
    TestActor->AddOwnedComponent(Component);
    Component->RegisterComponent();

    bool bTestPassed = true;

    // Test quality presets
    for (int32 QualityLevel = 0; QualityLevel < 3; ++QualityLevel)
    {
        Component->SetEffectQuality(QualityLevel);
        
        // Start a transition
        Component->StartTransitionEffect(ETimelineState::Light, ETimelineState::Dark, 1.0f);
        
        // Verify effect parameters match quality level
        UMaterialInstanceDynamic* Material = Component->GetPostProcessMaterial();
        if (Material)
        {
            float NoiseScale, DistortionStrength;
            Material->GetScalarParameterValue(FName("NoiseScale"), NoiseScale);
            Material->GetScalarParameterValue(FName("DistortionStrength"), DistortionStrength);
            
            // Values should decrease with lower quality
            bTestPassed &= TestTrue(TEXT("Quality parameters scaled"), 
                (QualityLevel == 0 && NoiseScale < 1.0f) ||
                (QualityLevel == 1 && NoiseScale == 1.0f) ||
                (QualityLevel == 2 && NoiseScale > 1.0f)
            );
        }
        
        Component->StopTransitionEffect();
    }

    TestActor->Destroy();
    return bTestPassed;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTimelinePostProcessPerformanceTest, "ShadowEchoes.Timeline.PostProcess.Performance", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTimelinePostProcessPerformanceTest::RunTest(const FString& Parameters)
{
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World)
    {
        return false;
    }

    AActor* TestActor = World->SpawnActor<AActor>();
    USETimelinePostProcessComponent* Component = NewObject<USETimelinePostProcessComponent>(TestActor);
    TestActor->AddOwnedComponent(Component);
    Component->RegisterComponent();

    bool bTestPassed = true;

    // Enable dynamic quality adjustment
    Component->EnableDynamicQuality(true);

    // Test performance under different conditions
    struct FPerformanceScenario
    {
        float FrameTime;
        int32 ExpectedQualityLevel;
    };

    TArray<FPerformanceScenario> Scenarios = {
        {1.0f / 60.0f, 2},  // 60 FPS - High quality
        {1.0f / 30.0f, 1},  // 30 FPS - Medium quality
        {1.0f / 15.0f, 0}   // 15 FPS - Low quality
    };

    for (const auto& Scenario : Scenarios)
    {
        // Simulate frames
        for (int32 i = 0; i < 60; ++i)
        {
            Component->UpdatePerformanceMetrics(Scenario.FrameTime);
        }

        // Verify quality adjustment
        bTestPassed &= TestEqual(TEXT("Quality level adjusted correctly"), 
            Component->GetCurrentQualityLevel(), 
            Scenario.ExpectedQualityLevel
        );
    }

    TestActor->Destroy();
    return bTestPassed;
}
