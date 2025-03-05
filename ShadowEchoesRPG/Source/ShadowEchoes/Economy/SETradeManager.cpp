// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Economy/SETradeManager.h"
#include "Core/SEGameInstance.h"
#include "Systems/TimelineManager.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

USETradeManager::USETradeManager()
    : MarketUpdateInterval(300.0f)  // 5 minutes
    , AuctionDuration(86400.0f)     // 24 hours
    , MaxPriceFluctuation(0.5f)     // 50% max price change
{
}

void USETradeManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
    if (GameInstance)
    {
        TimelineManager = GameInstance->GetTimelineManager();
    }

    // Initialize market data
    CurrentMarketData.MarketVolatility = 0.1f;
    CurrentMarketData.TimelineInfluence = 1.0f;

    // Start market update loop
    FTimerHandle UpdateTimer;
    GetWorld()->GetTimerManager().SetTimer(
        UpdateTimer,
        this,
        &USETradeManager::UpdateMarketPrices,
        MarketUpdateInterval,
        true
    );

    // Start auction processing
    FTimerHandle AuctionTimer;
    GetWorld()->GetTimerManager().SetTimer(
        AuctionTimer,
        this,
        &USETradeManager::ProcessAuctions,
        60.0f,  // Check auctions every minute
        true
    );
}

bool USETradeManager::ListItem(const FString& SellerID, const FMarketItem& Item)
{
    // Validate item data
    if (Item.Quantity <= 0 || Item.BasePrice <= 0)
    {
        return false;
    }

    // Generate unique item listing ID
    FString ItemID = FGuid::NewGuid().ToString();
    
    // Apply initial price modifier based on market conditions
    FMarketItem NewListing = Item;
    NewListing.ItemID = ItemID;
    NewListing.SellerID = SellerID;
    NewListing.CurrentPriceModifier = CalculatePriceModifier(Item);
    
    if (Item.bIsAuction)
    {
        NewListing.TimeRemaining = AuctionDuration;
    }

    // Add to market
    ListedItems.Add(ItemID, NewListing);

    // Update supply metrics
    if (!CurrentMarketData.ItemSupply.Contains(Item.ItemID))
    {
        CurrentMarketData.ItemSupply.Add(Item.ItemID, 0);
    }
    CurrentMarketData.ItemSupply[Item.ItemID] += Item.Quantity;

    // Update market data
    UpdateMarketPrices();

    return true;
}

bool USETradeManager::BuyItem(const FString& BuyerID, const FString& ItemID)
{
    if (!ListedItems.Contains(ItemID))
    {
        return false;
    }

    FMarketItem& Item = ListedItems[ItemID];
    
    // Validate trade
    if (!ValidateTrade(BuyerID, Item))
    {
        return false;
    }

    // Process transaction
    float CurrentPrice = GetCurrentPrice(ItemID);
    
    // Update market data
    if (!CurrentMarketData.ItemDemand.Contains(Item.ItemID))
    {
        CurrentMarketData.ItemDemand.Add(Item.ItemID, 0);
    }
    CurrentMarketData.ItemDemand[Item.ItemID]++;
    CurrentMarketData.ItemSupply[Item.ItemID] -= Item.Quantity;

    // Record transaction
    if (!ItemHistory.Contains(Item.ItemID))
    {
        ItemHistory.Add(Item.ItemID, TArray<FString>());
    }
    ItemHistory[Item.ItemID].Add(BuyerID);

    // Remove listing
    ListedItems.Remove(ItemID);

    // Notify completion
    OnTradeCompleted.Broadcast(ItemID, BuyerID);
    BP_OnTradeCompleted(ItemID, BuyerID);

    // Update market
    UpdateMarketPrices();

    return true;
}

