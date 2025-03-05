#include "SETimelineTestModule.h"
#include "SETimelineTestLogger.h"
#include "SETimelineTestMetrics.h"
#include "SETimelinePostProcessTestScenarios.h"
#include "SETimelineTestConfig.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

FSETimelineTestModule& FSETimelineTestModule::Get()
{
    static FSETimelineTestModule Instance;
    return Instance;
}

FSETimelineTestModule::FSETimelineTestModule()
    : bInitialized(false)
{
}

void FSETimelineTestModule::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    // Initialize components
    FSETimelineTestLogger::Get().Initialize();
    FSETimelineTestMetrics::Get().Initialize();
    Runner.Initialize();
    Runner.RegisterObserver(this);

    // Initialize test configs
    InitializeTestConfigs();

    bInitialized = true;
}

void FSETimelineTestModule::Shutdown()
{
    if (!bInitialized)
    {
        return;
    }

    // Save test configs
    SaveTestConfigs();

    // Shutdown components
    Runner.UnregisterObserver(this);
    Runner.Shutdown();
    FSETimelineTestMetrics::Get().Shutdown();
    FSETimelineTestLogger::Get().Shutdown();

    bInitialized = false;
}

bool FSETimelineTestModule::RunTests(const TArray<FString>& TestNames)
{
    if (!bInitialized)
    {
        return false;
    }

    if (TestNames.Num() == 0)
    {
        return RunAllTests();
    }

    bool bSuccess = true;
    for (const FString& TestName : TestNames)
    {
        bSuccess &= Runner.RunTest(TestName);
    }
    return bSuccess;
}

bool FSETimelineTestModule::RunTestSuite(const FString& SuiteName)
{
    if (!bInitialized)
    {
        return false;
    }

    return Runner.RunTestSuite(SuiteName);
}

bool FSETimelineTestModule::RunAllTests()
{
    if (!bInitialized)
    {
        return false;
    }

    return Runner.RunAllTests();
}

bool FSETimelineTestModule::RunTestsByCategory(SETimelineTest::ETestCategory Category)
{
    if (!bInitialized)
    {
        return false;
    }

    return Runner.RunTestsByCategory(Category);
}

bool FSETimelineTestModule::RunTestsByPriority(SETimelineTest::ETestPriority Priority)
{
    if (!bInitialized)
    {
        return false;
    }

    return Runner.RunTestsByPriority(Priority);
}

bool FSETimelineTestModule::RunFilteredTests(const FString& Filter)
{
    if (!bInitialized)
    {
        return false;
    }

    return Runner.RunFilteredTests(Filter);
}

bool FSETimelineTestModule::GetTestConfig(const FString& TestName, SETimelineTest::FTestConfig& OutConfig) const
{
    const SETimelineTest::FTestConfig* Config = GetTestConfigPtr(TestName);
    if (Config)
    {
        OutConfig = *Config;
        return true;
    }
    return false;
}

TArray<SETimelineTest::FTestConfig> FSETimelineTestModule::GetTestConfigs() const
{
    TArray<SETimelineTest::FTestConfig> Configs;
    TestConfigs.GenerateValueArray(Configs);
    return Configs;
}

const FSETimelinePostProcessTestExecutor::FTestResults& FSETimelineTestModule::GetResults() const
{
    return Runner.GetResults();
}

FString FSETimelineTestModule::GetCurrentTest() const
{
    return Runner.GetCurrentTest();
}

float FSETimelineTestModule::GetProgress() const
{
    return Runner.GetProgress();
}

bool FSETimelineTestModule::IsRunning() const
{
    return Runner.IsRunning();
}

void FSETimelineTestModule::PauseExecution()
{
    Runner.PauseExecution();
}

void FSETimelineTestModule::ResumeExecution()
{
    Runner.ResumeExecution();
}

void FSETimelineTestModule::StopExecution()
{
    Runner.StopExecution();
}

void FSETimelineTestModule::ConfigureExecution(const SETimelineTest::FTestExecutionOptions& Options)
{
    Runner.ConfigureExecution(Options);
}

const SETimelineTest::FTestExecutionOptions& FSETimelineTestModule::GetExecutionOptions() const
{
    return Runner.GetExecutionOptions();
}

void FSETimelineTestModule::InitializeTestConfigs()
{
    // Load existing configs
    LoadTestConfigs();

    // Add test scenarios
    const TArray<SETimelineTest::FTestConfig>& Scenarios = FSETimelinePostProcessTestScenarios::GetAllTestScenarios();
    for (const auto& Scenario : Scenarios)
    {
        TestConfigs.Add(Scenario.TestName, Scenario);
    }

    // Validate configs
    ValidateTestConfigs();
}

