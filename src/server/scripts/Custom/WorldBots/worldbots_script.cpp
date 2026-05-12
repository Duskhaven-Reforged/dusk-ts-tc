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

#include "ScriptMgr.h"
#include "WorldBotMgr.h"

class WorldBotsWorldScript : public WorldScript
{
public:
    WorldBotsWorldScript() : WorldScript("WorldBotsWorldScript") { }

    void OnConfigLoad(bool reload) override
    {
        sWorldBotMgr->LoadConfig(reload);
    }

    void OnStartup() override
    {
        sWorldBotMgr->OnStartup();
    }

    void OnShutdown() override
    {
        sWorldBotMgr->Shutdown();
    }

    void OnUpdate(uint32 diff) override
    {
        sWorldBotMgr->Update(diff);
    }
};

class WorldBotsContinentScript : public WorldMapScript
{
public:
    WorldBotsContinentScript(char const* name, uint32 mapId) : WorldMapScript(name, mapId) { }

    void OnUpdate(Map* map, uint32 diff) override
    {
        sWorldBotMgr->UpdateMap(map, diff);
    }
};

void AddSC_worldbots()
{
    new WorldBotsWorldScript();
    new WorldBotsContinentScript("WorldBotsEasternKingdoms", 0);
    new WorldBotsContinentScript("WorldBotsKalimdor", 1);
    new WorldBotsContinentScript("WorldBotsOutland", 530);
    new WorldBotsContinentScript("WorldBotsNorthrend", 571);
}
