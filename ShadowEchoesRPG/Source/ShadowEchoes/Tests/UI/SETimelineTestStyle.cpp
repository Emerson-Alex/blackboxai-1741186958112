#include "SETimelineTestStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Interfaces/IPluginManager.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

const FLinearColor FSETimelineTestStyle::PrimaryColor(0.0f, 0.48f, 1.0f, 1.0f);    // #007bff
const FLinearColor FSETimelineTestStyle::SecondaryColor(0.42f, 0.46f, 0.49f, 1.0f); // #6c757d
const FLinearColor FSETimelineTestStyle::SuccessColor(0.16f, 0.65f, 0.27f, 1.0f);   // #28a745
const FLinearColor FSETimelineTestStyle::WarningColor(1.0f, 0.76f, 0.03f, 1.0f);    // #ffc107
const FLinearColor FSETimelineTestStyle::ErrorColor(0.86f, 0.21f, 0.27f, 1.0f);     // #dc3545
const FLinearColor FSETimelineTestStyle::InfoColor(0.09f, 0.64f, 0.72f, 1.0f);      // #17a2b8
const FLinearColor FSETimelineTestStyle::LightColor(0.97f, 0.98f, 0.98f, 1.0f);     // #f8f9fa
const FLinearColor FSETimelineTestStyle::DarkColor(0.20f, 0.23f, 0.25f, 1.0f);      // #343a40

const FSlateFontInfo FSETimelineTestStyle::HeaderFont = FCoreStyle::GetDefaultFontStyle("Bold", 24);
const FSlateFontInfo FSETimelineTestStyle::TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);
const FSlateFontInfo FSETimelineTestStyle::SubtitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 16);
const FSlateFontInfo FSETimelineTestStyle::BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 14);
const FSlateFontInfo FSETimelineTestStyle::SmallFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);
const FSlateFontInfo FSETimelineTestStyle::MonoFont = FCoreStyle::GetDefaultFontStyle("Mono", 12);

const FName FSETimelineTestStyle::BackgroundBrush(TEXT("TimelineTest.Background"));
const FName FSETimelineTestStyle::PanelBrush(TEXT("TimelineTest.Panel"));
const FName FSETimelineTestStyle::ButtonBrush(TEXT("TimelineTest.Button"));
const FName FSETimelineTestStyle::ButtonHoveredBrush(TEXT("TimelineTest.Button.Hovered"));
const FName FSETimelineTestStyle::ButtonPressedBrush(TEXT("TimelineTest.Button.Pressed"));
const FName FSETimelineTestStyle::CheckBoxBrush(TEXT("TimelineTest.CheckBox"));
const FName FSETimelineTestStyle::CheckBoxCheckedBrush(TEXT("TimelineTest.CheckBox.Checked"));
const FName FSETimelineTestStyle::RadioButtonBrush(TEXT("TimelineTest.RadioButton"));
const FName FSETimelineTestStyle::RadioButtonSelectedBrush(TEXT("TimelineTest.RadioButton.Selected"));
const FName FSETimelineTestStyle::ScrollBarBrush(TEXT("TimelineTest.ScrollBar"));
const FName FSETimelineTestStyle::ScrollBarThumbBrush(TEXT("TimelineTest.ScrollBar.Thumb"));
const FName FSETimelineTestStyle::ProgressBarBrush(TEXT("TimelineTest.ProgressBar"));
const FName FSETimelineTestStyle::ProgressBarFillBrush(TEXT("TimelineTest.ProgressBar.Fill"));
const FName FSETimelineTestStyle::BorderBrush(TEXT("TimelineTest.Border"));
const FName FSETimelineTestStyle::SeparatorBrush(TEXT("TimelineTest.Separator"));

const FVector2D FSETimelineTestStyle::IconSize(16.0f, 16.0f);
const FVector2D FSETimelineTestStyle::SmallIconSize(12.0f, 12.0f);
const FVector2D FSETimelineTestStyle::LargeIconSize(24.0f, 24.0f);
const FVector2D FSETimelineTestStyle::ButtonSize(120.0f, 30.0f);
const FVector2D FSETimelineTestStyle::SmallButtonSize(80.0f, 24.0f);
const FVector2D FSETimelineTestStyle::LargeButtonSize(160.0f, 40.0f);
const FVector2D FSETimelineTestStyle::CheckBoxSize(20.0f, 20.0f);
const FVector2D FSETimelineTestStyle::RadioButtonSize(20.0f, 20.0f);
const FVector2D FSETimelineTestStyle::ScrollBarSize(8.0f, 8.0f);
const FVector2D FSETimelineTestStyle::ProgressBarSize(200.0f, 4.0f);
const FVector2D FSETimelineTestStyle::BorderSize(1.0f, 1.0f);
const FVector2D FSETimelineTestStyle::SeparatorSize(1.0f, 1.0f);

