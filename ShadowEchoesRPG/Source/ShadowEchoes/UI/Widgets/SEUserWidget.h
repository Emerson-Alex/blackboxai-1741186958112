// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SEUserWidget.generated.h"

class UWidgetAnimation;

/**
 * USEUserWidget - Base widget class for Shadow Echoes
 * Provides common functionality and styling for all UI widgets
 */
UCLASS()
class SHADOWECHOES_API USEUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    USEUserWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Setup widget animations */
    virtual void SetupAnimations();

    /** Play show animation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void PlayShowAnimation();

    /** Play hide animation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void PlayHideAnimation();

    /** Show the widget with animation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void ShowWidget();

    /** Hide the widget with animation */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void HideWidget();

    /** Set widget opacity */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI")
    virtual void SetWidgetOpacity(float NewOpacity);

    /** Get current opacity */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|UI")
    float GetWidgetOpacity() const { return CurrentOpacity; }

protected:
    /** Show animation */
    UPROPERTY(Transient)
    UWidgetAnimation* ShowAnimation;

    /** Hide animation */
    UPROPERTY(Transient)
    UWidgetAnimation* HideAnimation;

    /** Animation names */
    UPROPERTY(EditDefaultsOnly, Category = "Animations")
    FName ShowAnimationName;

    UPROPERTY(EditDefaultsOnly, Category = "Animations")
    FName HideAnimationName;

    /** Current widget opacity */
    UPROPERTY()
    float CurrentOpacity;

    /** Whether widget is currently visible */
    UPROPERTY()
    bool bIsVisible;

    /** Whether animations are currently playing */
    UPROPERTY()
    bool bIsAnimating;

    /** Handle animation finished */
    UFUNCTION()
    virtual void OnAnimationFinished(const UWidgetAnimation* Animation);

    /** Initialize widget style */
    virtual void InitializeStyle();

    /** Bind widget events */
    virtual void BindEvents();

    /** Unbind widget events */
    virtual void UnbindEvents();

    /** Get animation by name */
    UWidgetAnimation* GetAnimationByName(const FName& AnimationName) const;

private:
    /** Logger category */
    static const FName LogCategory;

    /** Animation completion delegates */
    FWidgetAnimationDynamicEvent ShowAnimationFinishedDelegate;
    FWidgetAnimationDynamicEvent HideAnimationFinishedDelegate;

    /** Setup animation bindings */
    void SetupAnimationBindings();

    /** Clean up animation bindings */
    void CleanupAnimationBindings();
};

/**
 * FSEWidgetStyle - Common styling for Shadow Echoes widgets
 */
USTRUCT(BlueprintType)
struct FSEWidgetStyle
{
    GENERATED_BODY()

    /** Background color */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FLinearColor BackgroundColor;

    /** Text color */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FLinearColor TextColor;

    /** Border color */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FLinearColor BorderColor;

    /** Accent color */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FLinearColor AccentColor;

    /** Font size */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    float FontSize;

    /** Border thickness */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    float BorderThickness;

    /** Corner radius */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    float CornerRadius;

    /** Padding */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
    FMargin Padding;

    FSEWidgetStyle()
        : BackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f))
        , TextColor(FLinearColor::White)
        , BorderColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f))
        , AccentColor(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f))
        , FontSize(16.0f)
        , BorderThickness(1.0f)
        , CornerRadius(4.0f)
        , Padding(FMargin(10.0f))
    {}
};

/**
 * FSEWidgetAnimation - Animation settings for Shadow Echoes widgets
 */
USTRUCT(BlueprintType)
struct FSEWidgetAnimation
{
    GENERATED_BODY()

    /** Animation duration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float Duration;

    /** Start value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float StartValue;

    /** End value */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float EndValue;

    /** Animation curve */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UCurveFloat* AnimationCurve;

    FSEWidgetAnimation()
        : Duration(0.3f)
        , StartValue(0.0f)
        , EndValue(1.0f)
        , AnimationCurve(nullptr)
    {}
};
