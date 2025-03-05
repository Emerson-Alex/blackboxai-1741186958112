#include "SETimelineTestModule.h"
#include "UI/SETimelineTestStyle.h"
#include "Framework/Commands/Commands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "EditorStyleSet.h"
#include "Misc/ConfigCacheIni.h"

// Static member initialization
const FName FSETimelineTestModule::TabId = TEXT("TimelineTestRunner");
const FName FSETimelineTestModule::WindowStateSection = TEXT("TimelineTestRunner");
const FVector2D FSETimelineTestModule::DefaultWindowSize(800, 600);
const FVector2D FSETimelineTestModule::MinWindowSize(400, 300);

void FSETimelineTestModule::RegisterCommands()
{
    CommandList = MakeShareable(new FUICommandList);

    CommandList->MapAction(
        TestCommands->OpenTestRunner,
        FExecuteAction::CreateRaw(this, &FSETimelineTestModule::HandleOpenTestRunner),
        FCanExecuteAction()
    );
}

void FSETimelineTestModule::RegisterMenuExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    // Add menu extension
    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
    MenuExtender->AddMenuExtension(
        "WindowLayout",
        EExtensionHook::After,
        CommandList,
        FMenuExtensionDelegate::CreateRaw(this, &FSETimelineTestModule::AddMenuEntry)
    );
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

    // Add toolbar extension
    TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
    ToolbarExtender->AddToolBarExtension(
        "Settings",
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateLambda([this](FToolBarBuilder& Builder)
        {
            Builder.AddToolBarButton(
                TestCommands->OpenTestRunner,
                NAME_None,
                FText::FromString(TEXT("Timeline Tests")),
                FText::FromString(TEXT("Open Timeline Test Runner")),
                FSlateIcon(FSETimelineTestStyle::GetStyleSetName(), "TimelineTest.Icon")
            );
        })
    );
    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FSETimelineTestModule::RegisterTabSpawners()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        TabId,
        FOnSpawnTab::CreateRaw(this, &FSETimelineTestModule::SpawnTestRunnerTab)
    )
    .SetDisplayName(NSLOCTEXT("TimelineTest", "TabTitle", "Timeline Test Runner"))
    .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSETimelineTestModule::UnregisterTabSpawners()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
}

TSharedRef<SSETimelineTestRunnerUI> FSETimelineTestModule::CreateTestRunnerUI()
{
    return SNew(SSETimelineTestRunnerUI);
}

void FSETimelineTestModule::SaveWindowState()
{
    if (!ShouldSaveWindowState())
    {
        return;
    }

    if (TestRunnerUI.IsValid())
    {
        TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(TestRunnerUI.ToSharedRef());
        if (Window.IsValid())
        {
            WindowState.Position = Window->GetPositionInScreen();
            WindowState.Size = Window->GetSizeInScreen();
            WindowState.bIsMaximized = Window->IsMaximized();

            GConfig->SetVector2D(
                *WindowStateSection,
                TEXT("Position"),
                WindowState.Position,
                GEditorPerProjectIni
            );

            GConfig->SetVector2D(
                *WindowStateSection,
                TEXT("Size"),
                WindowState.Size,
                GEditorPerProjectIni
            );

            GConfig->SetBool(
                *WindowStateSection,
                TEXT("IsMaximized"),
                WindowState.bIsMaximized,
                GEditorPerProjectIni
            );

            GConfig->Flush(false, GEditorPerProjectIni);
        }
    }
}

void FSETimelineTestModule::LoadWindowState()
{
    GConfig->GetVector2D(
        *WindowStateSection,
        TEXT("Position"),
        WindowState.Position,
        GEditorPerProjectIni
    );

    GConfig->GetVector2D(
        *WindowStateSection,
        TEXT("Size"),
        WindowState.Size,
        GEditorPerProjectIni
    );

    GConfig->GetBool(
        *WindowStateSection,
        TEXT("IsMaximized"),
        WindowState.bIsMaximized,
        GEditorPerProjectIni
    );

    // Validate window state
    FDisplayMetrics DisplayMetrics;
    FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

    const FVector2D DisplaySize(
        DisplayMetrics.PrimaryDisplayWidth,
        DisplayMetrics.PrimaryDisplayHeight
    );

    // Ensure window is within screen bounds
    if (WindowState.Position.X < 0 || WindowState.Position.X > DisplaySize.X ||
        WindowState.Position.Y < 0 || WindowState.Position.Y > DisplaySize.Y)
    {
        WindowState.Position = FVector2D(100, 100);
    }

    // Ensure window size is valid
    if (WindowState.Size.X < MinWindowSize.X || WindowState.Size.Y < MinWindowSize.Y)
    {
        WindowState.Size = DefaultWindowSize;
    }
}

void FSETimelineTestModule::InitializeWindowState()
{
    WindowState.Position = FVector2D(100, 100);
    WindowState.Size = DefaultWindowSize;
    WindowState.bIsMaximized = false;
    LoadWindowState();
}

void FSETimelineTestModule::UpdateWindowState(const FGeometry& AllottedGeometry)
{
    if (TestRunnerUI.IsValid())
    {
        TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(TestRunnerUI.ToSharedRef());
        if (Window.IsValid())
        {
            WindowState.Position = Window->GetPositionInScreen();
            WindowState.Size = Window->GetSizeInScreen();
            WindowState.bIsMaximized = Window->IsMaximized();
        }
    }
}

bool FSETimelineTestModule::ShouldSaveWindowState() const
{
    return TestRunnerUI.IsValid() && !WindowState.bIsMaximized;
}

void FSETimelineTestModule::HandleTabClosed(TSharedRef<SDockTab> Tab)
{
    SaveWindowState();
    TestRunnerUI.Reset();
}