const FMargin FSETimelineTestStyle::DefaultMargin(8.0f);
const FMargin FSETimelineTestStyle::SmallMargin(4.0f);
const FMargin FSETimelineTestStyle::LargeMargin(16.0f);
const FMargin FSETimelineTestStyle::ButtonMargin(4.0f, 2.0f);
const FMargin FSETimelineTestStyle::PanelMargin(12.0f);
const FMargin FSETimelineTestStyle::ContentMargin(20.0f);
const FMargin FSETimelineTestStyle::HeaderMargin(0.0f, 0.0f, 0.0f, 16.0f);
const FMargin FSETimelineTestStyle::FooterMargin(0.0f, 16.0f, 0.0f, 0.0f);

const FMargin FSETimelineTestStyle::DefaultPadding(8.0f);
const FMargin FSETimelineTestStyle::SmallPadding(4.0f);
const FMargin FSETimelineTestStyle::LargePadding(16.0f);
const FMargin FSETimelineTestStyle::ButtonPadding(12.0f, 6.0f);
const FMargin FSETimelineTestStyle::PanelPadding(16.0f);
const FMargin FSETimelineTestStyle::ContentPadding(24.0f);
const FMargin FSETimelineTestStyle::HeaderPadding(0.0f, 0.0f, 0.0f, 20.0f);
const FMargin FSETimelineTestStyle::FooterPadding(0.0f, 20.0f, 0.0f, 0.0f);

const FSlateRoundedCornerInfo FSETimelineTestStyle::DefaultCornerRadius(4.0f);
const FSlateRoundedCornerInfo FSETimelineTestStyle::SmallCornerRadius(2.0f);
const FSlateRoundedCornerInfo FSETimelineTestStyle::LargeCornerRadius(8.0f);
const FSlateRoundedCornerInfo FSETimelineTestStyle::ButtonCornerRadius(4.0f);
const FSlateRoundedCornerInfo FSETimelineTestStyle::PanelCornerRadius(6.0f);

const float FSETimelineTestStyle::TransitionDuration = 0.2f;
const float FSETimelineTestStyle::AnimationDuration = 0.3f;
const float FSETimelineTestStyle::TooltipDelay = 0.5f;
const float FSETimelineTestStyle::FadeInDuration = 0.2f;
const float FSETimelineTestStyle::FadeOutDuration = 0.2f;
const float FSETimelineTestStyle::SlideInDuration = 0.3f;
const float FSETimelineTestStyle::SlideOutDuration = 0.3f;
const float FSETimelineTestStyle::ScaleInDuration = 0.2f;
const float FSETimelineTestStyle::ScaleOutDuration = 0.2f;

FSETimelineTestStyle& FSETimelineTestStyle::Get()
{
    static FSETimelineTestStyle Instance;
    return Instance;
}

void FSETimelineTestStyle::Initialize()
{
    if (!StyleSet.IsValid())
    {
        StyleSet = MakeShared<FSlateStyleSet>(GetStyleSetName());
        StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("ShadowEchoes"))->GetBaseDir() / TEXT("Resources"));

        RegisterStyles();
        RegisterColors();
        RegisterFonts();
        RegisterBrushes();
        RegisterTextStyles();
        RegisterButtonStyles();
        RegisterPanelStyles();
        RegisterWidgetStyles();

        FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
    }
}

void FSETimelineTestStyle::Shutdown()
{
    if (StyleSet.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
        StyleSet.Reset();
    }
}

FName FSETimelineTestStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("TimelineTestStyle"));
    return StyleSetName;
}

const ISlateStyle& FSETimelineTestStyle::GetStyle() const
{
    return *StyleSet.Get();
}

