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
#include "CellImpl.h"
#include "Creature.h"
#include "DBCStructure.h"
#include "GridNotifiersImpl.h"
#include "Loot.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Random.h"
#include "SmartEnum.h"
#include "Spell.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldBotSession.h"
#include "WorldSession.h"
#include "Log.h"
#include <list>
#include <string>
#include <utility>
#include <vector>

namespace
{
    constexpr float TwoPi = 6.28318530718f;

    enum WorldBotSpellIds : uint32
    {
        WB_SPELL_HEROIC_STRIKE = 78,
        WB_SPELL_REND = 772,
        WB_SPELL_THUNDER_CLAP = 6343,
        WB_SPELL_CLEAVE = 845,
        WB_SPELL_SUNDER_ARMOR = 7386,
        WB_SPELL_EXECUTE = 5308,
        WB_SPELL_MORTAL_STRIKE = 12294,
        WB_SPELL_BLOODTHIRST = 23881,
        WB_SPELL_SHIELD_SLAM = 23922,

        WB_SPELL_CRUSADER_STRIKE = 35395,
        WB_SPELL_DIVINE_STORM = 53385,
        WB_SPELL_JUDGEMENT = 20271,
        WB_SPELL_HAMMER_OF_WRATH = 24275,
        WB_SPELL_CONSECRATION = 26573,

        WB_SPELL_ARCANE_SHOT = 3044,
        WB_SPELL_AIMED_SHOT = 19434,
        WB_SPELL_MULTI_SHOT = 2643,
        WB_SPELL_STEADY_SHOT = 34120,
        WB_SPELL_EXPLOSIVE_SHOT = 53301,
        WB_SPELL_SERPENT_STING = 1978,
        WB_SPELL_KILL_SHOT = 53351,

        WB_SPELL_SINISTER_STRIKE = 1752,
        WB_SPELL_EVISCERATE = 2098,
        WB_SPELL_KICK = 1766,

        WB_SPELL_SHADOW_WORD_PAIN = 589,
        WB_SPELL_MIND_BLAST = 8092,
        WB_SPELL_MIND_FLAY = 15407,
        WB_SPELL_SHADOW_WORD_DEATH = 32379,
        WB_SPELL_SMITE = 585,

        WB_SPELL_ICY_TOUCH = 45477,
        WB_SPELL_PLAGUE_STRIKE = 45462,
        WB_SPELL_BLOOD_STRIKE = 45902,
        WB_SPELL_DEATH_STRIKE = 49998,
        WB_SPELL_DEATH_COIL_DK = 47541,
        WB_AURA_FROST_FEVER = 55095,
        WB_AURA_BLOOD_PLAGUE = 55078,

        WB_SPELL_FLAME_SHOCK = 8050,
        WB_SPELL_EARTH_SHOCK = 8042,
        WB_SPELL_LIGHTNING_BOLT = 403,
        WB_SPELL_CHAIN_LIGHTNING = 421,
        WB_SPELL_LAVA_BURST = 51505,
        WB_SPELL_STORMSTRIKE = 17364,
        WB_SPELL_LAVA_LASH = 60103,

        WB_SPELL_COUNTERSPELL = 2139,
        WB_SPELL_LIVING_BOMB = 44457,
        WB_SPELL_FIREBALL = 133,
        WB_SPELL_FIRE_BLAST = 2136,
        WB_SPELL_FROSTBOLT = 116,
        WB_SPELL_ICE_LANCE = 30455,
        WB_SPELL_ARCANE_BLAST = 30451,
        WB_SPELL_ARCANE_MISSILES = 5143,

        WB_SPELL_CORRUPTION = 172,
        WB_SPELL_IMMOLATE = 348,
        WB_SPELL_CURSE_OF_AGONY = 980,
        WB_SPELL_SHADOW_BOLT = 686,
        WB_SPELL_INCINERATE = 29722,
        WB_SPELL_DRAIN_SOUL = 1120,

        WB_SPELL_INSECT_SWARM = 5570,
        WB_SPELL_MOONFIRE = 8921,
        WB_SPELL_STARFIRE = 2912,
        WB_SPELL_WRATH = 5176,
        WB_SPELL_RAKE = 1822,
        WB_SPELL_RIP = 1079,
        WB_SPELL_MANGLE_CAT = 33876,
        WB_SPELL_CLAW = 1082
    };

    struct WorldBotPreparedSpell
    {
        Spell* SpellObject = nullptr;
        Unit* Target = nullptr;
        uint32 SpellId = 0;

