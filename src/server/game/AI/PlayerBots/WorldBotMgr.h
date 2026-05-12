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

#ifndef TRINITY_WORLDBOTMGR_H
#define TRINITY_WORLDBOTMGR_H

#include "Common.h"
#include "ObjectGuid.h"
#include "Position.h"
#include "WorldBotConfig.h"
#include <memory>
#include <utility>
#include <vector>

class WorldSession;
class Creature;
class Map;
class Player;
class Unit;

class TC_GAME_API WorldBotMgr
{
public:
    static WorldBotMgr* instance();
    ~WorldBotMgr();

    void LoadConfig(bool reload);
    void OnStartup();
    void Shutdown();
    void Update(uint32 diff);
    void UpdateMap(Map* map, uint32 diff);

    WorldBotConfig const& GetConfig() const { return _config; }
    bool IsEnabled() const { return _config.Enabled; }
    uint32 GetActiveBotCount() const { return _activeBotCount; }
    uint32 GetUpdateCount() const { return _updateCount; }

private:
    WorldBotMgr() = default;

    void EnsureDebugBot();
    void PrepareGeneratedBots();
    bool MaterializeGeneratedBotProfile(WorldBotGeneratedProfile const& profile, uint32 index);
    std::string BuildGeneratedBotName(uint32 index) const;
    void UpdateDebugBot(uint32 diff);
    bool UpdateDebugBotDeath(Map* map, uint32 diff);
    bool UpdateDebugBotLoot(Map* map, uint32 diff);
    bool UpdateDebugBotRecovery(Map* map, uint32 diff);
    bool UpdateDebugBotConsumables(Player* bot, bool needsHealth, bool needsMana, uint32 diff);
    bool UpdateDebugBotQuestPlanCompletion(Map* map);
    bool UpdateDebugBotQuesting(Map* map, uint32 diff);
    bool UpdateDebugBotQuestTravel(Map* map, uint32 diff);
    bool UpdateDebugBotCombat(Map* map, uint32 diff);
    bool UpdateDebugBotSpellRotation(Player* bot, Unit* victim, uint32 diff);
    void UpdateDebugBotRoam(Map* map, uint32 diff);
    Creature* SelectDebugBotQuestGiver(Player* bot) const;
    bool IsDebugBotQuestGiver(Player* bot, Creature* creature) const;
    bool UseDebugBotQuestGiver(Player* bot, Creature* creature);
    bool TurnInDebugBotQuest(Player* bot, Creature* creature, uint32 questId);
    bool AcceptDebugBotQuest(Player* bot, Creature* creature, uint32 questId);
    bool IsDebugBotPlannedQuest(uint32 questId) const;
    bool IsDebugBotQuestPlanComplete(Player* bot) const;
    bool HasDebugBotActivePlannedKillObjective(Player* bot) const;
    uint32 GetDebugBotPlannedQuestKillScore(Player* bot, Creature* creature) const;
    Creature* SelectDebugBotPlannedQuestKillTarget(Player* bot, float range) const;
    bool SelectDebugBotQuestTravelDestination(Player* bot, uint32& mapId, Position& destination, float& arrivalDistance, uint32& questId,
        uint32& targetEntry, char const*& action) const;
    bool FindNearestDebugBotCreatureSpawn(Player* bot, uint32 entry, uint32& mapId, Position& destination) const;
    bool FindNearestDebugBotQuestCreatureSpawn(Player* bot, uint32 questId, bool involvedRelation, uint32& mapId, Position& destination,
        uint32& targetEntry) const;
    Unit* SelectDebugBotCombatTarget(Player* bot) const;
    Creature* SelectDebugBotLootTarget(Player* bot) const;
    bool IsDebugBotLootCandidate(Player* bot, Creature* creature) const;
    bool LootDebugBotCreature(Player* bot, Creature* creature);
    void BlacklistDebugLootTarget(ObjectGuid const& guid);
    bool IsDebugLootTargetBlacklisted(ObjectGuid const& guid) const;
    void UpdateDebugLootBlacklist(uint32 diff);

    WorldBotConfig _config;
    uint32 _updateTimer = 0;
    uint32 _updateCount = 0;
    uint32 _activeBotCount = 0;
    bool _debugLoginAttempted = false;
    bool _generatedPrepareAttempted = false;
    uint32 _debugCombatScanTimer = 0;
    uint32 _debugCombatNoVictimLogTimer = 0;
    uint32 _debugSpellCastTimer = 0;
    uint32 _debugSpellDisabledLogTimer = 0;
    uint32 _debugLootScanTimer = 0;
    uint32 _debugLootMoveTimer = 0;
    uint32 _debugLootTargetTimer = 0;
    uint32 _debugConsumableScanTimer = 0;
    uint32 _debugDeathReleaseTimer = 0;
    uint32 _debugDeathRespawnTimer = 0;
    uint32 _debugQuestScanTimer = 0;
    uint32 _debugQuestMoveTimer = 0;
    uint32 _debugQuestTargetTimer = 0;
    uint32 _debugQuestTravelTimer = 0;
    bool _debugRecovering = false;
    bool _debugDeathReleased = false;
    uint32 _debugRoamTimer = 0;
    uint32 _debugMovePointId = 1;
    ObjectGuid _debugLootTargetGuid;
    ObjectGuid _debugQuestTargetGuid;
    std::vector<std::pair<ObjectGuid, uint32>> _debugLootBlacklist;
    std::unique_ptr<WorldSession> _debugSession;
};

#define sWorldBotMgr WorldBotMgr::instance()

#endif
