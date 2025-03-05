#pragma once

#include "CoreMinimal.h"
#include "SETimelinePostProcessTestExecutor.h"
#include "SETimelineTestData.h"

/**
 * Runner for Timeline post-process tests
 */
class SHADOWECHOES_API FSETimelinePostProcessTestRunner : public FSETimelinePostProcessTestExecutor::ITestExecutionObserver
{
public:
    /** Get runner instance */
    static FSETimelinePostProcessTestRunner& Get();

    /** Initialize runner */
    void Initialize();

    /** Shutdown runner */
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

    /** Test execution observer */
    class ITestRunnerObserver
    {
    public:
        virtual ~ITestRunnerObserver() {}
        virtual void OnTestRunStarted() = 0;
        virtual void OnTestRunCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results) = 0;
        virtual void OnTestStarted(const FString& TestName) = 0;
        virtual void OnTestCompleted(const SETimelineTest::FTestResult& Result) = 0;
        virtual void OnTestSuiteStarted(const FString& SuiteName) = 0;
        virtual void OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result) = 0;
        virtual void OnProgressUpdated(float Progress) = 0;
        virtual void OnError(const FString& ErrorMessage) = 0;
    };

    /** Register observer */
    void RegisterObserver(ITestRunnerObserver* Observer);

    /** Unregister observer */
    void UnregisterObserver(ITestRunnerObserver* Observer);

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

    /** Test execution observer interface */
    virtual void OnTestStarted(const FString& TestName) override;
    virtual void OnTestCompleted(const SETimelineTest::FTestResult& Result) override;
    virtual void OnTestSuiteStarted(const FString& SuiteName) override;
    virtual void OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result) override;
    virtual void OnAllTestsCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results) override;
    virtual void OnExecutionPaused() override;
    virtual void OnExecutionResumed() override;
    virtual void OnExecutionStopped() override;
    virtual void OnError(const FString& ErrorMessage) override;

private:
    /** Constructor */
    FSETimelinePostProcessTestRunner();

    /** Test executor */
    FSETimelinePostProcessTestExecutor Executor;

    /** State */
    bool bInitialized;
    TArray<ITestRunnerObserver*> Observers;

    /** Prevent copying */
    FSETimelinePostProcessTestRunner(const FSETimelinePostProcessTestRunner&) = delete;
    FSETimelinePostProcessTestRunner& operator=(const FSETimelinePostProcessTestRunner&) = delete;

    /** Helper functions */
    void NotifyTestRunStarted();
    void NotifyTestRunCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results);
    void NotifyTestStarted(const FString& TestName);
    void NotifyTestCompleted(const SETimelineTest::FTestResult& Result);
    void NotifyTestSuiteStarted(const FString& SuiteName);
    void NotifyTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result);
    void NotifyProgressUpdated(float Progress);
    void NotifyError(const FString& ErrorMessage);
};