        explicit operator bool() const { return SpellObject != nullptr; }
    };

    using WorldBotPreparedSpellVector = std::vector<std::pair<WorldBotPreparedSpell, uint32>>;

    struct WorldBotSpellDiagnostics
    {
        uint32 CandidateCount = 0;
        uint32 AuraSkippedCount = 0;
        uint32 KnownRankCount = 0;
        uint32 ActiveSpellCount = 0;
        uint32 InvalidInfoCount = 0;
        uint32 PassiveCount = 0;
        uint32 GlobalCooldownCount = 0;
        uint32 CheckCastFailureCount = 0;
        uint32 UsableCount = 0;
        uint32 FirstNoKnownBaseSpell = 0;
        uint32 FirstInactiveSpell = 0;
        uint32 FirstCheckCastSpell = 0;
        SpellCastResult FirstCheckCastResult = SPELL_CAST_OK;
        float FirstCheckCastDistance = 0.0f;
        bool FirstCheckCastMoving = false;
        bool FirstCheckCastStopped = true;
    };

    WorldBotSpellDiagnostics* ActiveWorldBotSpellDiagnostics = nullptr;

    struct WorldBotSpellDiagnosticsScope
    {
        explicit WorldBotSpellDiagnosticsScope(WorldBotSpellDiagnostics* diagnostics) : Previous(ActiveWorldBotSpellDiagnostics)
        {
            ActiveWorldBotSpellDiagnostics = diagnostics;
        }

        ~WorldBotSpellDiagnosticsScope()
        {
            ActiveWorldBotSpellDiagnostics = Previous;
        }

        WorldBotSpellDiagnostics* Previous;
    };

    WorldBotPreparedSpell VerifyWorldBotSpellCast(Player* bot, uint32 spellId, Unit* target, bool debug)
    {
        if (!bot || !spellId)
            return {};

        WorldBotSpellDiagnostics* diagnostics = ActiveWorldBotSpellDiagnostics;

        uint32 knownRank = 0;
        uint32 nextRank = 0;

        if (bot->HasSpell(spellId))
        {
            knownRank = spellId;
            nextRank = sSpellMgr->GetNextSpellInChain(spellId);
        }
        else
            nextRank = sSpellMgr->GetFirstSpellInChain(spellId);

        while (nextRank && bot->HasSpell(nextRank))
        {
            knownRank = nextRank;
            nextRank = sSpellMgr->GetNextSpellInChain(knownRank);
        }

        if (!knownRank)
        {
            if (diagnostics && !diagnostics->FirstNoKnownBaseSpell)
                diagnostics->FirstNoKnownBaseSpell = spellId;

            if (debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} cannot cast base spell {}: no known rank.", bot->GetName(), spellId);

            return {};
        }

        if (diagnostics)
            ++diagnostics->KnownRankCount;

        if (!bot->HasActiveSpell(knownRank))
        {
            if (diagnostics && !diagnostics->FirstInactiveSpell)
                diagnostics->FirstInactiveSpell = knownRank;

            if (debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} cannot cast spell {} from base {}: not active in spellbook.", bot->GetName(), knownRank, spellId);

            return {};
        }

        if (diagnostics)
            ++diagnostics->ActiveSpellCount;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(knownRank);
        if (!spellInfo)
        {
            if (diagnostics)
                ++diagnostics->InvalidInfoCount;

            if (debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} cannot cast spell {} from base {}: no SpellInfo.", bot->GetName(), knownRank, spellId);

            return {};
        }

        if (spellInfo->IsPassive())
        {
            if (diagnostics)
                ++diagnostics->PassiveCount;

            return {};
        }

        if (bot->GetSpellHistory()->HasGlobalCooldown(spellInfo))
        {
            if (diagnostics)
                ++diagnostics->GlobalCooldownCount;

            if (debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} cannot cast spell {}: global cooldown.", bot->GetName(), knownRank);

            return {};
        }

        Spell* spell = new Spell(bot, spellInfo, TRIGGERED_NONE);
        SpellCastTargets targets;
        targets.SetUnitTarget(target);
        spell->InitExplicitTargets(targets);

        bot->SetInFront(target);

        SpellCastResult result = spell->CheckCast(true);
        if (result == SPELL_CAST_OK)
        {
            if (diagnostics)
                ++diagnostics->UsableCount;

            return { spell, target, spell->GetSpellInfo()->Id };
        }

        if (diagnostics)
        {
            ++diagnostics->CheckCastFailureCount;
            if (!diagnostics->FirstCheckCastSpell)
            {
                diagnostics->FirstCheckCastSpell = knownRank;
                diagnostics->FirstCheckCastResult = result;
                diagnostics->FirstCheckCastDistance = bot->GetDistance(target);
                diagnostics->FirstCheckCastMoving = bot->isMoving();
                diagnostics->FirstCheckCastStopped = bot->IsStopped();
            }
        }

        if (debug)
            TC_LOG_DEBUG("server.worldbots", "WorldBot {} cannot cast spell {} from base {}: CheckCast result {} ({}) distance={} moving={} stopped={}.",
                bot->GetName(), knownRank, spellId, uint32(result), EnumUtils::ToConstant(result), bot->GetDistance(target), bot->isMoving(),
                bot->IsStopped());

        delete spell;
        return {};
    }

