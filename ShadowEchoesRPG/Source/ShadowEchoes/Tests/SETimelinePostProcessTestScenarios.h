#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"

/**
 * Test scenarios for Timeline post-process system
 */
class SHADOWECHOES_API FSETimelinePostProcessTestScenarios
{
public:
    /** Get all test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetAllTestScenarios();

    /** Basic test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetBasicTestScenarios();

    /** Visual test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetVisualTestScenarios();

    /** Performance test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetPerformanceTestScenarios();

    /** Integration test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetIntegrationTestScenarios();

    /** Stress test scenarios */
    static TArray<SETimelineTest::FTestConfig> GetStressTestScenarios();

private:
    /** Basic test cases */
    static SETimelineTest::FTestConfig CreateBasicSetupTest();
    static SETimelineTest::FTestConfig CreateBasicParametersTest();
    static SETimelineTest::FTestConfig CreateBasicBlendingTest();
    static SETimelineTest::FTestConfig CreateBasicTransitionTest();
    static SETimelineTest::FTestConfig CreateBasicCleanupTest();

    /** Visual test cases */
    static SETimelineTest::FTestConfig CreateVisualEffectTest();
    static SETimelineTest::FTestConfig CreateVisualBlendingTest();
    static SETimelineTest::FTestConfig CreateVisualTransitionTest();
    static SETimelineTest::FTestConfig CreateVisualQualityTest();
    static SETimelineTest::FTestConfig CreateVisualComparisonTest();

    /** Performance test cases */
    static SETimelineTest::FTestConfig CreatePerformanceBaselineTest();
    static SETimelineTest::FTestConfig CreatePerformanceLoadTest();
    static SETimelineTest::FTestConfig CreatePerformanceScalingTest();
    static SETimelineTest::FTestConfig CreatePerformanceMemoryTest();
    static SETimelineTest::FTestConfig CreatePerformanceGPUTest();

    /** Integration test cases */
    static SETimelineTest::FTestConfig CreateIntegrationTimelineTest();
    static SETimelineTest::FTestConfig CreateIntegrationGameplayTest();
    static SETimelineTest::FTestConfig CreateIntegrationUITest();
    static SETimelineTest::FTestConfig CreateIntegrationSaveLoadTest();
    static SETimelineTest::FTestConfig CreateIntegrationNetworkTest();

    /** Stress test cases */
    static SETimelineTest::FTestConfig CreateStressLoadTest();
    static SETimelineTest::FTestConfig CreateStressMemoryTest();
    static SETimelineTest::FTestConfig CreateStressGPUTest();
    static SETimelineTest::FTestConfig CreateStressTransitionTest();
    static SETimelineTest::FTestConfig CreateStressRecoveryTest();

    /** Helper functions */
    static SETimelineTest::FTestConfig CreateTestConfig(
        const FString& TestName,
        const FString& Description,
        SETimelineTest::ETestCategory Category,
        SETimelineTest::ETestPriority Priority,
        const SETimelineTest::FTestRequirements& Requirements);

    static SETimelineTest::FTestRequirements CreateBasicRequirements();
    static SETimelineTest::FTestRequirements CreateVisualRequirements();
    static SETimelineTest::FTestRequirements CreatePerformanceRequirements();
    static SETimelineTest::FTestRequirements CreateIntegrationRequirements();
    static SETimelineTest::FTestRequirements CreateStressRequirements();

    /** Test parameters */
    static TMap<FString, FString> CreateBasicParameters();
    static TMap<FString, FString> CreateVisualParameters();
    static TMap<FString, FString> CreatePerformanceParameters();
    static TMap<FString, FString> CreateIntegrationParameters();
    static TMap<FString, FString> CreateStressParameters();

    /** Test metadata */
    static TMap<FString, FString> CreateBasicMetadata();
    static TMap<FString, FString> CreateVisualMetadata();
    static TMap<FString, FString> CreatePerformanceMetadata();
    static TMap<FString, FString> CreateIntegrationMetadata();
    static TMap<FString, FString> CreateStressMetadata();
};
