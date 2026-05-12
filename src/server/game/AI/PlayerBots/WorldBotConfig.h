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

#ifndef TRINITY_WORLDBOTCONFIG_H
#define TRINITY_WORLDBOTCONFIG_H

#include "Common.h"
#include <vector>

struct TC_GAME_API WorldBotConfig
{
    void Load(bool reload);

    bool Enabled = false;
    bool Debug = false;
    uint32 MaxActiveBots = 0;
    uint32 UpdateIntervalMs = 1000;
    uint32 MapTickBudgetMs = 2;
    uint32 DebugCharacterGuid = 0;
    uint32 DebugAccountId = 0;
    bool DebugRoam = false;
    uint32 DebugRoamIntervalMs = 5000;
    float DebugRoamRadius = 35.0f;
    float DebugRoamMinDistance = 8.0f;
    bool DebugCombat = false;
    uint32 DebugCombatScanIntervalMs = 1500;
    float DebugCombatSearchRange = 25.0f;
    float DebugCombatLeashRange = 60.0f;
    int32 DebugCombatMinLevelDelta = -5;
    int32 DebugCombatMaxLevelDelta = 2;
    bool DebugSpellRotation = false;
    uint32 DebugSpellCastIntervalMs = 1500;
    bool DebugLoot = false;
    uint32 DebugLootScanIntervalMs = 1000;
    float DebugLootSearchRange = 30.0f;
    uint32 DebugLootBlacklistMs = 30000;
    bool DebugRecovery = false;
    float DebugRecoveryStartHealthPct = 45.0f;
    float DebugRecoveryStopHealthPct = 85.0f;
    float DebugRecoveryStartManaPct = 20.0f;
    float DebugRecoveryStopManaPct = 75.0f;
    bool DebugConsumables = false;
    uint32 DebugConsumableScanIntervalMs = 2000;
    bool DebugDeathHandling = false;
    uint32 DebugDeathReleaseDelayMs = 5000;
    uint32 DebugDeathRespawnDelayMs = 10000;
    bool DebugQuesting = false;
    uint32 DebugQuestScanIntervalMs = 3000;
    float DebugQuestSearchRange = 35.0f;
    uint32 DebugQuestInteractTimeoutMs = 15000;
    std::vector<uint32> DebugQuestPlanIds;
};

#endif
