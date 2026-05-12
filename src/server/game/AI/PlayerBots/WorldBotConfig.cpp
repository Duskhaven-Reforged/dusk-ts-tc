/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldBotConfig.h"
#include "Config.h"
#include "Log.h"
#include "ObjectDefines.h"
#include "StringConvert.h"
#include "Util.h"
#include <string>
#include <string_view>
#include <utility>

void WorldBotConfig::Load(bool reload)
{
    Enabled = sConfigMgr->GetBoolDefault("WorldBots.Enable", false);
    Debug = sConfigMgr->GetBoolDefault("WorldBots.Debug", false);
    MaxActiveBots = sConfigMgr->GetIntDefault("WorldBots.MaxActiveBots", 0);
    UpdateIntervalMs = sConfigMgr->GetIntDefault("WorldBots.UpdateIntervalMs", 1000);
    MapTickBudgetMs = sConfigMgr->GetIntDefault("WorldBots.MapTickBudgetMs", 2);
    DebugCharacterGuid = sConfigMgr->GetIntDefault("WorldBots.Debug.CharacterGuid", 0);
    DebugAccountId = sConfigMgr->GetIntDefault("WorldBots.Debug.AccountId", 0);
    DebugRoam = sConfigMgr->GetBoolDefault("WorldBots.Debug.Roam", false);
    DebugRoamIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.RoamIntervalMs", 5000);
    DebugRoamRadius = sConfigMgr->GetFloatDefault("WorldBots.Debug.RoamRadius", 35.0f);
    DebugRoamMinDistance = sConfigMgr->GetFloatDefault("WorldBots.Debug.RoamMinDistance", 8.0f);
    DebugCombat = sConfigMgr->GetBoolDefault("WorldBots.Debug.Combat", false);
    DebugCombatScanIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.CombatScanIntervalMs", 1500);
    DebugCombatSearchRange = sConfigMgr->GetFloatDefault("WorldBots.Debug.CombatSearchRange", 25.0f);
    DebugCombatLeashRange = sConfigMgr->GetFloatDefault("WorldBots.Debug.CombatLeashRange", 60.0f);
    DebugCombatMinLevelDelta = sConfigMgr->GetIntDefault("WorldBots.Debug.CombatMinLevelDelta", -5);
    DebugCombatMaxLevelDelta = sConfigMgr->GetIntDefault("WorldBots.Debug.CombatMaxLevelDelta", 2);
    DebugSpellRotation = sConfigMgr->GetBoolDefault("WorldBots.Debug.SpellRotation", DebugCombat);
    DebugSpellCastIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.SpellCastIntervalMs", 1500);
    DebugLoot = sConfigMgr->GetBoolDefault("WorldBots.Debug.Loot", false);
    DebugLootScanIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.LootScanIntervalMs", 1000);
    DebugLootSearchRange = sConfigMgr->GetFloatDefault("WorldBots.Debug.LootSearchRange", 30.0f);
    DebugLootBlacklistMs = sConfigMgr->GetIntDefault("WorldBots.Debug.LootBlacklistMs", 30000);
    DebugRecovery = sConfigMgr->GetBoolDefault("WorldBots.Debug.Recovery", false);
    DebugRecoveryStartHealthPct = sConfigMgr->GetFloatDefault("WorldBots.Debug.RecoveryStartHealthPct", 45.0f);
    DebugRecoveryStopHealthPct = sConfigMgr->GetFloatDefault("WorldBots.Debug.RecoveryStopHealthPct", 85.0f);
    DebugRecoveryStartManaPct = sConfigMgr->GetFloatDefault("WorldBots.Debug.RecoveryStartManaPct", 20.0f);
    DebugRecoveryStopManaPct = sConfigMgr->GetFloatDefault("WorldBots.Debug.RecoveryStopManaPct", 75.0f);
    DebugConsumables = sConfigMgr->GetBoolDefault("WorldBots.Debug.Consumables", false);
    DebugConsumableScanIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.ConsumableScanIntervalMs", 2000);
    DebugDeathHandling = sConfigMgr->GetBoolDefault("WorldBots.Debug.DeathHandling", false);
    DebugDeathReleaseDelayMs = sConfigMgr->GetIntDefault("WorldBots.Debug.DeathReleaseDelayMs", 5000);
    DebugDeathRespawnDelayMs = sConfigMgr->GetIntDefault("WorldBots.Debug.DeathRespawnDelayMs", 10000);
    DebugQuesting = sConfigMgr->GetBoolDefault("WorldBots.Debug.Questing", false);
    DebugQuestScanIntervalMs = sConfigMgr->GetIntDefault("WorldBots.Debug.QuestScanIntervalMs", 3000);
    DebugQuestSearchRange = sConfigMgr->GetFloatDefault("WorldBots.Debug.QuestSearchRange", 35.0f);
    DebugQuestInteractTimeoutMs = sConfigMgr->GetIntDefault("WorldBots.Debug.QuestInteractTimeoutMs", 15000);
    DebugQuestPlanIds.clear();
    std::string questPlan = sConfigMgr->GetStringDefault("WorldBots.Debug.QuestPlan", "");
    for (std::string_view token : Trinity::Tokenize(questPlan, ',', false))
    {
        if (Optional<uint32> questId = Trinity::StringTo<uint32>(token))
            DebugQuestPlanIds.push_back(*questId);
        else
            TC_LOG_WARN("server.worldbots", "Ignoring invalid quest id '{}' in WorldBots.Debug.QuestPlan.", token);
    }

    if (UpdateIntervalMs < 100)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.UpdateIntervalMs ({}) is too low. Using 100.", UpdateIntervalMs);
        UpdateIntervalMs = 100;
    }

    if (!Enabled && MaxActiveBots != 0)
        TC_LOG_WARN("server.worldbots", "WorldBots.MaxActiveBots is {}, but WorldBots.Enable is disabled.", MaxActiveBots);

    if (DebugCharacterGuid && !DebugAccountId)
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CharacterGuid is set but WorldBots.Debug.AccountId is 0. Debug bot login will be skipped.");

    if (DebugRoamIntervalMs < 1000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.RoamIntervalMs ({}) is too low. Using 1000.", DebugRoamIntervalMs);
        DebugRoamIntervalMs = 1000;
    }

    if (DebugRoamRadius < 5.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.RoamRadius ({}) is too low. Using 5.", DebugRoamRadius);
        DebugRoamRadius = 5.0f;
    }
    else if (DebugRoamRadius > 200.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.RoamRadius ({}) is too high. Using 200.", DebugRoamRadius);
        DebugRoamRadius = 200.0f;
    }

    if (DebugRoamMinDistance < 1.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.RoamMinDistance ({}) is too low. Using 1.", DebugRoamMinDistance);
        DebugRoamMinDistance = 1.0f;
    }

    if (DebugRoamMinDistance > DebugRoamRadius)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.RoamMinDistance ({}) is above roam radius ({}). Using radius.", DebugRoamMinDistance, DebugRoamRadius);
        DebugRoamMinDistance = DebugRoamRadius;
    }

    if (DebugCombatScanIntervalMs < 500)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CombatScanIntervalMs ({}) is too low. Using 500.", DebugCombatScanIntervalMs);
        DebugCombatScanIntervalMs = 500;
    }

    if (DebugCombatSearchRange < 5.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CombatSearchRange ({}) is too low. Using 5.", DebugCombatSearchRange);
        DebugCombatSearchRange = 5.0f;
    }
    else if (DebugCombatSearchRange > 80.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CombatSearchRange ({}) is too high. Using 80.", DebugCombatSearchRange);
        DebugCombatSearchRange = 80.0f;
    }

    if (DebugCombatLeashRange < DebugCombatSearchRange)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CombatLeashRange ({}) is below search range ({}). Using search range.", DebugCombatLeashRange, DebugCombatSearchRange);
        DebugCombatLeashRange = DebugCombatSearchRange;
    }

    if (DebugCombatMinLevelDelta > DebugCombatMaxLevelDelta)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.CombatMinLevelDelta ({}) is above max delta ({}). Swapping.", DebugCombatMinLevelDelta, DebugCombatMaxLevelDelta);
        std::swap(DebugCombatMinLevelDelta, DebugCombatMaxLevelDelta);
    }

    if (DebugSpellCastIntervalMs < 500)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.SpellCastIntervalMs ({}) is too low. Using 500.", DebugSpellCastIntervalMs);
        DebugSpellCastIntervalMs = 500;
    }

    if (Debug && DebugCombat && !DebugSpellRotation)
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.Combat is enabled but WorldBots.Debug.SpellRotation is disabled. Bot will only melee.");

    if (DebugLootScanIntervalMs < 500)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.LootScanIntervalMs ({}) is too low. Using 500.", DebugLootScanIntervalMs);
        DebugLootScanIntervalMs = 500;
    }

    if (DebugLootSearchRange < INTERACTION_DISTANCE)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.LootSearchRange ({}) is too low. Using {}.", DebugLootSearchRange, INTERACTION_DISTANCE);
        DebugLootSearchRange = INTERACTION_DISTANCE;
    }
    else if (DebugLootSearchRange > 80.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.LootSearchRange ({}) is too high. Using 80.", DebugLootSearchRange);
        DebugLootSearchRange = 80.0f;
    }

    if (DebugLootBlacklistMs < 1000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.LootBlacklistMs ({}) is too low. Using 1000.", DebugLootBlacklistMs);
        DebugLootBlacklistMs = 1000;
    }

    if (DebugRecoveryStartHealthPct < 1.0f)
        DebugRecoveryStartHealthPct = 1.0f;
    else if (DebugRecoveryStartHealthPct > 99.0f)
        DebugRecoveryStartHealthPct = 99.0f;

    if (DebugRecoveryStopHealthPct < DebugRecoveryStartHealthPct)
        DebugRecoveryStopHealthPct = DebugRecoveryStartHealthPct;
    else if (DebugRecoveryStopHealthPct > 100.0f)
        DebugRecoveryStopHealthPct = 100.0f;

    if (DebugRecoveryStartManaPct < 0.0f)
        DebugRecoveryStartManaPct = 0.0f;
    else if (DebugRecoveryStartManaPct > 99.0f)
        DebugRecoveryStartManaPct = 99.0f;

    if (DebugRecoveryStopManaPct < DebugRecoveryStartManaPct)
        DebugRecoveryStopManaPct = DebugRecoveryStartManaPct;
    else if (DebugRecoveryStopManaPct > 100.0f)
        DebugRecoveryStopManaPct = 100.0f;

    if (DebugConsumables && !DebugRecovery)
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.Consumables is enabled but WorldBots.Debug.Recovery is disabled. Consumables will not be used.");

    if (DebugConsumableScanIntervalMs < 500)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.ConsumableScanIntervalMs ({}) is too low. Using 500.", DebugConsumableScanIntervalMs);
        DebugConsumableScanIntervalMs = 500;
    }

    if (DebugDeathReleaseDelayMs < 1000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.DeathReleaseDelayMs ({}) is too low. Using 1000.", DebugDeathReleaseDelayMs);
        DebugDeathReleaseDelayMs = 1000;
    }

    if (DebugDeathRespawnDelayMs < 1000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.DeathRespawnDelayMs ({}) is too low. Using 1000.", DebugDeathRespawnDelayMs);
        DebugDeathRespawnDelayMs = 1000;
    }

    if (DebugQuestScanIntervalMs < 1000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.QuestScanIntervalMs ({}) is too low. Using 1000.", DebugQuestScanIntervalMs);
        DebugQuestScanIntervalMs = 1000;
    }

    if (DebugQuestSearchRange < INTERACTION_DISTANCE)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.QuestSearchRange ({}) is too low. Using {}.", DebugQuestSearchRange, INTERACTION_DISTANCE);
        DebugQuestSearchRange = INTERACTION_DISTANCE;
    }
    else if (DebugQuestSearchRange > 100.0f)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.QuestSearchRange ({}) is too high. Using 100.", DebugQuestSearchRange);
        DebugQuestSearchRange = 100.0f;
    }

    if (DebugQuestInteractTimeoutMs < 3000)
    {
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.QuestInteractTimeoutMs ({}) is too low. Using 3000.", DebugQuestInteractTimeoutMs);
        DebugQuestInteractTimeoutMs = 3000;
    }

    if (DebugQuesting && DebugQuestPlanIds.empty())
        TC_LOG_WARN("server.worldbots", "WorldBots.Debug.Questing is enabled but WorldBots.Debug.QuestPlan is empty. Bot will not opportunistically accept unrelated quests.");

    TC_LOG_INFO("server.worldbots", "WorldBots config {}: enabled={}, maxActiveBots={}, updateIntervalMs={}, mapTickBudgetMs={}, debug={}, debugCharacterGuid={}, debugAccountId={}, debugRoam={}, debugRoamIntervalMs={}, debugRoamRadius={}, debugRoamMinDistance={}, debugCombat={}, debugCombatScanIntervalMs={}, debugCombatSearchRange={}, debugCombatLeashRange={}, debugCombatMinLevelDelta={}, debugCombatMaxLevelDelta={}, debugSpellRotation={}, debugSpellCastIntervalMs={}, debugLoot={}, debugLootScanIntervalMs={}, debugLootSearchRange={}, debugLootBlacklistMs={}, debugRecovery={}, debugRecoveryStartHealthPct={}, debugRecoveryStopHealthPct={}, debugRecoveryStartManaPct={}, debugRecoveryStopManaPct={}, debugConsumables={}, debugConsumableScanIntervalMs={}, debugDeathHandling={}, debugDeathReleaseDelayMs={}, debugDeathRespawnDelayMs={}, debugQuesting={}, debugQuestScanIntervalMs={}, debugQuestSearchRange={}, debugQuestInteractTimeoutMs={}, debugQuestPlanCount={}",
        reload ? "reloaded" : "loaded", Enabled, MaxActiveBots, UpdateIntervalMs, MapTickBudgetMs, Debug, DebugCharacterGuid, DebugAccountId,
        DebugRoam, DebugRoamIntervalMs, DebugRoamRadius, DebugRoamMinDistance, DebugCombat, DebugCombatScanIntervalMs, DebugCombatSearchRange,
        DebugCombatLeashRange, DebugCombatMinLevelDelta, DebugCombatMaxLevelDelta, DebugSpellRotation, DebugSpellCastIntervalMs, DebugLoot,
        DebugLootScanIntervalMs, DebugLootSearchRange, DebugLootBlacklistMs, DebugRecovery, DebugRecoveryStartHealthPct, DebugRecoveryStopHealthPct,
        DebugRecoveryStartManaPct, DebugRecoveryStopManaPct, DebugConsumables, DebugConsumableScanIntervalMs, DebugDeathHandling, DebugDeathReleaseDelayMs,
        DebugDeathRespawnDelayMs, DebugQuesting, DebugQuestScanIntervalMs, DebugQuestSearchRange, DebugQuestInteractTimeoutMs, DebugQuestPlanIds.size());
}