bool USETradeManager::PlaceBid(const FString& BidderID, const FString& ItemID, int32 BidAmount)
{
    if (!ListedItems.Contains(ItemID))
    {
        return false;
    }

    FMarketItem& Item = ListedItems[ItemID];
    
    if (!Item.bIsAuction)
    {
        return false;
    }

    // Validate bid amount
    float CurrentPrice = GetCurrentPrice(ItemID);
    if (BidAmount <= CurrentPrice)
    {
        return false;
    }

    // Add bid
    Item.Bids.Add(BidderID);
    Item.BasePrice = BidAmount;  // Update base price to current bid

    // Update market volatility
    CurrentMarketData.MarketVolatility += 0.01f;
    CurrentMarketData.MarketVolatility = FMath::Min(CurrentMarketData.MarketVolatility, 1.0f);

    return true;
}

float USETradeManager::GetCurrentPrice(const FString& ItemID) const
{
    if (!ListedItems.Contains(ItemID))
    {
        return 0.0f;
    }

    const FMarketItem& Item = ListedItems[ItemID];
    return Item.BasePrice * Item.CurrentPriceModifier;
}

void USETradeManager::UpdateMarketPrices()
{
    // Update market volatility
    UpdateMarketVolatility();

    // Process market events
    ProcessMarketEvents();

    // Update each item's price
    for (auto& Pair : ListedItems)
    {
        FMarketItem& Item = Pair.Value;
        Item.CurrentPriceModifier = CalculatePriceModifier(Item);
    }

    // Update supply and demand
    UpdateSupplyAndDemand();

    // Calculate market trends
    CalculateMarketTrends();

    // Clean up old listings
    CleanupExpiredListings();

    // Notify market update
    OnMarketUpdate.Broadcast(CurrentMarketData);
    BP_OnMarketUpdate(CurrentMarketData);
}

void USETradeManager::OnTimelineStateChanged(ETimelineState NewState)
{
    // Apply timeline-specific effects
    ApplyTimelineEffects(NewState);

    // Update market prices
    UpdateMarketPrices();

    BP_OnTimelineStateChanged(NewState);
}

TArray<FMarketItem> USETradeManager::GetListedItems(ETradeItemType Type) const
{
    TArray<FMarketItem> Items;
    
    for (const auto& Pair : ListedItems)
    {
        if (Pair.Value.Type == Type)
        {
            Items.Add(Pair.Value);
        }
    }

    return Items;
}

void USETradeManager::ProcessAuctions()
{
    float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    TArray<FString> CompletedAuctions;

    // Check each auction
    for (auto& Pair : ListedItems)
    {
        FMarketItem& Item = Pair.Value;
        
        if (!Item.bIsAuction)
        {
            continue;
        }

        Item.TimeRemaining -= 60.0f;  // Reduce time by one minute

        if (Item.TimeRemaining <= 0)
        {
            CompletedAuctions.Add(Pair.Key);
            
            // Find winner
            if (Item.Bids.Num() > 0)
            {
                FString WinnerID = Item.Bids.Last();
                OnAuctionEnded.Broadcast(Item.ItemID, WinnerID);
                BP_OnAuctionEnded(Item.ItemID, WinnerID);
            }
        }
    }

    // Remove completed auctions
    for (const FString& AuctionID : CompletedAuctions)
    {
        ListedItems.Remove(AuctionID);
    }
}

void USETradeManager::UpdateSupplyAndDemand()
{
    for (auto& Pair : CurrentMarketData.ItemSupply)
    {
        const FString& ItemID = Pair.Key;
        
        // Calculate supply ratio
        float SupplyRatio = Pair.Value / 100.0f;  // Base value of 100 items
        
        // Get demand
        int32 Demand = CurrentMarketData.ItemDemand.Contains(ItemID) ? 
            CurrentMarketData.ItemDemand[ItemID] : 0;
        
        // Calculate demand ratio
        float DemandRatio = Demand / 100.0f;

        // Update price modifiers
        if (!CurrentMarketData.PriceModifiers.Contains(ETradeItemType::Resource))
        {
            CurrentMarketData.PriceModifiers.Add(ETradeItemType::Resource, 1.0f);
        }

        float& Modifier = CurrentMarketData.PriceModifiers[ETradeItemType::Resource];
        Modifier = FMath::Clamp(DemandRatio / SupplyRatio, 0.5f, 2.0f);
    }
}

