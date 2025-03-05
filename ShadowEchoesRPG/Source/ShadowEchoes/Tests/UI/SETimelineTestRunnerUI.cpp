// Continuing from previous implementation...

void USETimelineTestRunnerUI::UpdateTestStatus()
{
    if (StatusText)
    {
        const bool bIsRunning = ProgressBar && ProgressBar->GetVisibility() == ESlateVisibility::Visible;
        const int32 TotalTests = TestResults.Num();
        const int32 PassedTests = TestResults.FilterByPredicate([](const USETimelineTestResultEntry* Entry) {
            return Entry && Entry->GetTestResult().Status == SETimelineTest::ETestStatus::Passed;
        }).Num();
        const int32 FailedTests = TestResults.FilterByPredicate([](const USETimelineTestResultEntry* Entry) {
            return Entry && Entry->GetTestResult().Status == SETimelineTest::ETestStatus::Failed;
        }).Num();
        
        if (bIsRunning)
        {
            StatusText->SetText(FText::FromString(TEXT("Running tests...")));
            StatusText->SetColorAndOpacity(FSlateColor(Style->InfoColor));
        }
        else if (TotalTests == 0)
        {
            StatusText->SetText(FText::FromString(TEXT("Ready")));
            StatusText->SetColorAndOpacity(FSlateColor(Style->SecondaryColor));
        }
        else
        {
            FString Status = FString::Printf(TEXT("%d/%d tests passed"), PassedTests, TotalTests);
            if (FailedTests > 0)
            {
                Status += FString::Printf(TEXT(" (%d failed)"), FailedTests);
                StatusText->SetColorAndOpacity(FSlateColor(Style->ErrorColor));
            }
            else
            {
                StatusText->SetColorAndOpacity(FSlateColor(Style->SuccessColor));
            }
            StatusText->SetText(FText::FromString(Status));
        }
    }
}

void USETimelineTestRunnerUI::UpdateErrorMessage()
{
    if (ErrorText)
    {
        const bool bHasError = !ErrorText->GetText().IsEmpty();
        ErrorText->SetVisibility(bHasError ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        
        if (bHasError)
        {
            PlayAnimation(GetAnimation(TEXT("ErrorShake")));
        }
    }
}

void USETimelineTestRunnerUI::UpdateLoadingState()
{
    if (ProgressBar)
    {
        const bool bIsRunning = ProgressBar->GetVisibility() == ESlateVisibility::Visible;
        
        if (bIsRunning)
        {
            PlayAnimation(GetAnimation(TEXT("LoadingPulse")));
        }
        else
        {
            StopAnimation(GetAnimation(TEXT("LoadingPulse")));
        }
        
        UpdateEnabledState();
        UpdateTestStatus();
    }
}

void USETimelineTestRunnerUI::UpdateErrorState()
{
    if (ErrorText)
    {
        const bool bHasError = !ErrorText->GetText().IsEmpty();
        
        if (bHasError)
        {
            PlayAnimation(GetAnimation(TEXT("ErrorShake")));
            
            if (StatusText)
            {
                StatusText->SetColorAndOpacity(FSlateColor(Style->ErrorColor));
            }
            
            if (BackgroundBorder)
            {
                FLinearColor ErrorHighlight = Style->ErrorColor;
                ErrorHighlight.A = 0.1f;
                BackgroundBorder->SetBrushColor(ErrorHighlight);
            }
        }
        else
        {
            if (StatusText)
            {
                StatusText->SetColorAndOpacity(FSlateColor(Style->SecondaryColor));
            }
            
            if (BackgroundBorder)
            {
                BackgroundBorder->SetBrushColor(Style->LightColor);
            }
        }
    }
}

void USETimelineTestRunnerUI::UpdateSuccessState()
{
    const int32 TotalTests = Test