const FTextBlockStyle& FSETimelineTestStyle::GetStatusStyle(SETimelineTest::ETestStatus Status)
{
    switch (Status)
    {
        case SETimelineTest::ETestStatus::Passed:
            return StyleSet->GetWidgetStyle<FTextBlockStyle>(TEXT("TimelineTest.Status.Passed"));
        case SETimelineTest::ETestStatus::Failed:
            return StyleSet->GetWidgetStyle<FTextBlockStyle>(TEXT("TimelineTest.Status.Failed"));
        case SETimelineTest::ETestStatus::Skipped:
            return StyleSet->GetWidgetStyle<FTextBlockStyle>(TEXT("TimelineTest.Status.Skipped"));
        default:
            return StyleSet->GetWidgetStyle<FTextBlockStyle>(TEXT("TimelineTest.Status.Default"));
    }
}

const FLinearColor& FSETimelineTestStyle::GetStatusColor(SETimelineTest::ETestStatus Status)
{
    switch (Status)
    {
        case SETimelineTest::ETestStatus::Passed:
            return SuccessColor;
        case SETimelineTest::ETestStatus::Failed:
            return ErrorColor;
        case SETimelineTest::ETestStatus::Skipped:
            return WarningColor;
        default:
            return SecondaryColor;
    }
}

