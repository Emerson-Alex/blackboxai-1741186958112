// Copyright Shadow Echoes RPG. All Rights Reserved.

#include "Combat/SEEquipmentManager.h"
#include "Core/SEGameInstance.h"

USEEquipmentManager::USEEquipmentManager()
    : UpgradeCostMultiplier(1.5f)
    , RarityStatMultiplier(1.2f)
{
}

void USEEquipmentManager::Initialize(USEGameInstance* InGameInstance)
{
    GameInstance = InGameInstance;
}

bool USEEquipmentManager::EquipItem(const FEquipmentInfo& Equipment)
{
    if (!ValidateEquipment(Equipment))
    {
        return false;
    }

    // Store previous equipment for comparison
    FEquipmentInfo* PreviousEquipment = EquippedItems.Find(Equipment.Slot);

    // Update equipment
    EquippedItems.Add(Equipment.Slot, Equipment);

    // Update stats
    UpdateTotalStats();

    // Notify events
    OnEquipmentChanged.Broadcast(Equipment.Slot, Equipment);
    BP_OnEquipmentChanged(Equipment.Slot, Equipment);

    return true;
}

bool USEEquipmentManager::UnequipItem(EEquipmentSlot Slot)
{
    if (!EquippedItems.Contains(Slot))
    {
        return false;
    }

    // Store equipment info before removal
    FEquipmentInfo UnequippedItem = EquippedItems[Slot];

    // Remove equipment
    EquippedItems.Remove(Slot);

    // Update stats
    UpdateTotalStats();

    // Create empty equipment info for event
    FEquipmentInfo EmptyEquipment;
    EmptyEquipment.Slot = Slot;

    // Notify events
    OnEquipmentChanged.Broadcast(Slot, EmptyEquipment);
    BP_OnEquipmentChanged(Slot, EmptyEquipment);

    return true;
}

const FEquipmentInfo* USEEquipmentManager::GetEquippedItem(EEquipmentSlot Slot) const
{
    return EquippedItems.Find(Slot);
}

bool USEEquipmentManager::UpgradeEquipment(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade)
{
    if (!CanUpgradeEquipment(Slot, Upgrade))
    {
        return false;
    }

    // Get equipment
    FEquipmentInfo* Equipment = EquippedItems.Find(Slot);
    if (!Equipment)
    {
        return false;
    }

    // Apply upgrade stats
    ApplyUpgradeStats(Equipment->Stats, Upgrade);

    // Update total stats
    UpdateTotalStats();

    // Notify events
    OnEquipmentUpgraded.Broadcast(*Equipment);
    BP_OnEquipmentUpgraded(*Equipment);

    return true;
}

bool USEEquipmentManager::CanUpgradeEquipment(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade) const
{
    if (!ValidateUpgrade(Upgrade))
    {
        return false;
    }

    // Check if equipment exists
    const FEquipmentInfo* Equipment = GetEquippedItem(Slot);
    if (!Equipment)
    {
        return false;
    }

    // Check player currency if game instance exists
    if (GameInstance)
    {
        int32 UpgradeCost = GetUpgradeCost(Slot, Upgrade);
        if (GameInstance->GetPlayerCurrency() < UpgradeCost)
        {
            return false;
        }
    }

    return true;
}

int32 USEEquipmentManager::GetUpgradeCost(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade) const
{
    const FEquipmentInfo* Equipment = GetEquippedItem(Slot);
    if (!Equipment)
    {
        return 0;
    }

    // Calculate base cost
    float BaseCost = Upgrade.CostPerLevel;

    // Apply rarity multiplier
    BaseCost *= GetRarityMultiplier(Equipment->Rarity);

    // Apply level scaling
    BaseCost *= FMath::Pow(UpgradeCostMultiplier, Equipment->Level - 1);

    return FMath::RoundToInt(BaseCost);
}

void USEEquipmentManager::SetBaseStats(const FCombatStats& NewStats)
{
    BaseStats = NewStats;
    UpdateTotalStats();
}

