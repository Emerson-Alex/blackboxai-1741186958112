// Copyright Shadow Echoes RPG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/** Log category for Shadow Echoes */
DECLARE_LOG_CATEGORY_EXTERN(LogShadowEchoes, Log, All);

/** Logging macros for convenience */
#define SE_LOG(Verbosity, Format, ...) \
    UE_LOG(LogShadowEchoes, Verbosity, Format, ##__VA_ARGS__)

#define SE_LOG_WARNING(Format, ...) \
    SE_LOG(Warning, Format, ##__VA_ARGS__)

#define SE_LOG_ERROR(Format, ...) \
    SE_LOG(Error, Format, ##__VA_ARGS__)

/** Version information for Shadow Echoes */
#define SHADOW_ECHOES_MAJOR_VERSION 1
#define SHADOW_ECHOES_MINOR_VERSION 0
#define SHADOW_ECHOES_PATCH_VERSION 0

/** Gameplay tags for Shadow Echoes */
#define GAMEPLAY_TAG_TIMELINE_BRIGHT TEXT("Timeline.Bright")
#define GAMEPLAY_TAG_TIMELINE_DARK TEXT("Timeline.Dark")
#define GAMEPLAY_TAG_ABILITY_LIGHT TEXT("Ability.Light")
#define GAMEPLAY_TAG_ABILITY_DARK TEXT("Ability.Dark")
#define GAMEPLAY_TAG_COMBAT_PARRY TEXT("Combat.Parry")
#define GAMEPLAY_TAG_COMBAT_DODGE TEXT("Combat.Dodge")
#define GAMEPLAY_TAG_COMBAT_COMBO TEXT("Combat.Combo")

/** Asset paths for Shadow Echoes */
#define ASSET_PATH_BRIGHT_WORLD TEXT("/Game/Levels/BrightWorld")
#define ASSET_PATH_DARK_WORLD TEXT("/Game/Levels/DarkWorld")
#define ASSET_PATH_QUEST_DATA TEXT("/Game/Data/DT_Quests")
#define ASSET_PATH_ABILITY_DATA TEXT("/Game/Data/DT_Abilities")

/** Configuration settings */
#define DEFAULT_TIMELINE_TRANSITION_DURATION 2.0f
#define DEFAULT_COMBAT_PARRY_WINDOW 0.2f
#define DEFAULT_COMBAT_COMBO_WINDOW 1.5f
#define DEFAULT_HITSTOP_DURATION 0.1f

/** Class to manage the Shadow Echoes game module */
class FShadowEchoesModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Get the module instance */
    static FShadowEchoesModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FShadowEchoesModule>("ShadowEchoes");
    }

    /** Get the module instance (static) */
    static FShadowEchoesModule* GetPtr()
    {
        return static_cast<FShadowEchoesModule*>(FModuleManager::Get().GetModule("ShadowEchoes"));
    }

private:
    /** Register custom settings */
    void RegisterSettings();

    /** Unregister custom settings */
    void UnregisterSettings();
};