void FSETimelineTestStyle::RegisterStyles()
{
    // Register base styles
    StyleSet->Set("TimelineTest.Background", new BOX_BRUSH("Background", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.Panel", new BOX_BRUSH("Panel", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.Border", new BOX_BRUSH("Border", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.Separator", new BOX_BRUSH("Separator", FMargin(4.0f/16.0f)));
}

void FSETimelineTestStyle::RegisterColors()
{
    StyleSet->Set("TimelineTest.Primary", PrimaryColor);
    StyleSet->Set("TimelineTest.Secondary", SecondaryColor);
    StyleSet->Set("TimelineTest.Success", SuccessColor);
    StyleSet->Set("TimelineTest.Warning", WarningColor);
    StyleSet->Set("TimelineTest.Error", ErrorColor);
    StyleSet->Set("TimelineTest.Info", InfoColor);
    StyleSet->Set("TimelineTest.Light", LightColor);
    StyleSet->Set("TimelineTest.Dark", DarkColor);
}

void FSETimelineTestStyle::RegisterFonts()
{
    StyleSet->Set("TimelineTest.Font.Header", HeaderFont);
    StyleSet->Set("TimelineTest.Font.Title", TitleFont);
    StyleSet->Set("TimelineTest.Font.Subtitle", SubtitleFont);
    StyleSet->Set("TimelineTest.Font.Body", BodyFont);
    StyleSet->Set("TimelineTest.Font.Small", SmallFont);
    StyleSet->Set("TimelineTest.Font.Mono", MonoFont);
}

void FSETimelineTestStyle::RegisterBrushes()
{
    // Register button brushes
    StyleSet->Set("TimelineTest.Button.Normal", new BOX_BRUSH("Button", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.Button.Hovered", new BOX_BRUSH("Button_Hovered", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.Button.Pressed", new BOX_BRUSH("Button_Pressed", FMargin(4.0f/16.0f)));

    // Register checkbox brushes
    StyleSet->Set("TimelineTest.CheckBox", new IMAGE_BRUSH("CheckBox", CheckBoxSize));
    StyleSet->Set("TimelineTest.CheckBox.Checked", new IMAGE_BRUSH("CheckBox_Checked", CheckBoxSize));

    // Register radio button brushes
    StyleSet->Set("TimelineTest.RadioButton", new IMAGE_BRUSH("RadioButton", RadioButtonSize));
    StyleSet->Set("TimelineTest.RadioButton.Selected", new IMAGE_BRUSH("RadioButton_Selected", RadioButtonSize));

    // Register scrollbar brushes
    StyleSet->Set("TimelineTest.ScrollBar", new BOX_BRUSH("ScrollBar", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.ScrollBar.Thumb", new BOX_BRUSH("ScrollBar_Thumb", FMargin(4.0f/16.0f)));

    // Register progress bar brushes
    StyleSet->Set("TimelineTest.ProgressBar", new BOX_BRUSH("ProgressBar", FMargin(4.0f/16.0f)));
    StyleSet->Set("TimelineTest.ProgressBar.Fill", new BOX_BRUSH("ProgressBar_Fill", FMargin(4.0f/16.0f)));
}

void FSETimelineTestStyle::RegisterTextStyles()
{
    // Register text styles for different test statuses
    StyleSet->Set("TimelineTest.Status.Default", CreateTextStyle(BodyFont, SecondaryColor));
    StyleSet->Set("TimelineTest.Status.Passed", CreateTextStyle(BodyFont, SuccessColor));
    StyleSet->Set("TimelineTest.Status.Failed", CreateTextStyle(BodyFont, ErrorColor));
    StyleSet->Set("TimelineTest.Status.Skipped", CreateTextStyle(BodyFont, WarningColor));

    // Register text styles for different test categories
    StyleSet->Set("TimelineTest.Category.Core", CreateTextStyle(BodyFont, PrimaryColor));
    StyleSet->Set("TimelineTest.Category.Visual", CreateTextStyle(BodyFont, InfoColor));
    StyleSet->Set("TimelineTest.Category.Performance", CreateTextStyle(BodyFont, WarningColor));
    StyleSet->Set("TimelineTest.Category.Integration", CreateTextStyle(BodyFont, SuccessColor));
    StyleSet->Set("TimelineTest.Category.Stress", CreateTextStyle(BodyFont, ErrorColor));

    // Register text styles for different test priorities
    StyleSet->Set("TimelineTest.Priority.Critical", CreateTextStyle(BodyFont, ErrorColor));
    StyleSet->Set("TimelineTest.Priority.High", CreateTextStyle(BodyFont, WarningColor));
    StyleSet->Set("TimelineTest.Priority.Medium", CreateTextStyle(BodyFont, InfoColor));
    StyleSet->Set("TimelineTest.Priority.Low", CreateTextStyle(BodyFont, SecondaryColor));
}

void FSETimelineTestStyle::RegisterButtonStyles()
{
    // Register button styles
    StyleSet->Set("TimelineTest.Button", CreateButtonStyle(
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Normal,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Hovered,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Pressed,
        ButtonPadding
    ));

    // Register small button styles
    StyleSet->Set("TimelineTest.Button.Small", CreateButtonStyle(
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Normal,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Hovered,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Pressed,
        SmallPadding
    ));

    // Register large button styles
    StyleSet->Set("TimelineTest.Button.Large", CreateButtonStyle(
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Normal,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Hovered,
        &StyleSet->GetWidgetStyle<FButtonStyle>("Button").Pressed,
        LargePadding
    ));
}

void FSETimelineTestStyle::RegisterPanelStyles()
{
    // Register panel styles
    StyleSet->Set("TimelineTest.Panel", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        PanelPadding
    ));

    // Register header panel styles
    StyleSet->Set("TimelineTest.Panel.Header", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        HeaderPadding
    ));

    // Register content panel styles
    StyleSet->Set("TimelineTest.Panel.Content", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        ContentPadding
    ));
}

void FSETimelineTestStyle::RegisterWidgetStyles()
{
    // Register widget styles
    StyleSet->Set("TimelineTest.Widget", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        DefaultPadding
    ));

    // Register widget header styles
    StyleSet->Set("TimelineTest.Widget.Header", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        HeaderPadding
    ));

    // Register widget content styles
    StyleSet->Set("TimelineTest.Widget.Content", CreatePanelStyle(
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").EvenRowBackgroundBrush,
        &StyleSet->GetWidgetStyle<FTableRowStyle>("TableView.Row").SelectorFocusedBrush,
        ContentPadding
    ));
}

FTextBlockStyle FSETimelineTestStyle::CreateTextStyle(
    const FSlateFontInfo& Font,
    const FLinearColor& Color,
    const FLinearColor& HighlightColor)
{
    FTextBlockStyle Style;
    Style.SetFont(Font);
    Style.SetColorAndOpacity(FSlateColor(Color));
    Style.SetHighlightColor(FSlateColor(HighlightColor));
    return Style;
}

FButtonStyle FSETimelineTestStyle::CreateButtonStyle(
    const FSlateBrush* Normal,
    const FSlateBrush* Hovered,
    const FSlateBrush* Pressed,
    const FMargin& Padding)
{
    FButtonStyle Style;
    Style.SetNormal(*Normal);
    Style.SetHovered(*Hovered);
    Style.SetPressed(*Pressed);
    Style.SetPadding(Padding);
    return Style;
}

FTableRowStyle FSETimelineTestStyle::CreatePanelStyle(
    const FSlateBrush* Background,
    const FSlateBrush* Border,
    const FMargin& Padding)
{
    FTableRowStyle Style;
    Style.SetEvenRowBackgroundBrush(*Background);
    Style.SetSelectorFocusedBrush(*Border);
    Style.SetPadding(Padding);
    return Style;
}
