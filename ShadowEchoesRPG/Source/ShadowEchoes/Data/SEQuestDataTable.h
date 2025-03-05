// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/SETypes.h"
#include "SEQuestDataTable.generated.h"

/**
 * FQuestObjectiveRow - Data table row for quest objectives
 */
USTRUCT(BlueprintType)
struct FSEQuestObjectiveRow : public FTableRowBase
{
    GENERATED_BODY()

    /** Objective ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName ObjectiveID;

    /** Objective description */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    /** Whether this objective is optional */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsOptional;

    /** Required timeline state for this objective */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    ETimelineState RequiredTimeline;

    /** Required level for this objective */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RequiredLevel;

    /** Reward experience points */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardXP;

    /** Reward currency */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardCurrency;

    FSEQuestObjectiveRow()
        : bIsOptional(false)
        , RequiredTimeline(ETimelineState::BrightWorld)
        , RequiredLevel(1)
        , RewardXP(0)
        , RewardCurrency(0)
    {}
};

/**
 * FQuestRow - Data table row for quests
 */
USTRUCT(BlueprintType)
struct FSEQuestRow : public FTableRowBase
{
    GENERATED_BODY()

    /** Quest ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName QuestID;

    /** Quest title */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Title;

    /** Quest description */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FText Description;

    /** Quest type */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    EQuestType Type;

    /** Required timeline state */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    ETimelineState RequiredTimeline;

    /** Required level */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RequiredLevel;

    /** Prerequisite quest IDs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> PrerequisiteQuests;

    /** Objective IDs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> ObjectiveIDs;

    /** Reward experience points */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardXP;

    /** Reward currency */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 RewardCurrency;

    /** Reward items */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TArray<FName> RewardItems;

    /** Whether this quest is repeatable */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsRepeatable;

    /** Cooldown time for repeatable quests (in seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "bIsRepeatable"))
    float RepeatCooldown;

    FSEQuestRow()
        : Type(EQuestType::SideQuest)
        , RequiredTimeline(ETimelineState::BrightWorld)
        , RequiredLevel(1)
        , RewardXP(0)
        , RewardCurrency(0)
        , bIsRepeatable(false)
        , RepeatCooldown(86400.0f) // 24 hours
    {}
};

/**
 * Helper functions for quest data tables
 */
namespace SEQuestDataTableHelpers
{
    /** Convert data table row to quest data */
    inline FQuestData ConvertRowToQuestData(const FSEQuestRow& Row, const TArray<FSEQuestObjectiveRow>& ObjectiveRows)
    {
        FQuestData QuestData;
        QuestData.QuestID = Row.QuestID;
        QuestData.Title = Row.Title;
        QuestData.Description = Row.Description;
        QuestData.Type = Row.Type;
        QuestData.RequiredTimeline = Row.RequiredTimeline;
        QuestData.State = EQuestState::NotStarted;

        // Add objectives
        for (const FName& ObjectiveID : Row.ObjectiveIDs)
        {
            const FSEQuestObjectiveRow* ObjectiveRow = ObjectiveRows.FindByPredicate(
                [ObjectiveID](const FSEQuestObjectiveRow& Row) { return Row.ObjectiveID == ObjectiveID; }
            );

            if (ObjectiveRow)
            {
                FQuestObjective Objective;
                Objective.ObjectiveID = ObjectiveRow->ObjectiveID;
                Objective.Description = ObjectiveRow->Description;
                Objective.bIsOptional = ObjectiveRow->bIsOptional;
                Objective.RequiredTimeline = ObjectiveRow->RequiredTimeline;
                Objective.bIsComplete = false;
                Objective.Progress = 0.0f;
                QuestData.Objectives.Add(Objective);
            }
        }

        return QuestData;
    }
}