    void PushWorldBotSpellCast(WorldBotPreparedSpellVector& spells, Player* bot, Unit* target, uint32 spellId, uint32 weight,
        bool skipIfTargetHasCasterAura = false, uint32 auraSpellId = 0, bool debug = false)
    {
        if (!weight || !target)
            return;

        WorldBotSpellDiagnostics* diagnostics = ActiveWorldBotSpellDiagnostics;
        if (diagnostics)
            ++diagnostics->CandidateCount;

        uint32 auraToCheck = auraSpellId ? auraSpellId : spellId;
        if (skipIfTargetHasCasterAura && target->GetAuraApplicationOfRankedSpell(auraToCheck, bot->GetGUID()))
        {
            if (diagnostics)
                ++diagnostics->AuraSkippedCount;

            return;
        }

        if (WorldBotPreparedSpell spell = VerifyWorldBotSpellCast(bot, spellId, target, debug))
            spells.push_back({ spell, weight });
    }

    WorldBotPreparedSpell SelectWorldBotSpellCast(WorldBotPreparedSpellVector& spells)
    {
        uint32 totalWeights = 0;
        for (auto const& weightedSpell : spells)
            totalWeights += weightedSpell.second;

        if (!totalWeights)
            return {};

        WorldBotPreparedSpell selected;
        uint32 randNum = urand(0, totalWeights - 1);
        for (auto const& weightedSpell : spells)
        {
            if (selected)
            {
                delete weightedSpell.first.SpellObject;
                continue;
            }

            if (randNum < weightedSpell.second)
                selected = weightedSpell.first;
            else
            {
                randNum -= weightedSpell.second;
                delete weightedSpell.first.SpellObject;
            }
        }

        spells.clear();
        return selected;
    }

