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
#include "WorldBotConfig.h"
#include <memory>

class WorldSession;
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
    void UpdateDebugBot(uint32 diff);
    bool UpdateDebugBotCombat(Map* map, uint32 diff);
    void UpdateDebugBotRoam(Map* map, uint32 diff);
    Unit* SelectDebugBotCombatTarget(Player* bot) const;

    WorldBotConfig _config;
    uint32 _updateTimer = 0;
    uint32 _updateCount = 0;
    uint32 _activeBotCount = 0;
    bool _debugLoginAttempted = false;
    uint32 _debugCombatScanTimer = 0;
    uint32 _debugRoamTimer = 0;
    uint32 _debugMovePointId = 1;
    std::unique_ptr<WorldSession> _debugSession;
};

#define sWorldBotMgr WorldBotMgr::instance()

#endif
