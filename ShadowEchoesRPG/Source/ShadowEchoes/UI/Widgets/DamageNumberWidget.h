// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/SEBaseWidget.h"
#include "Core/SETypes.h"
#include "DamageNumberWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UENUM(BlueprintType)
enum class EDamageNumberType : uint8
{
    Normal      UMETA(DisplayName = "Normal"),
    Critical    UMETA(DisplayName = "Critical"),
    Timeline    UMETA(DisplayName = "Timeline"),
    Healing     UMETA(DisplayName = "Healing")
};

/**
 * Widget for displaying floating damage numbers
 */
UCLASS()
class SHADOWECHOES_API UDamageNumberWidget : public USEBaseWidget
{
    GENERATED_BODY()

public:
    UDamageNumberWidget(const FObjectInitializer& ObjectInitializer);

    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /** Initialize damage number */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void SetDamageNumber(float Damage, EDamageNumberType Type = EDamageNumberType::Normal);

    /** Movement settings */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void SetMovementDirection(const FVector2D& Direction);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|UI|Combat")
    void SetStartPosition(const FVector2D& Position);

protected:
    /** UI Components */
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* DamageText;

    /** Animations */
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* FadeAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ScaleAnimation;

    /** Visual settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor NormalColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor CriticalColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor TimelineColor;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Style")
    FLinearColor HealingColor;

    /** Movement settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Movement")
    float MovementSpeed;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Movement")
    float Lifetime;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Movement")
    UCurveFloat* MovementCurve;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Movement")
    UCurveFloat* ScaleCurve;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|UI|Movement")
    UCurveFloat* OpacityCurve;

private:
    /** Current state */
    float CurrentDamage;
    EDamageNumberType DamageType;
    float CurrentLifetime;
    FVector2D CurrentPosition;
    FVector2D MovementDir;
    bool bIsActive;

    /** Initialize widget */
    void InitializeWidget();

    /** Update movement */
    void UpdateMovement(float DeltaTime);
    void UpdateScale(float Progress);
    void UpdateOpacity(float Progress);

    /** Visual helpers */
    FLinearColor GetDamageColor() const;
    FText GetDamageText() const;

    /** Animation handlers */
    UFUNCTION()
    void OnFadeAnimationFinished();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnDamageSet(float Damage, EDamageNumberType Type);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|UI|Combat")
    void BP_OnLifetimeExpired();
};
