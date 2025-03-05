#pragma once

#include "CoreMinimal.h"
#include "SETimelineTestData.h"
#include "SETimelinePostProcessTestRunner.h"
#include "SETimelinePostProcessTestExecutor.h"

/**
 * Main module for Timeline post-process testing system
 */
class SHADOWECHOES_API FSETimelineTestModule : public FSETimelinePostProcessTestRunner::ITestRunnerObserver
{
public:
    /** Get module instance */
    static FSETimelineTestModule& Get();

    /** Initialize module */
    void Initialize();

    /** Shutdown module */
    void Shutdown();

    /** Run tests */
    bool RunTests(const TArray<FString>& TestNames = TArray<FString>());

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

    /** Get test config */
    bool GetTestConfig(const FString& TestName, SETimelineTest::FTestConfig& OutConfig) const;

    /** Get all test configs */
    TArray<SETimelineTest::FTestConfig> GetTestConfigs() const;

    /** Get test results */
    const FSETimelinePostProcessTestExecutor::FTestResults& GetResults() const;

    /** Get current test */
    FString GetCurrentTest() const;

    /** Get progress */
    float GetProgress() const;

    /** Is running */
    bool IsRunning() const;

    /** Pause execution */
    void PauseExecution();

    /** Resume execution */
    void ResumeExecution();

    /** Stop execution */
    void StopExecution();

    /** Configure execution */
    void ConfigureExecution(const SETimelineTest::FTestExecutionOptions& Options);

    /** Get execution options */
    const SETimelineTest::FTestExecutionOptions& GetExecutionOptions() const;

    /** Test runner observer interface */
    virtual void OnTestRunStarted() override;
    virtual void OnTestRunCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results) override;
    virtual void OnTestStarted(const FString& TestName) override;
    virtual void OnTestCompleted(const SETimelineTest::FTestResult& Result) override;
    virtual void OnTestSuiteStarted(const FString& SuiteName) override;
    virtual void OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result) override;
    virtual void OnProgressUpdated(float Progress) override;
    virtual void OnError(const FString& ErrorMessage) override;

private:
    /** Constructor */
    FSETimelineTestModule();

    /** Test runner */
    FSETimelinePostProcessTestRunner Runner;

    /** Test configs */
    TMap<FString, SETimelineTest::FTestConfig> TestConfigs;

    /** State */
    bool bInitialized;

    /** Initialize test configs */
    void InitializeTestConfigs();

    /** Load test configs */
    void LoadTestConfigs();

    /** Save test configs */
    void SaveTestConfigs();

    /** Validate test configs */
    bool ValidateTestConfigs();

    /** Get test config by name */
    SETimelineTest::FTestConfig* GetTestConfigPtr(const FString& TestName);
    const SETimelineTest::FTestConfig* GetTestConfigPtr(const FString& TestName) const;

    /** Filter test configs */
    TArray<SETimelineTest::FTestConfig> FilterTestConfigs(
        const TFunction<bool(const SETimelineTest::FTestConfig&)>& Predicate) const;

    /** Prevent copying */
    FSETimelineTestModule(const FSETimelineTestModule&) = delete;
    FSETimelineTestModule& operator=(const FSETimelineTestModule&) = delete;
};