void FSETimelineTestModule::LoadTestConfigs()
{
    // Load from config file
    const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("TimelineTest.ini");
    if (GConfig->LoadFile(ConfigPath))
    {
        // Load test configs
        TArray<FString> TestNames;
        GConfig->GetSection(TEXT("TestConfigs"), TestNames, ConfigPath);

        for (const FString& TestName : TestNames)
        {
            SETimelineTest::FTestConfig Config;
            Config.TestName = TestName;

            // Load test config properties
            FString SectionName = FString::Printf(TEXT("TestConfig_%s"), *TestName);
            GConfig->GetString(*SectionName, TEXT("Description"), Config.Description, ConfigPath);
            int32 Category;
            GConfig->GetInt(*SectionName, TEXT("Category"), Category, ConfigPath);
            Config.Category = static_cast<SETimelineTest::ETestCategory>(Category);
            int32 Priority;
            GConfig->GetInt(*SectionName, TEXT("Priority"), Priority, ConfigPath);
            Config.Priority = static_cast<SETimelineTest::ETestPriority>(Priority);
            GConfig->GetBool(*SectionName, TEXT("Enabled"), Config.bEnabled, ConfigPath);

            TestConfigs.Add(TestName, Config);
        }
    }
}

void FSETimelineTestModule::SaveTestConfigs()
{
    // Save to config file
    const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("TimelineTest.ini");

    // Save test configs
    for (const auto& Pair : TestConfigs)
    {
        const FString& TestName = Pair.Key;
        const SETimelineTest::FTestConfig& Config = Pair.Value;

        FString SectionName = FString::Printf(TEXT("TestConfig_%s"), *TestName);
        GConfig->SetString(*SectionName, TEXT("Description"), *Config.Description, ConfigPath);
        GConfig->SetInt(*SectionName, TEXT("Category"), static_cast<int32>(Config.Category), ConfigPath);
        GConfig->SetInt(*SectionName, TEXT("Priority"), static_cast<int32>(Config.Priority), ConfigPath);
        GConfig->SetBool(*SectionName, TEXT("Enabled"), Config.bEnabled, ConfigPath);
    }

    GConfig->SaveFile(ConfigPath);
}

bool FSETimelineTestModule::ValidateTestConfigs()
{
    bool bValid = true;
    for (const auto& Pair : TestConfigs)
    {
        const FString& TestName = Pair.Key;
        const SETimelineTest::FTestConfig& Config = Pair.Value;

        if (TestName != Config.TestName)
        {
            FSETimelineTestLogger::Get().LogError(TEXT("TestModule"), 
                FString::Printf(TEXT("Test config name mismatch: %s != %s"), *TestName, *Config.TestName));
            bValid = false;
        }
    }
    return bValid;
}

SETimelineTest::FTestConfig* FSETimelineTestModule::GetTestConfigPtr(const FString& TestName)
{
    return TestConfigs.Find(TestName);
}

const SETimelineTest::FTestConfig* FSETimelineTestModule::GetTestConfigPtr(const FString& TestName) const
{
    return TestConfigs.Find(TestName);
}

TArray<SETimelineTest::FTestConfig> FSETimelineTestModule::FilterTestConfigs(
    const TFunction<bool(const SETimelineTest::FTestConfig&)>& Predicate) const
{
    TArray<SETimelineTest::FTestConfig> FilteredConfigs;
    for (const auto& Pair : TestConfigs)
    {
        if (Predicate(Pair.Value))
        {
            FilteredConfigs.Add(Pair.Value);
        }
    }
    return FilteredConfigs;
}

void FSETimelineTestModule::OnTestRunStarted()
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestModule"), TEXT("Test run started"));
}

void FSETimelineTestModule::OnTestRunCompleted(const FSETimelinePostProcessTestExecutor::FTestResults& Results)
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestModule"), 
        FString::Printf(TEXT("Test run completed: %d total, %d passed, %d failed, %d skipped"),
            Results.TotalTests,
            Results.PassedTests,
            Results.FailedTests,
            Results.SkippedTests));
}

void FSETimelineTestModule::OnTestStarted(const FString& TestName)
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestModule"), 
        FString::Printf(TEXT("Starting test: %s"), *TestName));
}

void FSETimelineTestModule::OnTestCompleted(const SETimelineTest::FTestResult& Result)
{
    FSETimelineTestLogger::Get().LogTestResult(Result);
}

void FSETimelineTestModule::OnTestSuiteStarted(const FString& SuiteName)
{
    FSETimelineTestLogger::Get().LogInfo(TEXT("TestModule"), 
        FString::Printf(TEXT("Starting test suite: %s"), *SuiteName));
}

void FSETimelineTestModule::OnTestSuiteCompleted(const SETimelineTest::FTestSuiteResult& Result)
{
    FSETimelineTestLogger::Get().LogTestSuiteResult(Result);
}

void FSETimelineTestModule::OnProgressUpdated(float Progress)
{
    // No need to log progress updates
}

void FSETimelineTestModule::OnError(const FString& ErrorMessage)
{
    FSETimelineTestLogger::Get().LogError(TEXT("TestModule"), ErrorMessage);
}