    WorldBotPreparedSpell SelectWorldBotClassSpell(Player* bot, Unit* victim, bool debug)
    {
        WorldBotPreparedSpellVector spells;

        switch (bot->GetClass())
        {
            case CLASS_WARRIOR:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_REND, 3, true, 0, debug);
                if (victim->HealthBelowPct(20))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_EXECUTE, 10, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MORTAL_STRIKE, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_BLOODTHIRST, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SHIELD_SLAM, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_THUNDER_CLAP, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SUNDER_ARMOR, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CLEAVE, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_HEROIC_STRIKE, 6, false, 0, debug);
                break;
            case CLASS_PALADIN:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CRUSADER_STRIKE, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_DIVINE_STORM, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_JUDGEMENT, 4, false, 0, debug);
                if (victim->HealthBelowPct(20))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_HAMMER_OF_WRATH, 8, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CONSECRATION, 1, false, 0, debug);
                break;
            case CLASS_HUNTER:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SERPENT_STING, 4, true, 0, debug);
                if (victim->HealthBelowPct(20))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_KILL_SHOT, 10, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_EXPLOSIVE_SHOT, 6, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_AIMED_SHOT, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_ARCANE_SHOT, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MULTI_SHOT, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_STEADY_SHOT, 2, false, 0, debug);
                break;
            case CLASS_ROGUE:
                if (victim->HasUnitState(UNIT_STATE_CASTING))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_KICK, 12, false, 0, debug);
                if (bot->GetPower(POWER_COMBO) >= 4)
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_EVISCERATE, 10, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SINISTER_STRIKE, 6, false, 0, debug);
                break;
            case CLASS_PRIEST:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SHADOW_WORD_PAIN, 4, true, 0, debug);
                if (victim->HealthBelowPct(25))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SHADOW_WORD_DEATH, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MIND_BLAST, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MIND_FLAY, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SMITE, 2, false, 0, debug);
                break;
            case CLASS_DEATH_KNIGHT:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_ICY_TOUCH, 5, true, WB_AURA_FROST_FEVER, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_PLAGUE_STRIKE, 5, true, WB_AURA_BLOOD_PLAGUE, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_DEATH_STRIKE, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_BLOOD_STRIKE, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_DEATH_COIL_DK, 2, false, 0, debug);
                break;
            case CLASS_SHAMAN:
                if (victim->GetAuraApplicationOfRankedSpell(WB_SPELL_FLAME_SHOCK, bot->GetGUID()))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_LAVA_BURST, 6, false, 0, debug);
                else
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_FLAME_SHOCK, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_STORMSTRIKE, 5, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_EARTH_SHOCK, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CHAIN_LIGHTNING, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_LIGHTNING_BOLT, 2, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_LAVA_LASH, 2, false, 0, debug);
                break;
            case CLASS_MAGE:
                if (victim->HasUnitState(UNIT_STATE_CASTING))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_COUNTERSPELL, 12, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_LIVING_BOMB, 4, true, 0, debug);
                if (victim->HasAuraState(AURA_STATE_FROZEN, nullptr, bot))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_ICE_LANCE, 8, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_FIRE_BLAST, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_ARCANE_BLAST, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_ARCANE_MISSILES, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_FROSTBOLT, 4, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_FIREBALL, 3, false, 0, debug);
                break;
            case CLASS_WARLOCK:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CORRUPTION, 6, true, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_IMMOLATE, 5, true, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CURSE_OF_AGONY, 3, true, 0, debug);
                if (victim->HealthBelowPct(25))
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_DRAIN_SOUL, 8, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_INCINERATE, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_SHADOW_BOLT, 5, false, 0, debug);
                break;
            case CLASS_DRUID:
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_INSECT_SWARM, 4, true, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MOONFIRE, 4, true, 0, debug);
                if (bot->GetPower(POWER_COMBO) >= 4)
                    PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_RIP, 8, true, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_MANGLE_CAT, 6, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_RAKE, 5, true, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_CLAW, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_STARFIRE, 3, false, 0, debug);
                PushWorldBotSpellCast(spells, bot, victim, WB_SPELL_WRATH, 4, false, 0, debug);
                break;
            default:
                break;
        }

        return SelectWorldBotSpellCast(spells);
    }
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
    _debugCombatScanTimer = 0;
    _debugSpellCastTimer = 0;
    _debugLootScanTimer = 0;
    _debugLootMoveTimer = 0;
    _debugLootTargetTimer = 0;
    _debugRecovering = false;
    _debugLootTargetGuid.Clear();
    _debugLootBlacklist.clear();
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
    if (!_config.Enabled)
        return;

    if (UpdateDebugBotLoot(map, diff))
        return;

    if (UpdateDebugBotRecovery(map, diff))
        return;

    if (UpdateDebugBotCombat(map, diff))
        return;

    if (!_config.DebugRoam)
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

bool WorldBotMgr::UpdateDebugBotLoot(Map* map, uint32 diff)
{
    if (!_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    UpdateDebugLootBlacklist(diff);

    if (ObjectGuid lootGuid = bot->GetLootGUID())
    {
        if (!_config.DebugLoot)
        {
            _debugSession->DoLootRelease(lootGuid);
            return false;
        }

        if (lootGuid.IsCreatureOrVehicle())
            if (Creature* creature = map->GetCreature(lootGuid))
                return LootDebugBotCreature(bot, creature);

        _debugSession->DoLootRelease(lootGuid);
        BlacklistDebugLootTarget(lootGuid);
        return false;
    }

    if (!_config.DebugLoot)
        return false;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsInCombat() || bot->GetVictim() || bot->IsBeingTeleported())
        return false;

    if (_debugLootTargetGuid)
    {
        if (_debugLootTargetTimer <= diff)
        {
            BlacklistDebugLootTarget(_debugLootTargetGuid);
            _debugLootTargetGuid.Clear();
            _debugLootMoveTimer = 0;
            _debugLootTargetTimer = 0;
            return false;
        }

        _debugLootTargetTimer -= diff;

        Creature* creature = map->GetCreature(_debugLootTargetGuid);
        if (!IsDebugBotLootCandidate(bot, creature))
        {
            BlacklistDebugLootTarget(_debugLootTargetGuid);
            _debugLootTargetGuid.Clear();
            _debugLootMoveTimer = 0;
            _debugLootTargetTimer = 0;
            return false;
        }

        if (!bot->IsWithinDistInMap(creature, INTERACTION_DISTANCE))
        {
            if (bot->IsMounted())
                bot->Dismount();

            if (_debugLootMoveTimer > diff && !bot->IsStopped())
            {
                _debugLootMoveTimer -= diff;
                return true;
            }

            _debugLootMoveTimer = 1000;
            Position destination = creature->GetPosition();
            bot->GetMotionMaster()->MovePoint(_debugMovePointId++, destination, true);

            if (_config.Debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} moving to loot {} distance={}.", bot->GetName(),
                    creature->GetGUID().ToString(), bot->GetDistance(creature));

            return true;
        }

        _debugLootTargetGuid.Clear();
        _debugLootMoveTimer = 0;
        _debugLootTargetTimer = 0;
        return LootDebugBotCreature(bot, creature);
    }

    if (_debugLootScanTimer > diff)
    {
        _debugLootScanTimer -= diff;
        return false;
    }

    _debugLootScanTimer = _config.DebugLootScanIntervalMs;

    Creature* target = SelectDebugBotLootTarget(bot);
    if (!target)
        return false;

    _debugLootTargetGuid = target->GetGUID();
    _debugLootMoveTimer = 0;
    _debugLootTargetTimer = 15000;
    return true;
}

