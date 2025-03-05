// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SETradeManager.generated.h"

class USEGameInstance;
class UTimelineManager;

UENUM(BlueprintType)
enum class ETradeItemType : uint8
{
    Resource        UMETA(DisplayName = "Resource"),
    Equipment       UMETA(DisplayName = "Equipment"),
    Consumable      UMETA(DisplayName = "Consumable"),
    TimelineEssence UMETA(DisplayName = "Timeline Essence"),
    Cosmetic        UMETA(DisplayName = "Cosmetic")
};

USTRUCT(BlueprintType)
struct FMarketItem
{
    GENERATED_BODY()

    UPROPERTY()
    FString ItemID;

    UPROPERTY()
    FString SellerID;

    UPROPERTY()
    ETradeItemType Type;

    UPROPERTY()
    int32 Quantity;

    UPROPERTY()
    int32 BasePrice;

    UPROPERTY()
    float CurrentPriceModifier;

    UPROPERTY()
    ETimelineState PreferredTimeline;

    UPROPERTY()
    bool bIsAuction;

    UPROPERTY()
    float TimeRemaining;

    UPROPERTY()
    TArray<FString> Bids;
};

USTRUCT(BlueprintType)
struct FMarketData
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<ETradeItemType, float> PriceModifiers;

    UPROPERTY()
    TMap<FString, int32> ItemSupply;

    UPROPERTY()
    TMap<FString, int32> ItemDemand;

    UPROPERTY()
    float TimelineInfluence;

    UPROPERTY()
    float MarketVolatility;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMarketUpdate, const FMarketData&, MarketData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTradeCompleted, const FString&, ItemID, const FString&, BuyerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAuctionEnded, const FString&, ItemID, const FString&, WinnerID);

/**
 * Manages the trading post system with timeline-based market dynamics
 */
UCLASS()
class SHADOWECHOES_API USETradeManager : public UObject
{
    GENERATED_BODY()

public:
    USETradeManager();

    /** Initialize the trade system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Market operations */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    bool ListItem(const FString& SellerID, const FMarketItem& Item);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    bool BuyItem(const FString& BuyerID, const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    bool PlaceBid(const FString& BidderID, const FString& ItemID, int32 BidAmount);

    /** Market analysis */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    float GetCurrentPrice(const FString& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    void UpdateMarketPrices();

    /** Timeline integration */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    void OnTimelineStateChanged(ETimelineState NewState);

    /** Market status */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Trade")
    const FMarketData& GetMarketData() const { return CurrentMarketData; }

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Trade")
    TArray<FMarketItem> GetListedItems(ETradeItemType Type) const;

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Trade|Events")
    FOnMarketUpdate OnMarketUpdate;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Trade|Events")
    FOnTradeCompleted OnTradeCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Trade|Events")
    FOnAuctionEnded OnAuctionEnded;

protected:
    /** Trade settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Trade")
    float MarketUpdateInterval;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Trade")
    float AuctionDuration;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Trade")
    float MaxPriceFluctuation;

private:
    /** Current state */
    UPROPERTY()
    TMap<FString, FMarketItem> ListedItems;

    UPROPERTY()
    FMarketData CurrentMarketData;

    UPROPERTY()
    TMap<FString, TArray<FString>> ItemHistory;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    UPROPERTY()
    UTimelineManager* TimelineManager;

    /** Internal functionality */
    void ProcessAuctions();
    void UpdateSupplyAndDemand();
    void CalculateMarketTrends();
    float CalculatePriceModifier(const FMarketItem& Item) const;
    bool ValidateTrade(const FString& BuyerID, const FMarketItem& Item) const;
    void ApplyTimelineEffects(ETimelineState State);
    void UpdateMarketVolatility();
    void ProcessMarketEvents();
    void CleanupExpiredListings();

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Trade|Events")
    void BP_OnMarketUpdate(const FMarketData& MarketData);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Trade|Events")
    void BP_OnTradeCompleted(const FString& ItemID, const FString& BuyerID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Trade|Events")
    void BP_OnAuctionEnded(const FString& ItemID, const FString& WinnerID);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Trade|Events")
    void BP_OnTimelineStateChanged(ETimelineState NewState);
};