bool USEEquipmentManager::ValidateEquipment(const FEquipmentInfo& Equipment) const
{
    // Check for valid ID
    if (Equipment.EquipmentID.IsNone())
    {
        return false;
    }

    // Check level requirement if game instance exists
    if (GameInstance && GameInstance->GetPlayerLevel() < Equipment.Level)
    {
        return false;
    }

    return true;
}

bool USEEquipmentManager::ValidateUpgrade(const FUpgradeInfo& Upgrade) const
{
    // Check for valid ID
    if (Upgrade.UpgradeID.IsNone())
    {
        return false;
    }

    // Check level limits
    if (Upgrade.Level >= Upgrade.MaxLevel)
    {
        return false;
    }

    return true;
}

void USEEquipmentManager::UpdateTotalStats()
{
    // Start with base stats
    TotalStats = BaseStats;

    // Add equipment stats
    FCombatStats EquipmentStats = CalculateEquipmentStats();
    
    // Apply equipment stats
    TotalStats.Health += EquipmentStats.Health;
    TotalStats.MaxHealth += EquipmentStats.MaxHealth;
    TotalStats.Attack += EquipmentStats.Attack;
    TotalStats.Defense += EquipmentStats.Defense;
    TotalStats.CriticalChance += EquipmentStats.CriticalChance;
    TotalStats.CriticalDamage += EquipmentStats.CriticalDamage;

    // Notify events
    OnStatsChanged.Broadcast(TotalStats);
    BP_OnStatsChanged(TotalStats);
}

FCombatStats USEEquipmentManager::CalculateEquipmentStats() const
{
    FCombatStats TotalEquipmentStats;

    // Sum up stats from all equipped items
    for (const auto& Pair : EquippedItems)
    {
        const FEquipmentInfo& Equipment = Pair.Value;
        
        // Apply rarity multiplier
        float Multiplier = GetRarityMultiplier(Equipment.Rarity);

        // Add scaled stats
        TotalEquipmentStats.Health += Equipment.Stats.Health * Multiplier;
        TotalEquipmentStats.MaxHealth += Equipment.Stats.MaxHealth * Multiplier;
        TotalEquipmentStats.Attack += Equipment.Stats.Attack * Multiplier;
        TotalEquipmentStats.Defense += Equipment.Stats.Defense * Multiplier;
        TotalEquipmentStats.CriticalChance += Equipment.Stats.CriticalChance * Multiplier;
        TotalEquipmentStats.CriticalDamage += Equipment.Stats.CriticalDamage * Multiplier;
    }

    return TotalEquipmentStats;
}

float USEEquipmentManager::GetRarityMultiplier(EEquipmentRarity Rarity) const
{
    float Multiplier = 1.0f;

    switch (Rarity)
    {
        case EEquipmentRarity::Common:
            Multiplier = 1.0f;
            break;
        case EEquipmentRarity::Uncommon:
            Multiplier = RarityStatMultiplier;
            break;
        case EEquipmentRarity::Rare:
            Multiplier = RarityStatMultiplier * RarityStatMultiplier;
            break;
        case EEquipmentRarity::Epic:
            Multiplier = RarityStatMultiplier * RarityStatMultiplier * RarityStatMultiplier;
            break;
        case EEquipmentRarity::Legendary:
            Multiplier = RarityStatMultiplier * RarityStatMultiplier * RarityStatMultiplier * RarityStatMultiplier;
            break;
    }

    return Multiplier;
}

void USEEquipmentManager::ApplyUpgradeStats(FCombatStats& Stats, const FUpgradeInfo& Upgrade) const
{
    // Apply stat increases
    Stats.Health += Upgrade.StatsPerLevel.Health;
    Stats.MaxHealth += Upgrade.StatsPerLevel.MaxHealth;
    Stats.Attack += Upgrade.StatsPerLevel.Attack;
    Stats.Defense += Upgrade.StatsPerLevel.Defense;
    Stats.CriticalChance += Upgrade.StatsPerLevel.CriticalChance;
    Stats.CriticalDamage += Upgrade.StatsPerLevel.CriticalDamage;
}