bool WorldBotMgr::UpdateDebugBotRecovery(Map* map, uint32 /*diff*/)
{
    if (!_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!_config.DebugRecovery)
    {
        if (_debugRecovering)
        {
            _debugRecovering = false;
            if (!bot->IsStandState())
                bot->SetStandState(UNIT_STAND_STATE_STAND);
        }

        return false;
    }

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsBeingTeleported())
    {
        _debugRecovering = false;
        return false;
    }

    if (bot->IsInCombat() || bot->GetVictim())
        return false;

    bool const hasActiveMana = bot->GetPowerType() == POWER_MANA && bot->GetMaxPower(POWER_MANA) > 0;
    bool const needsHealth = bot->GetHealthPct() <= _config.DebugRecoveryStartHealthPct;
    bool const needsMana = hasActiveMana && bot->GetPowerPct(POWER_MANA) <= _config.DebugRecoveryStartManaPct;

    if (!_debugRecovering && !needsHealth && !needsMana)
        return false;

    bool const healthReady = bot->GetHealthPct() >= _config.DebugRecoveryStopHealthPct;
    bool const manaReady = !hasActiveMana || bot->GetPowerPct(POWER_MANA) >= _config.DebugRecoveryStopManaPct;

    if (_debugRecovering && healthReady && manaReady)
    {
        _debugRecovering = false;
        if (!bot->IsStandState())
            bot->SetStandState(UNIT_STAND_STATE_STAND);

        if (_config.Debug)
            TC_LOG_DEBUG("server.worldbots", "WorldBot {} recovery complete healthPct={} manaPct={}.", bot->GetName(),
                bot->GetHealthPct(), hasActiveMana ? bot->GetPowerPct(POWER_MANA) : 100.0f);

        return false;
    }

    if (!_debugRecovering)
    {
        _debugRecovering = true;
        if (_config.Debug)
            TC_LOG_DEBUG("server.worldbots", "WorldBot {} entering recovery healthPct={} manaPct={}.", bot->GetName(),
                bot->GetHealthPct(), hasActiveMana ? bot->GetPowerPct(POWER_MANA) : 100.0f);
    }

    if (bot->IsMounted())
        bot->Dismount();

    if (!bot->IsStopped())
        bot->StopMoving();

    if (bot->GetStandState() != UNIT_STAND_STATE_SIT)
        bot->SetStandState(UNIT_STAND_STATE_SIT);

    return true;
}

