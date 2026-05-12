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

#include "WorldBotMgr.h"
#include "DBCStructure.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Random.h"
#include "World.h"
#include "WorldBotSession.h"
#include "WorldSession.h"
#include "Log.h"
#include <string>
#include <utility>

namespace
{
    constexpr float TwoPi = 6.28318530718f;
}

WorldBotMgr* WorldBotMgr::instance()
{
    static WorldBotMgr instance;
    return &instance;
}

WorldBotMgr::~WorldBotMgr() = default;

void WorldBotMgr::LoadConfig(bool reload)
{
    _config.Load(reload);
    _updateTimer = 0;
    _debugLoginAttempted = false;
    _debugRoamTimer = 0;

    if (!_config.Enabled && _debugSession)
    {
        _debugSession->LogoutPlayer(true);
        _debugSession.reset();
        _activeBotCount = 0;
    }
}

void WorldBotMgr::OnStartup()
{
    if (!_config.Enabled)
        return;

    TC_LOG_INFO("server.worldbots", "WorldBots startup complete. Population system is not implemented yet.");

    if (_config.DebugCharacterGuid)
        TC_LOG_INFO("server.worldbots", "WorldBots debug character {} will be loaded on first world update.", _config.DebugCharacterGuid);
}

void WorldBotMgr::Shutdown()
{
    if (!_debugSession)
        return;

    _debugSession->LogoutPlayer(true);
    _debugSession.reset();
    _activeBotCount = 0;
}

void WorldBotMgr::Update(uint32 diff)
{
    if (!_config.Enabled)
        return;

    EnsureDebugBot();
    UpdateDebugBot(diff);

    if (_updateTimer > diff)
    {
        _updateTimer -= diff;
        return;
    }

    _updateTimer = _config.UpdateIntervalMs;
    ++_updateCount;

    if (_config.Debug)
        TC_LOG_DEBUG("server.worldbots", "WorldBots tick {} activeBots={}", _updateCount, _activeBotCount);
}

void WorldBotMgr::UpdateMap(Map* map, uint32 diff)
{
    if (!_config.Enabled || !_config.DebugRoam)
        return;

    UpdateDebugBotRoam(map, diff);
}

void WorldBotMgr::EnsureDebugBot()
{
    if (_debugSession || _debugLoginAttempted || !_config.DebugCharacterGuid)
        return;

    _debugLoginAttempted = true;

    if (!_config.DebugAccountId)
    {
        TC_LOG_WARN("server.worldbots", "Skipping debug WorldBot login for character {}: WorldBots.Debug.AccountId is 0.", _config.DebugCharacterGuid);
        return;
    }

    if (!_config.MaxActiveBots)
    {
        TC_LOG_WARN("server.worldbots", "Skipping debug WorldBot login for character {}: WorldBots.MaxActiveBots is 0.", _config.DebugCharacterGuid);
        return;
    }

    std::string accountName = "WorldBot" + std::to_string(_config.DebugAccountId);
    std::unique_ptr<WorldSession> session = WorldBotSession::Create(_config.DebugAccountId, std::move(accountName),
        uint8(sWorld->getIntConfig(CONFIG_EXPANSION)));
    ObjectGuid playerGuid(HighGuid::Player, _config.DebugCharacterGuid);

    if (!session->BeginPlayerLogin(playerGuid, false))
    {
        TC_LOG_ERROR("server.worldbots", "Failed to begin debug WorldBot login for character {} account {}.", _config.DebugCharacterGuid, _config.DebugAccountId);
        return;
    }

    _debugSession = std::move(session);
    TC_LOG_INFO("server.worldbots", "Queued debug WorldBot login for character {} account {}.", _config.DebugCharacterGuid, _config.DebugAccountId);
}

void WorldBotMgr::UpdateDebugBot(uint32 diff)
{
    if (!_debugSession)
    {
        _activeBotCount = 0;
        return;
    }

    WorldSessionFilter updater(_debugSession.get());
    if (!_debugSession->Update(diff, updater))
    {
        TC_LOG_INFO("server.worldbots", "Debug WorldBot session ended.");
        _debugSession.reset();
        _activeBotCount = 0;
        return;
    }

    if (!_debugSession->PlayerLoading() && !_debugSession->GetPlayer())
    {
        TC_LOG_ERROR("server.worldbots", "Debug WorldBot login did not produce a player. Ending debug session.");
        _debugSession.reset();
        _activeBotCount = 0;
        return;
    }

    _activeBotCount = _debugSession->GetPlayer() ? 1 : 0;
}

void WorldBotMgr::UpdateDebugBotRoam(Map* map, uint32 diff)
{
    if (!_debugSession)
        return;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return;

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return;

    if (_debugRoamTimer > diff)
    {
        _debugRoamTimer -= diff;
        return;
    }

    _debugRoamTimer = _config.DebugRoamIntervalMs;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsInCombat() || bot->IsBeingTeleported() || !bot->IsStopped())
        return;

    for (uint8 attempt = 0; attempt < 8; ++attempt)
    {
        float distance = frand(_config.DebugRoamMinDistance, _config.DebugRoamRadius);
        float angle = frand(0.0f, TwoPi);
        Position destination = bot->GetPosition();
        bot->MovePositionToFirstCollision(destination, distance, angle);

        if (!destination.IsPositionValid() || bot->GetExactDist2d(destination) < _config.DebugRoamMinDistance)
            continue;

        if (!bot->IsWithinLOS(destination.GetPositionX(), destination.GetPositionY(), destination.GetPositionZ()))
            continue;

        bot->GetMotionMaster()->MovePoint(_debugMovePointId++, destination, true);

        if (_config.Debug)
            TC_LOG_DEBUG("server.worldbots", "WorldBot {} roaming to map={} x={} y={} z={}", bot->GetName(), map->GetId(),
                destination.GetPositionX(), destination.GetPositionY(), destination.GetPositionZ());

        return;
    }

    if (_config.Debug)
        TC_LOG_DEBUG("server.worldbots", "WorldBot {} could not find a roam destination near map={} x={} y={} z={}", bot->GetName(),
            map->GetId(), bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
}