void USETradeManager::CalculateMarketTrends()
{
    // Analyze recent transactions
    for (const auto& Pair : ItemHistory)
    {
        const FString& ItemID = Pair.Key;
        const TArray<FString>& Transactions = Pair.Value;

        // Calculate transaction frequency
        float TransactionsPerHour = Transactions.Num() / (MarketUpdateInterval / 3600.0f);

        // Update market volatility based on transaction frequency
        CurrentMarketData.MarketVolatility = FMath::Lerp(
            CurrentMarketData.MarketVolatility,
            TransactionsPerHour / 100.0f,  // Normalize to 0-1 range
            0.1f  // Smooth changes
        );
    }
}

float USETradeManager::CalculatePriceModifier(const FMarketItem& Item) const
{
    float Modifier = 1.0f;

    // Apply supply/demand modifier
    if (CurrentMarketData.PriceModifiers.Contains(Item.Type))
    {
        Modifier *= CurrentMarketData.PriceModifiers[Item.Type];
    }

    // Apply timeline influence
    if (TimelineManager && Item.PreferredTimeline != ETimelineState::Any)
    {
        if (TimelineManager->GetCurrentState() == Item.PreferredTimeline)
        {
            Modifier *= 0.8f;  // 20% discount in preferred timeline
        }
        else
        {
            Modifier *= 1.2f;  // 20% markup in other timeline
        }
    }

    // Apply market volatility
    float VolatilityEffect = FMath::RandRange(-CurrentMarketData.MarketVolatility, CurrentMarketData.MarketVolatility);
    Modifier *= (1.0f + VolatilityEffect);

    // Clamp final modifier
    return FMath::Clamp(Modifier, 1.0f - MaxPriceFluctuation, 1.0f + MaxPriceFluctuation);
}

bool USETradeManager::ValidateTrade(const FString& BuyerID, const FMarketItem& Item) const
{
    // Check if item is in auction
    if (Item.bIsAuction)
    {
        return false;
    }

    // Check if buyer has required currency
    // This would be handled by the economy system

    // Check if buyer can receive item
    // This would be handled by the inventory system

    return true;
}

void USETradeManager::ApplyTimelineEffects(ETimelineState State)
{
    // Update timeline influence
    switch (State)
    {
        case ETimelineState::BrightWorld:
            CurrentMarketData.TimelineInfluence = 1.2f;  // Light timeline boosts prices
            break;

        case ETimelineState::DarkWorld:
            CurrentMarketData.TimelineInfluence = 0.8f;  // Dark timeline reduces prices
            break;

        default:
            CurrentMarketData.TimelineInfluence = 1.0f;
            break;
    }

    // Apply timeline-specific modifiers
    for (auto& Pair : CurrentMarketData.PriceModifiers)
    {
        Pair.Value *= CurrentMarketData.TimelineInfluence;
    }
}

void USETradeManager::UpdateMarketVolatility()
{
    // Decay volatility over time
    CurrentMarketData.MarketVolatility *= 0.95f;

    // Ensure minimum volatility
    CurrentMarketData.MarketVolatility = FMath::Max(CurrentMarketData.MarketVolatility, 0.05f);
}

void USETradeManager::ProcessMarketEvents()
{
    // Check for special events that affect market prices
    if (TimelineManager)
    {
        // Timeline events
        // World events
        // Guild activities
        // Raid completions
    }
}

void USETradeManager::CleanupExpiredListings()
{
    TArray<FString> ExpiredListings;

    float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    
    for (const auto& Pair : ListedItems)
    {
        if (!Pair.Value.bIsAuction && CurrentTime > Pair.Value.TimeRemaining)
        {
            ExpiredListings.Add(Pair.Key);
        }
    }

    for (const FString& ListingID : ExpiredListings)
    {
        ListedItems.Remove(ListingID);
    }
}
