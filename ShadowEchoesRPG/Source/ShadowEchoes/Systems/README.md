# Shadow Echoes Quest System

The Shadow Echoes Quest System provides a flexible, data-driven approach to managing quests and objectives across different timelines. This document outlines the system's architecture and usage.

## Architecture Overview

### Core Components

1. **QuestManager (`QuestManager.h/cpp`)**
   - Manages quest lifecycle and state
   - Handles quest loading from data tables
   - Tracks quest progress and completion
   - Provides quest filtering and rewards

2. **Quest Data Tables**
   - `DT_Quests.json`: Defines quest properties and relationships
   - `DT_QuestObjectives.json`: Defines individual quest objectives

3. **UI Components**
   - `QuestLogWidget`: Main quest log interface
   - `QuestEntryWidget`: Individual quest entry display
   - `QuestObjectiveWidget`: Individual objective display
   - `NotificationWidget`: Quest-related notifications

### Data Structure

#### Quest Data (`FSEQuestRow`)
```cpp
- QuestID: Unique identifier
- Title: Display title
- Description: Detailed description
- Type: MainQuest/SideQuest/TimelineQuest
- RequiredTimeline: BrightWorld/DarkWorld
- RequiredLevel: Minimum player level
- PrerequisiteQuests: Required completed quests
- ObjectiveIDs: List of objectives
- Rewards: XP, Currency, Items
```

#### Objective Data (`FSEQuestObjectiveRow`)
```cpp
- ObjectiveID: Unique identifier
- Description: Task description
- bIsOptional: Optional objective flag
- RequiredTimeline: BrightWorld/DarkWorld
- RequiredLevel: Minimum level
- Rewards: XP, Currency
```

## Usage Guide

### Adding New Quests

1. Add quest entry to `DT_Quests.json`:
```json
{
    "Name": "QuestID",
    "QuestID": "UniqueID",
    "Title": "Quest Title",
    "Description": "Quest Description",
    "Type": "QuestType",
    "RequiredTimeline": "Timeline",
    "RequiredLevel": 1,
    "PrerequisiteQuests": [],
    "ObjectiveIDs": ["Objective1", "Objective2"],
    "RewardXP": 100,
    "RewardCurrency": 50,
    "RewardItems": ["ItemID"],
    "bIsRepeatable": false
}
```

2. Add objectives to `DT_QuestObjectives.json`:
```json
{
    "Name": "ObjectiveID",
    "ObjectiveID": "UniqueID",
    "Description": "Objective Description",
    "bIsOptional": false,
    "RequiredTimeline": "Timeline",
    "RequiredLevel": 1,
    "RewardXP": 25,
    "RewardCurrency": 10
}
```

### Quest Management

```cpp
// Load quests from data tables
QuestManager->LoadQuestsFromDataTable(QuestTable, ObjectiveTable);

// Add quest
QuestManager->AddQuest(QuestData);

// Update progress
QuestManager->UpdateQuestProgress(QuestID, ObjectiveID, Progress);

// Complete quest
QuestManager->CompleteQuest(QuestID);

// Get available quests
TArray<FQuestData> Quests = QuestManager->GetAvailableQuestsForLevel(PlayerLevel);
```

### Timeline Integration

The quest system integrates with the timeline system through:
- Timeline-specific quests and objectives
- Quest availability based on current timeline
- Visual indicators for timeline requirements
- Automatic quest state updates on timeline changes

### UI Integration

The quest system provides UI components that automatically:
- Display quest log with filtering options
- Show quest notifications
- Track objective progress
- Indicate timeline requirements
- Display rewards and prerequisites

## Best Practices

1. **Quest Design**
   - Use clear, unique IDs for quests and objectives
   - Keep objectives focused and measurable
   - Consider timeline implications
   - Balance rewards appropriately

2. **Timeline Integration**
   - Consider both timelines when designing quest chains
   - Use timeline-specific objectives for narrative branching
   - Provide clear timeline requirements in UI

3. **Performance**
   - Cache quest data where appropriate
   - Use efficient quest filtering
   - Minimize UI updates

## Extending the System

To extend the quest system:

1. **Add New Quest Types**
   - Extend `EQuestType` enum
   - Update UI to handle new types
   - Add type-specific logic to QuestManager

2. **Add New Objective Types**
   - Create new objective structs
   - Implement tracking logic
   - Update UI components

3. **Add New Rewards**
   - Extend reward structs
   - Implement reward distribution
   - Update UI to display new rewards

## Troubleshooting

Common issues and solutions:

1. **Quest Not Appearing**
   - Check level requirements
   - Verify timeline state
   - Check prerequisite completion
   - Validate data table entries

2. **Progress Not Updating**
   - Verify objective IDs match
   - Check timeline requirements
   - Validate progress calculation

3. **UI Not Updating**
   - Check widget bindings
   - Verify event broadcasts
   - Check UI update calls
