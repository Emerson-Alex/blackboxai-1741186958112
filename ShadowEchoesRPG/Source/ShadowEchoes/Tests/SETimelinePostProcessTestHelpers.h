#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "Components/SETimelinePostProcessComponent.h"
#include "GameFramework/Actor.h"

/**
 * Helper functions for Timeline post-process tests
 */
class SHADOWECHOES_API FSETimelinePostProcessTestHelpers
{
public:
    /** Setup test environment */
    bool SetupTestEnvironment(UWorld* World, const SETimelineTest::FTestConfig& Config);

    /** Create test actor */
    AActor* CreateTestActor(UWorld* World, const FString& Name);

    /** Create post-process component */
    USETimelinePostProcessComponent* CreatePostProcessComponent(AActor* Actor);

    /** Setup post-process parameters */
    void SetupPostProcessParameters(USETimelinePostProcessComponent* Component, const FTimelinePostProcessParameters& Parameters);

    /** Validate post-process effect */
    bool ValidatePostProcessEffect(USETimelinePostProcessComponent* Component, const FTimelinePostProcessParameters& ExpectedParams);

    /** Capture screenshot */
    bool CaptureScreenshot(const FString& TestName, const FString& OutputPath);

    /** Compare screenshots */
    float CompareScreenshots(const FString& ScreenshotA, const FString& ScreenshotB);

    /** Get test world */
    static UWorld* GetTestWorld();

    /** Get test game mode */
    static AGameModeBase* GetTestGameMode();

    /** Get test player controller */
    static APlayerController* GetTestPlayerController();

    /** Get test pawn */
    static APawn* GetTestPawn();

    /** Get test camera */
    static ACameraActor* GetTestCamera();

    /** Setup test scene */
    void SetupTestScene(UWorld* World);

    /** Create test mesh */
    UStaticMeshComponent* CreateTestMesh(AActor* Actor);

    /** Create test material */
    UMaterialInterface* CreateTestMaterial();

    /** Create test texture */
    UTexture2D* CreateTestTexture();

    /** Create test light */
    ALight* CreateTestLight(UWorld* World);

    /** Setup test viewport */
    void SetupTestViewport();

    /** Setup test rendering */
    void SetupTestRendering();

    /** Setup test input */
    void SetupTestInput();

    /** Setup test audio */
    void SetupTestAudio();

    /** Setup test physics */
    void SetupTestPhysics();

    /** Setup test networking */
    void SetupTestNetworking();

    /** Setup test AI */
    void SetupTestAI();

    /** Setup test navigation */
    void SetupTestNavigation();

    /** Setup test animation */
    void SetupTestAnimation();

    /** Setup test particles */
    void SetupTestParticles();

    /** Setup test UI */
    void SetupTestUI();

    /** Setup test save game */
    void SetupTestSaveGame();

    /** Setup test streaming */
    void SetupTestStreaming();

    /** Setup test profiling */
    void SetupTestProfiling();

    /** Setup test debugging */
    void SetupTestDebugging();

    /** Setup test automation */
    void SetupTestAutomation();

    /** Setup test analytics */
    void SetupTestAnalytics();

    /** Setup test telemetry */
    void SetupTestTelemetry();

    /** Setup test crash reporting */
    void SetupTestCrashReporting();

    /** Setup test memory tracking */
    void SetupTestMemoryTracking();

    /** Setup test performance tracking */
    void SetupTestPerformanceTracking();

    /** Setup test error handling */
    void SetupTestErrorHandling();

    /** Setup test logging */
    void SetupTestLogging();

    /** Setup test configuration */
    void SetupTestConfiguration();

    /** Setup test localization */
    void SetupTestLocalization();

    /** Setup test serialization */
    void SetupTestSerialization();

    /** Setup test threading */
    void SetupTestThreading();

    /** Setup test timing */
    void SetupTestTiming();

    /** Setup test math */
    void SetupTestMath();

    /** Setup test random */
    void SetupTestRandom();

    /** Setup test file system */
    void SetupTestFileSystem();

    /** Setup test networking */
    void SetupTestNetwork();

    /** Setup test security */
    void SetupTestSecurity();

    /** Setup test encryption */
    void SetupTestEncryption();

    /** Setup test compression */
    void SetupTestCompression();

    /** Setup test validation */
    void SetupTestValidation();

    /** Setup test verification */
    void SetupTestVerification();

    /** Setup test cleanup */
    void SetupTestCleanup();

private:
    /** Helper functions */
    void InitializeTestEnvironment();
    void ValidateTestEnvironment();
    void CleanupTestEnvironment();
    void HandleTestError(const FString& ErrorMessage);
    void LogTestMessage(const FString& Message);
    void TrackTestMetrics();
    void UpdateTestProgress();
    void NotifyTestObservers();
    void ProcessTestResults();
    void GenerateTestReport();
    void ArchiveTestData();
};
