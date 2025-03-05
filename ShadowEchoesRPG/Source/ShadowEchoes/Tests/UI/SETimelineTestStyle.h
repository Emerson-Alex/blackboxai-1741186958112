#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

/**
 * Style set for Timeline post-process test UI elements
 */
class FSETimelineTestStyle
{
public:
    static void Initialize();
    static void Shutdown();
    static const ISlateStyle& Get();
    static FName GetStyleSetName();

private:
    static TSharedRef<FSlateStyleSet> Create();
    static TSharedPtr<FSlateStyleSet> StyleInstance;

    /** Style constants */
    struct Colors
    {
        static const FLinearColor Background;
        static const FLinearColor Border;
        static const FLinearColor HeaderBackground;
        static const FLinearColor Success;
        static const FLinearColor Failure;
        static const FLinearColor Warning;
        static const FLinearColor Neutral;
        static const FLinearColor ProgressBarFill;
        static const FLinearColor ProgressBarBackground;
    };

    struct Dimensions
    {
        static const FVector2D IconSize;
        static const FVector2D ButtonSize;
        static const FVector2D MinEntrySize;
        static const float BorderRadius;
        static const float ProgressBarHeight;
    };

    struct Fonts
    {
        static const FSlateFontInfo HeaderFont;
        static const FSlateFontInfo NormalFont;
        static const FSlateFontInfo SmallFont;
        static const FSlateFontInfo MonoFont;
    };

    struct Brushes
    {
        static const FName Background;
        static const FName Border;
        static const FName HeaderBackground;
        static const FName EntryBackground;
        static const FName ProgressBarFill;
        static const FName ProgressBarBackground;
        static const FName SuccessIcon;
        static const FName FailureIcon;
        static const FName WarningIcon;
        static const FName ExpandIcon;
        static const FName CollapseIcon;
    };

    struct Margins
    {
        static const FMargin Default;
        static const FMargin Header;
        static const FMargin Content;
        static const FMargin Entry;
        static const FMargin Icon;
    };

    /** Helper functions */
    static void RegisterBrushes(TSharedRef<FSlateStyleSet>& StyleSet);
    static void RegisterFonts(TSharedRef<FSlateStyleSet>& StyleSet);
    static void RegisterColors(TSharedRef<FSlateStyleSet>& StyleSet);
    static void RegisterWidgetStyles(TSharedRef<FSlateStyleSet>& StyleSet);

    /** Create brushes */
    static FSlateBrush* CreateBoxBrush(const FString& ResourcePath, const FMargin& Margin, const FLinearColor& Color);
    static FSlateBrush* CreateImageBrush(const FString& ResourcePath, const FVector2D& Size);

    /** Resource paths */
    static const FString ResourcePath;
    static FString GetResourcePath(const FString& RelativePath);
};
