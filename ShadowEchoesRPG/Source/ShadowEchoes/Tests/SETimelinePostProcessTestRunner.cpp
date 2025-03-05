#include "SETimelinePostProcessTestRunner.h"
#include "SETimelineTestLogger.h"
#include "SETimelineTestModule.h"

FSETimelinePostProcessTestRunner& FSETimelinePostProcessTestRunner::Get()
{
    static FSETimelinePostProcessTestRunner Instance;
    return Instance;
}

FSETimelinePostProcessTestRunner::FSETimelinePostProcessTestRunner()
    : bInitialized(false)
{
}

void FSETimelinePostProcessTestRunner::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    Executor.Initialize();
    Executor.RegisterObserver(this);
    bInitialized = true;
}

void FSETimelinePostProcessTestRunner::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    StopExecution();
    Executor.UnregisterObserver(this);
    Executor.Shutdown();
    Observers.Empty();
    bInitialized = false;
}

bool FSETimelinePostProcessTestRunner::RunTest(const FString& TestName)
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunTest(TestName);
    return bResult;
}

bool FSETimelinePostProcessTestRunner::RunTestSuite(const FString& SuiteName)
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunTestSuite(SuiteName);
    return bResult;
}

bool FSETimelinePostProcessTestRunner::RunAllTests()
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunAllTests();
    return bResult;
}

bool FSETimelinePostProcessTestRunner::RunTestsByCategory(SETimelineTest::ETestCategory Category)
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunTestsByCategory(Category);
    return bResult;
}

bool FSETimelinePostProcessTestRunner::RunTestsByPriority(SETimelineTest::ETestPriority Priority)
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunTestsByPriority(Priority);
    return bResult;
}

bool FSETimelinePostProcessTestRunner::RunFilteredTests(const FString& Filter)
{
    if (!bInitialized)
    {
        return false;
    }

    NotifyTestRunStarted();
    const bool bResult = Executor.RunFilteredTests(Filter);
    return bResult;
}

void FSETimelinePostProcessTestRunner::RegisterObserver(ITestRunnerObserver* Observer)
{
    if (Observer)
    {
        Observers.AddUnique(Observer);
    }
}

void FSETimelinePostProcessTestRunner::UnregisterObserver(ITestRunnerObserver* Observer)
{
    if (Observer)
    {
        Observers.Remove(Observer);
    }
}

const FSETimelinePostProcessTestExecutor::FTestResults& FSETimelinePostProcessTestRunner::GetResults() const
{
    return Executor.GetResults();
}

FString FSETimelinePostProcessTestRunner::GetCurrentTest() const
{
    return Executor.GetCurrentTest();
}

float FSETimelinePostProcessTestRunner::GetProgress() const
{
    return Executor.GetProgress();
}

bool FSETimelinePostProcessTestRunner::IsRunning() const
{
    return Executor.IsRunning();
}

void FSETimelinePostProcessTestRunner::PauseExecution()
{
    Executor.PauseExecution();
}

void FSETimelinePostProcessTestRunner::ResumeExecution()
{
    Executor.ResumeExecution();
}

void FSETimelinePostProcessTestRunner::StopExecution()
{
    Executor.StopExecution();
}

void FSETimelinePostProcessTestRunner::ConfigureExecution(const SETimelineTest::FTestExecutionOptions& Options)
{
    Executor.ConfigureExecution(Options);
}

const SETimelineTest::FTestExecutionOptions& FSETimelinePostProcessTestRunner::GetExecutionOptions() const
{
    return Executor.GetExecutionOptions();
}

void FSETimelinePostProcessTestRunner::OnTestStarted(const FString& TestName)
{
    NotifyTestStarted(TestName);
}

void FSETimelinePostProcessTestRunner::OnTestCompleted(const SETimelineTest::FTestResult& Result)
{
    NotifyTestCompleted(Result);
}

void FSETimelinePostProcessTestRunner::OnTestSuiteStarted(const FString& SuiteName)
{
    NotifyTestSuiteStarted(SuiteName);
}

void FSETimelinePostProcessTestRunner::OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result)
{
    NotifyTestSuiteCompleted(Result);
}

void FSETimelinePostProcessTestRunner::OnAllTestsCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    NotifyTestRunCompleted(Results);
}

void FSETimelinePostProcessTestRunner::OnExecutionPaused()
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestRunner"), TEXT("Test execution paused"));
}

void FSETimelinePostProcessTestRunner::OnExecutionResumed()
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestRunner"), TEXT("Test execution resumed"));
}

void FSETimelinePostProcessTestRunner::OnExecutionStopped()
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestRunner"), TEXT("Test execution stopped"));
}

void FSETimelinePostProcessTestRunner::OnError(const FString& ErrorMessage)
{
    NotifyError(ErrorMessage);
}

void FSETimelinePostProcessTestRunner::NotifyTestRunStarted()
{
    for (auto Observer : Observers)
    {
        Observer->OnTestRunStarted();
    }
}

void FSETimelinePostProcessTestRunner::NotifyTestRunCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    for (auto Observer : Observers)
    {
        Observer->OnTestRunCompleted(Results);
    }
}

void FSETimelinePostProcessTestRunner::NotifyTestStarted(const FString& TestName)
{
    for (auto Observer : Observers)
    {
        Observer->OnTestStarted(TestName);
    }
}

void FSETimelinePostProcessTestRunner::NotifyTestCompleted(const SETimelineTest::FTestResult& Result)
{
    for (auto Observer : Observers)
    {
        Observer->OnTestCompleted(Result);
    }
}

void FSETimelinePostProcessTestRunner::NotifyTestSuiteStarted(const FString& SuiteName)
{
    for (auto Observer : Observers)
    {
        Observer->OnTestSuiteStarted(SuiteName);
    }
}

void FSETimelinePostProcessTestRunner::NotifyTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result)
{
    for (auto Observer : Observers)
    {
        Observer->OnTestSuiteCompleted(Result);
    }
}

void FSETimelinePostProcessTestRunner::NotifyProgressUpdated(float Progress)
{
    for (auto Observer : Observers)
    {
        Observer->OnProgressUpdated(Progress);
    }
}

void FSETimelinePostProcessTestRunner::NotifyError(const FString& ErrorMessage)
{
    FSETimelineTestLogger::Get().LogError(TEXT("TestRunner"), ErrorMessage);
    for (auto Observer : Observers)
    {
        Observer->OnError(ErrorMessage);
    }
}
