#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "HAL/CriticalSection.h"

/**
 * Executor for Timeline post-process tests
 */
class SHADOWECHOES_API FSETimelinePostProcessTestExecutor
{
public:
    /** Test execution observer */
    class ITestExecutionObserver
    {
    public:
        virtual ~ITestExecutionObserver() {}
        virtual void OnTestStarted(const FString& TestName) = 0;
        virtual void OnTestCompleted(const SETimelineTest::FTestResult& Result) = 0;
        virtual void OnTestSuiteStarted(const FString& SuiteName) = 0;
        virtual void OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result) = 0;
        virtual void OnAllTestsCompleted(const FTestResults& Results) = 0;
        virtual void OnExecutionPaused() = 0;
        virtual void OnExecutionResumed() = 0;
        virtual void OnExecutionStopped() = 0;
        virtual void OnError(const FString& ErrorMessage) = 0;
    };

    /** Test results */
    struct FTestResults
    {
        int32 TotalTests;
        int32 PassedTests;
        int32 FailedTests;
        int32 SkippedTests;
        float TotalExecutionTime;
        FDateTime StartTime;
        FDateTime EndTime;
        TArray<SETimelineTest::FTestResult> Results;
        TArray<FString> Screenshots;
        TArray<FString> Logs;

        FTestResults()
            : TotalTests(0)
            , PassedTests(0)
            , FailedTests(0)
            , SkippedTests(0)
            , TotalExecutionTime(0.0f)
            , StartTime(FDateTime::Now())
            , EndTime(FDateTime::Now())
        {}
    };

    /** Initialize executor */
    void Initialize();

    /** Shutdown executor */
    void Shutdown();

    /** Run test */
    bool RunTest(const FString& TestName);

    /** Run test suite */
    bool RunTestSuite(const FString& SuiteName);

    /** Run all tests */
    bool RunAllTests();

    /** Run tests by category */
    bool RunTestsByCategory(SETimelineTest::ETestCategory Category);

    /** Run tests by priority */
    bool RunTestsByPriority(SETimelineTest::ETestPriority Priority);

    /** Run filtered tests */
    bool RunFilteredTests(const FString& Filter);

    /** Register observer */
    void RegisterObserver(ITestExecutionObserver* Observer);

    /** Unregister observer */
    void UnregisterObserver(ITestExecutionObserver* Observer);

    /** Get test results */
    const FTestResults& GetResults() const { return Results; }

    /** Get current test */
    FString GetCurrentTest() const { return CurrentTest; }

    /** Get progress */
    float GetProgress() const { return Progress; }

    /** Is running */
    bool IsRunning() const { return bIsRunning; }

    /** Pause execution */
    void PauseExecution();

    /** Resume execution */
    void ResumeExecution();

    /** Stop execution */
    void StopExecution();

    /** Configure execution */
    void ConfigureExecution(const SETimelineTest::FTestExecutionOptions& Options);

    /** Get execution options */
    const SETimelineTest::FTestExecutionOptions& GetExecutionOptions() const { return ExecutionOptions; }

private:
    /** Execute test */
    SETimelineTest::FTestResult ExecuteTest(const SETimelineTest::FTestConfig& Config);

    /** Execute test suite */
    SETimelineTest::FTestSuiteResult ExecuteTestSuite(const FString& SuiteName, const TArray<SETimelineTest::FTestConfig>& Tests);

    /** Test execution helpers */
    bool SetupTestEnvironment(const SETimelineTest::FTestConfig& Config);
    void CleanupTestEnvironment();
    bool ValidateTestRequirements(const SETimelineTest::FTestConfig& Config);
    void CaptureTestScreenshot(const FString& TestName);
    void CollectTestLogs(const FString& TestName);
    void UpdateProgress(float NewProgress);
    void NotifyObservers(const FString& TestName, const SETimelineTest::FTestResult& Result);
    void HandleTestError(const FString& TestName, const FString& ErrorMessage);
    void HandleTestTimeout(const FString& TestName);
    void HandleTestCrash(const FString& TestName);

    /** State */
    bool bInitialized;
    bool bIsRunning;
    bool bIsPaused;
    FString CurrentTest;
    float Progress;
    FTestResults Results;
    SETimelineTest::FTestExecutionOptions ExecutionOptions;
    TArray<ITestExecutionObserver*> Observers;
    mutable FCriticalSection ExecutionLock;

    /** Test environment */
    UWorld* TestWorld;
    TArray<AActor*> TestActors;
    TArray<UObject*> TestObjects;

    /** Prevent copying */
    FSETimelinePostProcessTestExecutor(const FSETimelinePostProcessTestExecutor&) = delete;
    FSETimelinePostProcessTestExecutor& operator=(const FSETimelinePostProcessTestExecutor&) = delete;
};
