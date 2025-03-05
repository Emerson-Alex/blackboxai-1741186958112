// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "SEUserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"

const FName USEUserWidget::LogCategory = TEXT("ShadowEchoes.UserWidget");

USEUserWidget::USEUserWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ShowAnimation(nullptr)
    , HideAnimation(nullptr)
    , CurrentOpacity(1.0f)
    , bIsVisible(true)
    , bIsAnimating(false)
    , ShowAnimationName(TEXT("ShowAnimation"))
    , HideAnimationName(TEXT("HideAnimation"))
{
}

void USEUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize widget
    InitializeStyle();
    SetupAnimations();
    BindEvents();

    // Setup animation bindings
    SetupAnimationBindings();
}

void USEUserWidget::NativeDestruct()
{
    // Clean up
    UnbindEvents();
    CleanupAnimationBindings();

    Super::NativeDestruct();
}

void USEUserWidget::SetupAnimations()
{
    // Get animations by name
    ShowAnimation = GetAnimationByName(ShowAnimationName);
    HideAnimation = GetAnimationByName(HideAnimationName);

    // Log warning if animations are not found
    if (!ShowAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Show animation not found: %s"), 
            *LogCategory.ToString(), *ShowAnimationName.ToString());
    }

    if (!HideAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Hide animation not found: %s"), 
            *LogCategory.ToString(), *HideAnimationName.ToString());
    }
}

void USEUserWidget::PlayShowAnimation()
{
    if (!ShowAnimation || bIsAnimating)
    {
        return;
    }

    bIsAnimating = true;
    PlayAnimation(ShowAnimation);
}

void USEUserWidget::PlayHideAnimation()
{
    if (!HideAnimation || bIsAnimating)
    {
        return;
    }

    bIsAnimating = true;
    PlayAnimation(HideAnimation);
}

void USEUserWidget::ShowWidget()
{
    if (!bIsVisible)
    {
        bIsVisible = true;
        SetVisibility(ESlateVisibility::Visible);
        PlayShowAnimation();
    }
}

void USEUserWidget::HideWidget()
{
    if (bIsVisible)
    {
        PlayHideAnimation();
        // Actual visibility change happens in animation finished callback
    }
}

void USEUserWidget::SetWidgetOpacity(float NewOpacity)
{
    CurrentOpacity = FMath::Clamp(NewOpacity, 0.0f, 1.0f);

    // Apply opacity to all child widgets
    if (UPanelWidget* RootPanel = Cast<UPanelWidget>(GetRootWidget()))
    {
        for (int32 i = 0; i < RootPanel->GetChildrenCount(); ++i)
        {
            if (UWidget* Child = RootPanel->GetChildAt(i))
            {
                Child->SetRenderOpacity(CurrentOpacity);
            }
        }
    }
}

void USEUserWidget::OnAnimationFinished(const UWidgetAnimation* Animation)
{
    bIsAnimating = false;

    if (Animation == HideAnimation)
    {
        bIsVisible = false;
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void USEUserWidget::InitializeStyle()
{
    // Apply default style
    FSEWidgetStyle DefaultStyle;

    // Apply background color
    if (UPanelWidget* RootPanel = Cast<UPanelWidget>(GetRootWidget()))
    {
        // TODO: Apply style to root panel
    }

    // Apply text style to all text widgets
    WidgetTree->ForEachWidget([&](UWidget* Widget) {
        // TODO: Apply style to text widgets
    });
}

void USEUserWidget::BindEvents()
{
    // Override in child classes to bind specific events
}

void USEUserWidget::UnbindEvents()
{
    // Override in child classes to unbind specific events
}

UWidgetAnimation* USEUserWidget::GetAnimationByName(const FName& AnimationName) const
{
    UWidgetAnimation* const* FoundAnim = Animations.FindByPredicate(
        [&AnimationName](const UWidgetAnimation* Anim) {
            return Anim && Anim->GetFName() == AnimationName;
        });

    return FoundAnim ? *FoundAnim : nullptr;
}

void USEUserWidget::SetupAnimationBindings()
{
    // Bind animation finished delegates
    if (ShowAnimation)
    {
        ShowAnimationFinishedDelegate.BindDynamic(this, &USEUserWidget::OnAnimationFinished);
        BindToAnimationFinished(ShowAnimation, ShowAnimationFinishedDelegate);
    }

    if (HideAnimation)
    {
        HideAnimationFinishedDelegate.BindDynamic(this, &USEUserWidget::OnAnimationFinished);
        BindToAnimationFinished(HideAnimation, HideAnimationFinishedDelegate);
    }
}

void USEUserWidget::CleanupAnimationBindings()
{
    // Unbind animation finished delegates
    if (ShowAnimation)
    {
        UnbindFromAnimationFinished(ShowAnimation, ShowAnimationFinishedDelegate);
    }

    if (HideAnimation)
    {
        UnbindFromAnimationFinished(HideAnimation, HideAnimationFinishedDelegate);
    }

    ShowAnimationFinishedDelegate.Clear();
    HideAnimationFinishedDelegate.Clear();
}

#if WITH_EDITOR
void USEUserWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Handle property changes in editor
    const FName PropertyName = (PropertyChangedEvent.Property != nullptr) 
        ? PropertyChangedEvent.Property->GetFName() 
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(USEUserWidget, ShowAnimationName) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(USEUserWidget, HideAnimationName))
    {
        SetupAnimations();
    }
}
#endif

// Example of how to create widget animations in code
// Note: This is typically done in the UMG editor, but this shows how it can be done programmatically
void USEUserWidget::CreateDefaultAnimations()
{
    // Create show animation
    if (!ShowAnimation)
    {
        ShowAnimation = NewObject<UWidgetAnimation>(this, ShowAnimationName);
        if (ShowAnimation)
        {
            // Add fade-in track
            FMovieSceneFloatChannel* OpacityChannel = nullptr;
            // TODO: Setup animation tracks
            
            // Set animation properties
            ShowAnimation->MovieScene->SetDisplayRate(FFrameRate(60, 1));
            ShowAnimation->MovieScene->SetPlaybackRange(0, 30); // 0.5 seconds at 60 fps
        }
    }

    // Create hide animation
    if (!HideAnimation)
    {
        HideAnimation = NewObject<UWidgetAnimation>(this, HideAnimationName);
        if (HideAnimation)
        {
            // Add fade-out track
            FMovieSceneFloatChannel* OpacityChannel = nullptr;
            // TODO: Setup animation tracks
            
            // Set animation properties
            HideAnimation->MovieScene->SetDisplayRate(FFrameRate(60, 1));
            HideAnimation->MovieScene->SetPlaybackRange(0, 30); // 0.5 seconds at 60 fps
        }
    }
}