bool WorldBotMgr::UpdateDebugBotCombat(Map* map, uint32 diff)
{
    if (!_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    Unit* victim = bot->GetVictim();
    if (victim)
    {
        _debugCombatNoVictimLogTimer = 0;

        if (!victim->IsAlive() || !bot->IsValidAttackTarget(victim) || !bot->IsWithinDistInMap(victim, _config.DebugCombatLeashRange))
        {
            bot->AttackStop();
            bot->CombatStop(true);
            _debugSpellCastTimer = 0;
            return false;
        }

        if (UpdateDebugBotSpellRotation(bot, victim, diff))
            return true;

        if (bot->HasUnitState(UNIT_STATE_CASTING))
            return true;

        if (!bot->IsWithinMeleeRange(victim) && bot->IsStopped())
            bot->GetMotionMaster()->MoveChase(victim);

        return true;
    }

    if (bot->IsInCombat())
    {
        Unit* attackerTarget = nullptr;
        for (Unit* attacker : bot->getAttackers())
        {
            if (!attacker || !attacker->IsAlive() || !bot->IsValidAttackTarget(attacker) || !bot->IsWithinDistInMap(attacker, _config.DebugCombatLeashRange))
                continue;

            attackerTarget = attacker;
            break;
        }

        if (attackerTarget && bot->Attack(attackerTarget, true))
        {
            if (UpdateDebugBotSpellRotation(bot, attackerTarget, diff))
                return true;

            if (!bot->IsWithinMeleeRange(attackerTarget) && bot->IsStopped())
                bot->GetMotionMaster()->MoveChase(attackerTarget);

            return true;
        }

        if (_config.Debug)
        {
            if (_debugCombatNoVictimLogTimer <= diff)
            {
                TC_LOG_INFO("server.worldbots", "WorldBot {} is in combat without a victim: attackers={} spellRotation={}.",
                    bot->GetName(), bot->getAttackers().size(), _config.DebugSpellRotation);
                _debugCombatNoVictimLogTimer = 5000;
            }
            else
                _debugCombatNoVictimLogTimer -= diff;
        }

        return true;
    }

    if (!_config.DebugCombat)
        return false;

    if (_debugCombatScanTimer > diff)
    {
        _debugCombatScanTimer -= diff;
        return false;
    }

    _debugCombatScanTimer = _config.DebugCombatScanIntervalMs;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsBeingTeleported() || bot->HasUnitState(UNIT_STATE_NOT_MOVE))
        return false;

    Unit* target = SelectDebugBotCombatTarget(bot);
    if (!target)
        return false;

    if (bot->IsMounted())
        bot->Dismount();

    if (!bot->Attack(target, true))
        return false;

    bot->GetMotionMaster()->MoveChase(target);

    if (_config.Debug)
        TC_LOG_DEBUG("server.worldbots", "WorldBot {} attacking {} level={} distance={}", bot->GetName(), target->GetGUID().ToString(),
            target->GetLevelForTarget(bot), bot->GetDistance(target));

    return true;
}

