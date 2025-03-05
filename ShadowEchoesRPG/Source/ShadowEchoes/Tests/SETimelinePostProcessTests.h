#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

class USETimelinePostProcessComponent;

/**
 * Test suite for Timeline post-process effects
 */
class FTimelinePostProcessTest : public FAutomationTestBase
{
public:
    FTimelinePostProcessTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
    }

    virtual bool RunTest(const FString& Parameters) override;

private:
    /** Test component initialization */
    bool TestInitialization(USETimelinePostProcessComponent* Component);

    /** Test transition effect functionality */
    bool TestTransitionEffects(USETimelinePostProcessComponent* Component);

    /** Test quality settings */
    bool TestQualitySettings(USETimelinePostProcessComponent* Component);

    /** Test performance monitoring */
    bool TestPerformanceMonitoring(USETimelinePostProcessComponent* Component);
};

/**
 * Specialized test for quality settings
 */
class FTimelinePostProcessQualityTest : public FAutomationTestBase
{
public:
    FTimelinePostProcessQualityTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
    }

    virtual bool RunTest(const FString& Parameters) override;

private:
    /** Verify quality preset parameters */
    bool VerifyQualityPreset(USETimelinePostProcessComponent* Component, int32 QualityLevel);

    /** Test quality transitions */
    bool TestQualityTransitions(USETimelinePostProcessComponent* Component);

    /** Validate material parameters for quality level */
    bool ValidateMaterialParameters(USETimelinePostProcessComponent* Component, int32 QualityLevel);
};

/**
 * Specialized test for performance monitoring and optimization
 */
class FTimelinePostProcessPerformanceTest : public FAutomationTestBase
{
public:
    FTimelinePostProcessPerformanceTest(const FString& InName, const bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask)
    {
    }

    virtual bool RunTest(const FString& Parameters) override;

private:
    /** Test performance scenarios */
    struct FPerformanceScenario
    {
        float FrameTime;
        int32 ExpectedQualityLevel;
        FString Description;
    };

    /** Run performance scenario */
    bool RunPerformanceScenario(USETimelinePostProcessComponent* Component, const FPerformanceScenario& Scenario);

    /** Simulate frame updates */
    void SimulateFrames(USETimelinePostProcessComponent* Component, float FrameTime, int32 NumFrames);

    /** Verify performance adaptations */
    bool VerifyPerformanceAdaptation(USETimelinePostProcessComponent* Component, const FPerformanceScenario& Scenario);
};

/**
 * Helper functions for post-process tests
 */
namespace TimelinePostProcessTestHelpers
{
    /** Create test actor with post-process component */
    AActor* CreateTestActor(UWorld* World);

    /** Setup test environment */
    void SetupTestEnvironment();

    /** Cleanup test environment */
    void CleanupTestEnvironment();

    /** Validate material parameter values */
    bool ValidateMaterialParameter(UMaterialInstanceDynamic* Material, const FName& ParamName, float ExpectedValue, float Tolerance = 0.01f);

    /** Validate color parameter values */
    bool ValidateColorParameter(UMaterialInstanceDynamic* Material, const FName& ParamName, const FLinearColor& ExpectedColor, float Tolerance = 0.01f);

    /** Get quality level settings */
    struct FQualitySettings
    {
        float NoiseScale;
        float DistortionStrength;
        float EmissiveIntensity;
        float EdgeSharpness;
    };

    FQualitySettings GetQualitySettings(int32 QualityLevel);

    /** Performance monitoring helpers */
    struct FPerformanceMetrics
    {
        float AverageFrameTime;
        float FrameTimeDeviation;
        int32 QualityLevel;
    };

    FPerformanceMetrics GatherPerformanceMetrics(USETimelinePostProcessComponent* Component, int32 NumFrames);

    /** Test constants */
    namespace Constants
    {
        const float HighQualityFPS = 60.0f;
        const float MediumQualityFPS = 30.0f;
        const float LowQualityFPS = 15.0f;
        const float FrameTimeThreshold = 0.1f;
        const int32 TestFrameCount = 60;
        const float TransitionDuration = 1.0f;
    }
}
