// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Combat/SECombatTypes.h"
#include "SEEquipmentManager.generated.h"

class USEGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlot, Slot, const FEquipmentInfo&, Equipment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUpgraded, const FEquipmentInfo&, Equipment);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsChanged, const FCombatStats&, NewStats);

/**
 * Manages equipment and upgrade systems
 */
UCLASS()
class SHADOWECHOES_API USEEquipmentManager : public UObject
{
    GENERATED_BODY()

public:
    USEEquipmentManager();

    /** Initialize the equipment system */
    void Initialize(USEGameInstance* InGameInstance);

    /** Equipment management */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Equipment")
    bool EquipItem(const FEquipmentInfo& Equipment);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Equipment")
    bool UnequipItem(EEquipmentSlot Slot);

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Equipment")
    const FEquipmentInfo* GetEquippedItem(EEquipmentSlot Slot) const;

    /** Equipment upgrades */
    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Equipment")
    bool UpgradeEquipment(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade);

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Equipment")
    bool CanUpgradeEquipment(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade) const;

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Equipment")
    int32 GetUpgradeCost(EEquipmentSlot Slot, const FUpgradeInfo& Upgrade) const;

    /** Stats management */
    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Equipment")
    FCombatStats GetTotalStats() const { return TotalStats; }

    UFUNCTION(BlueprintPure, Category = "Shadow Echoes|Equipment")
    FCombatStats GetBaseStats() const { return BaseStats; }

    UFUNCTION(BlueprintCallable, Category = "Shadow Echoes|Equipment")
    void SetBaseStats(const FCombatStats& NewStats);

    /** Events */
    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Equipment|Events")
    FOnEquipmentChanged OnEquipmentChanged;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Equipment|Events")
    FOnEquipmentUpgraded OnEquipmentUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Shadow Echoes|Equipment|Events")
    FOnStatsChanged OnStatsChanged;

protected:
    /** Equipment settings */
    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Equipment")
    float UpgradeCostMultiplier;

    UPROPERTY(EditDefaultsOnly, Category = "Shadow Echoes|Equipment")
    float RarityStatMultiplier;

private:
    /** Current equipment */
    UPROPERTY()
    TMap<EEquipmentSlot, FEquipmentInfo> EquippedItems;

    /** Current stats */
    UPROPERTY()
    FCombatStats BaseStats;

    UPROPERTY()
    FCombatStats TotalStats;

    /** Game instance reference */
    UPROPERTY()
    USEGameInstance* GameInstance;

    /** Equipment helpers */
    bool ValidateEquipment(const FEquipmentInfo& Equipment) const;
    bool ValidateUpgrade(const FUpgradeInfo& Upgrade) const;
    void UpdateTotalStats();

    /** Stat calculation */
    FCombatStats CalculateEquipmentStats() const;
    float GetRarityMultiplier(EEquipmentRarity Rarity) const;
    void ApplyUpgradeStats(FCombatStats& Stats, const FUpgradeInfo& Upgrade) const;

protected:
    /** Blueprint events */
    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Equipment|Events")
    void BP_OnEquipmentChanged(EEquipmentSlot Slot, const FEquipmentInfo& Equipment);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Equipment|Events")
    void BP_OnEquipmentUpgraded(const FEquipmentInfo& Equipment);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shadow Echoes|Equipment|Events")
    void BP_OnStatsChanged(const FCombatStats& NewStats);
};