bool WorldBotMgr::UpdateDebugBotSpellRotation(Player* bot, Unit* victim, uint32 diff)
{
    if (!_config.DebugSpellRotation)
    {
        if (_config.Debug)
        {
            if (_debugSpellDisabledLogTimer <= diff)
            {
                TC_LOG_WARN("server.worldbots", "WorldBot spell rotation is disabled while combat is active. Set WorldBots.Debug.SpellRotation = 1.");
                _debugSpellDisabledLogTimer = 5000;
            }
            else
                _debugSpellDisabledLogTimer -= diff;
        }

        return false;
    }

    _debugSpellDisabledLogTimer = 0;

    if (!bot || !victim || !bot->IsAlive() || !victim->IsAlive())
        return false;

    if (bot->HasUnitState(UNIT_STATE_CASTING))
    {
        if (!bot->IsStopped())
            bot->StopMoving();

        return true;
    }

    if (_debugSpellCastTimer > diff)
    {
        _debugSpellCastTimer -= diff;
        return false;
    }

    _debugSpellCastTimer = _config.DebugSpellCastIntervalMs;

    if (bot->IsMounted())
        bot->Dismount();

    if (!bot->IsStandState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    bool const wasMoving = !bot->IsStopped();
    if (wasMoving && bot->IsWithinDistInMap(victim, 35.0f))
        bot->StopMoving();

    bot->SetInFront(victim);

    WorldBotSpellDiagnostics diagnostics;
    WorldBotPreparedSpell spell;
    {
        WorldBotSpellDiagnosticsScope diagnosticsScope(&diagnostics);
        spell = SelectWorldBotClassSpell(bot, victim, _config.Debug);
    }

    if (!spell)
    {
        if (_config.Debug)
        {
            TC_LOG_INFO("server.worldbots", "WorldBot {} found no valid spell: class={} target={} distance={} candidates={} auraSkipped={} known={} active={} gcd={} checkCastFailed={} usable={} firstNoKnownBase={} firstInactive={} powerType={} power={} moving={} stopped={}.",
                bot->GetName(), bot->GetClass(), victim->GetGUID().ToString(), bot->GetDistance(victim), diagnostics.CandidateCount,
                diagnostics.AuraSkippedCount, diagnostics.KnownRankCount, diagnostics.ActiveSpellCount, diagnostics.GlobalCooldownCount,
                diagnostics.CheckCastFailureCount, diagnostics.UsableCount, diagnostics.FirstNoKnownBaseSpell, diagnostics.FirstInactiveSpell,
                uint32(bot->GetPowerType()), bot->GetPower(bot->GetPowerType()), bot->isMoving(), bot->IsStopped());

            if (diagnostics.FirstCheckCastSpell)
                TC_LOG_INFO("server.worldbots", "WorldBot {} first CheckCast failure: spell={} result={} ({}) distance={} moving={} stopped={}.",
                    bot->GetName(), diagnostics.FirstCheckCastSpell, uint32(diagnostics.FirstCheckCastResult),
                    EnumUtils::ToConstant(diagnostics.FirstCheckCastResult), diagnostics.FirstCheckCastDistance, diagnostics.FirstCheckCastMoving,
                    diagnostics.FirstCheckCastStopped);
        }

        if (wasMoving && bot->IsStopped() && !bot->IsWithinMeleeRange(victim))
            bot->GetMotionMaster()->MoveChase(victim);

        return false;
    }

    SpellCastTargets targets;
    targets.SetUnitTarget(spell.Target);
    bot->GetMotionMaster()->Clear();
    bot->StopMoving();
    bot->SetInFront(spell.Target);
    SpellCastResult result = spell.SpellObject->prepare(targets);

    if (_config.Debug)
    {
        if (result == SPELL_CAST_OK)
            TC_LOG_INFO("server.worldbots", "WorldBot {} casting spell {} at {} distance={} powerType={} power={}.", bot->GetName(), spell.SpellId,
                spell.Target->GetGUID().ToString(), bot->GetDistance(spell.Target), uint32(bot->GetPowerType()), bot->GetPower(bot->GetPowerType()));
        else
            TC_LOG_WARN("server.worldbots", "WorldBot {} prepare rejected spell {}: result={} ({}) distance={} moving={} stopped={}.", bot->GetName(),
                spell.SpellId, uint32(result), EnumUtils::ToConstant(result), bot->GetDistance(spell.Target), bot->isMoving(), bot->IsStopped());
    }

    return result == SPELL_CAST_OK;
}

Creature* WorldBotMgr::SelectDebugBotLootTarget(Player* bot) const
{
    std::list<Unit*> targets;
    Trinity::AllDeadCreaturesInRange check(bot, _config.DebugLootSearchRange);
    Trinity::UnitListSearcher<Trinity::AllDeadCreaturesInRange> searcher(bot, targets, check);
    Cell::VisitAllObjects(bot, searcher, _config.DebugLootSearchRange);

    Creature* bestTarget = nullptr;
    float bestDistance = _config.DebugLootSearchRange;

    for (Unit* unit : targets)
    {
        Creature* creature = unit ? unit->ToCreature() : nullptr;
        if (!IsDebugBotLootCandidate(bot, creature))
            continue;

        if (IsDebugLootTargetBlacklisted(creature->GetGUID()))
            continue;

        float distance = bot->GetDistance(creature);
        if (distance > bestDistance)
            continue;

        bestDistance = distance;
        bestTarget = creature;
    }

    return bestTarget;
}

bool WorldBotMgr::IsDebugBotLootCandidate(Player* bot, Creature* creature) const
{
    if (!bot || !creature || creature->IsAlive() || creature->GetMap() != bot->GetMap())
        return false;

    if (!creature->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE) || creature->loot.isLooted())
        return false;

    if (creature->GetLootRecipientGUID() != bot->GetGUID() && creature->loot.lootOwnerGUID != bot->GetGUID())
        return false;

    if (!creature->IsWithinDistInMap(bot, _config.DebugLootSearchRange))
        return false;

    return true;
}

bool WorldBotMgr::LootDebugBotCreature(Player* bot, Creature* creature)
{
    if (!IsDebugBotLootCandidate(bot, creature))
    {
        if (creature)
            BlacklistDebugLootTarget(creature->GetGUID());

        return false;
    }

    if (!bot->IsWithinDistInMap(creature, INTERACTION_DISTANCE))
    {
        _debugLootTargetGuid = creature->GetGUID();
        if (!_debugLootTargetTimer)
            _debugLootTargetTimer = 15000;
        return true;
    }

    if (!bot->IsStopped())
        bot->StopMoving();

    ObjectGuid const lootGuid = creature->GetGUID();
    if (bot->GetLootGUID() != lootGuid)
        bot->SendLoot(lootGuid, LOOT_CORPSE);

    if (bot->GetLootGUID() != lootGuid)
    {
        BlacklistDebugLootTarget(lootGuid);
        return false;
    }

    Loot* loot = &creature->loot;
    uint8 const startUnlootedCount = loot->unlootedCount;
    uint32 const startGold = loot->gold;

    if (loot->gold)
    {
        uint32 gold = loot->gold;
        uint32 const goldMod = CalculatePct(gold, bot->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MONEY_GAIN, 0)) +
            CalculatePct(gold, bot->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MONEY_GAIN, 2));
        if (goldMod)
            gold += goldMod;

        loot->NotifyMoneyRemoved();
        bot->ModifyMoney(gold);
        bot->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_MONEY, gold);
        loot->gold = 0;
    }

    uint32 const maxSlot = loot->GetMaxSlotInLootFor(bot);
    for (uint32 slot = 0; slot < maxSlot && !loot->isLooted(); ++slot)
    {
        LootItem* item = loot->LootItemInSlot(slot, bot);
        if (!item || item->is_looted)
            continue;

        bot->StoreLootItem(uint8(slot), loot);
    }

    bool const progressed = startGold != loot->gold || startUnlootedCount != loot->unlootedCount;
    bool const fullyLooted = loot->isLooted();
    uint8 const remainingItems = loot->unlootedCount;
    uint32 const remainingGold = loot->gold;

    _debugSession->DoLootRelease(lootGuid);

    if (!progressed || !fullyLooted)
        BlacklistDebugLootTarget(lootGuid);

    if (_config.Debug)
        TC_LOG_DEBUG("server.worldbots", "WorldBot {} looted {} progressed={} fullyLooted={} remainingItems={} remainingGold={}.",
            bot->GetName(), lootGuid.ToString(), progressed, fullyLooted, remainingItems, remainingGold);

    return true;
}

void WorldBotMgr::BlacklistDebugLootTarget(ObjectGuid const& guid)
{
    if (!guid)
        return;

    if (_debugLootTargetGuid == guid)
    {
        _debugLootTargetGuid.Clear();
        _debugLootMoveTimer = 0;
        _debugLootTargetTimer = 0;
    }

    for (auto& blocked : _debugLootBlacklist)
    {
        if (blocked.first == guid)
        {
            blocked.second = _config.DebugLootBlacklistMs;
            return;
        }
    }

    _debugLootBlacklist.push_back({ guid, _config.DebugLootBlacklistMs });
}

bool WorldBotMgr::IsDebugLootTargetBlacklisted(ObjectGuid const& guid) const
{
    for (auto const& blocked : _debugLootBlacklist)
        if (blocked.first == guid)
            return true;

    return false;
}

void WorldBotMgr::UpdateDebugLootBlacklist(uint32 diff)
{
    for (auto itr = _debugLootBlacklist.begin(); itr != _debugLootBlacklist.end();)
    {
        if (itr->second <= diff)
            itr = _debugLootBlacklist.erase(itr);
        else
        {
            itr->second -= diff;
            ++itr;
        }
    }
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

Unit* WorldBotMgr::SelectDebugBotCombatTarget(Player* bot) const
{
    std::list<Unit*> targets;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck check(bot, bot, _config.DebugCombatSearchRange);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(bot, targets, check);
    Cell::VisitAllObjects(bot, searcher, _config.DebugCombatSearchRange);

    Unit* bestTarget = nullptr;
    float bestDistance = _config.DebugCombatSearchRange;

    for (Unit* unit : targets)
    {
        if (!unit || unit == bot || unit->GetTypeId() != TYPEID_UNIT)
            continue;

        Creature* creature = unit->ToCreature();
        if (!creature || creature->IsPet() || creature->IsTotem() || creature->IsCritter() || creature->IsSpiritService())
            continue;

        if (creature->IsCivilian() || creature->IsGuard() || creature->IsTrigger() || creature->isWorldBoss() || creature->IsInEvadeMode())
            continue;

        if (creature->IsFlying() || creature->IsInCombat())
            continue;

        if (!creature->isTargetableForAttack(false) || !bot->IsValidAttackTarget(creature) || !bot->CanSeeOrDetect(creature) || !bot->IsWithinLOSInMap(creature))
            continue;

        int32 levelDelta = int32(creature->GetLevelForTarget(bot)) - int32(bot->GetLevel());
        if (levelDelta < _config.DebugCombatMinLevelDelta || levelDelta > _config.DebugCombatMaxLevelDelta)
            continue;

        float distance = bot->GetDistance(creature);
        if (distance > bestDistance)
            continue;

        bestDistance = distance;
        bestTarget = creature;
    }

    return bestTarget;
}
