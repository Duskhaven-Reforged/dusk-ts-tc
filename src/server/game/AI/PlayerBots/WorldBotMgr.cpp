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
#include "Bag.h"
#include "CharacterCache.h"
#include "CellImpl.h"
#include "Corpse.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureData.h"
#include "DatabaseEnv.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "GridNotifiersImpl.h"
#include "GossipDef.h"
#include "Loot.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Item.h"
#include "Player.h"
#include "QuestDef.h"
#include "Random.h"
#include "ScriptMgr.h"
#include "SmartEnum.h"
#include "Spell.h"
#include "SpellAuraDefines.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldBotSession.h"
#include "WorldSession.h"
// @tswow-begin
#include "TSPlayer.h"
#include "TSEvents.h"
// @tswow-end
#include "Log.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <list>
#include <memory>
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

    struct WorldBotConsumableCandidate
    {
        Item* Consumable = nullptr;
        uint32 SpellId = 0;
        bool RestoresHealth = false;
        bool RestoresMana = false;

        explicit operator bool() const { return Consumable != nullptr; }
    };

    bool WorldBotSpellRestoresForRecovery(SpellInfo const* spellInfo, bool& restoresHealth, bool& restoresMana)
    {
        if (!spellInfo)
            return false;

        for (SpellEffectInfo const& effect : spellInfo->GetEffects())
        {
            if (!effect.IsAura())
                continue;

            switch (effect.ApplyAuraName)
            {
                case SPELL_AURA_MOD_REGEN:
                case SPELL_AURA_MOD_REGEN_DURING_COMBAT:
                    restoresHealth = true;
                    break;
                case SPELL_AURA_MOD_POWER_REGEN:
                case SPELL_AURA_MOD_POWER_REGEN_PERCENT:
                    if (effect.MiscValue == POWER_MANA || effect.MiscValue < 0)
                        restoresMana = true;
                    break;
                case SPELL_AURA_PERIODIC_ENERGIZE:
                    if (effect.MiscValue == POWER_MANA)
                        restoresMana = true;
                    break;
                default:
                    break;
            }
        }

        return restoresHealth || restoresMana;
    }

    WorldBotConsumableCandidate GetWorldBotConsumableCandidate(Item* item, bool needsHealth, bool needsMana)
    {
        if (!item)
            return {};

        ItemTemplate const* itemTemplate = item->GetTemplate();
        if (!itemTemplate || itemTemplate->Class != ITEM_CLASS_CONSUMABLE || itemTemplate->SubClass != ITEM_SUBCLASS_FOOD)
            return {};

        for (_Spell const& spellData : itemTemplate->Spells)
        {
            if (spellData.SpellId <= 0)
                continue;

            if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                continue;

            bool restoresHealth = false;
            bool restoresMana = false;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellData.SpellId);
            if (!WorldBotSpellRestoresForRecovery(spellInfo, restoresHealth, restoresMana))
                return {};

            if ((needsHealth && restoresHealth) || (needsMana && restoresMana))
                return { item, uint32(spellData.SpellId), restoresHealth, restoresMana };

            return {};
        }

        return {};
    }

    bool FinishWorldBotNearTeleport(Player* bot)
    {
        if (!bot || !bot->IsBeingTeleportedNear())
            return false;

        bot->SetSemaphoreTeleportNear(false);

        WorldLocation const& destination = bot->GetTeleportDest();
        bot->UpdatePosition(destination, true);
        bot->UpdateArea(bot->GetAreaIdFromPosition());
        bot->SetFallInformation(0, bot->GetPositionZ());
        bot->ResummonPetTemporaryUnSummonedIfAny();
        bot->ProcessDelayedOperations();

        return true;
    }

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
    _debugConsumableScanTimer = 0;
    _debugDeathReleaseTimer = 0;
    _debugDeathRespawnTimer = 0;
    _debugQuestScanTimer = 0;
    _debugQuestMoveTimer = 0;
    _debugQuestTargetTimer = 0;
    _debugQuestTravelTimer = 0;
    _debugDeathReleased = false;
    _generatedPrepareAttempted = false;
    _debugLootTargetGuid.Clear();
    _debugQuestTargetGuid.Clear();
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

    if (_config.GeneratedEnabled)
        TC_LOG_INFO("server.worldbots", "WorldBots generated profile preparation enabled: account={} count={} profiles={}.",
            _config.GeneratedAccountId, _config.GeneratedCount, _config.GeneratedProfiles.size());
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

    PrepareGeneratedBots();
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

    if (UpdateDebugBotDeath(map, diff))
        return;

    if (UpdateDebugBotLoot(map, diff))
        return;

    if (UpdateDebugBotRecovery(map, diff))
        return;

    if (UpdateDebugBotQuestPlanCompletion(map))
        return;

    if (UpdateDebugBotQuesting(map, diff))
        return;

    if (UpdateDebugBotQuestTravel(map, diff))
        return;

    if (UpdateDebugBotCombat(map, diff))
        return;

    if (_debugSession)
        if (Player* bot = _debugSession->GetPlayer())
            if (bot->GetMap() == map && HasDebugBotActivePlannedKillObjective(bot))
                return;

    if (!_config.DebugRoam)
        return;

    UpdateDebugBotRoam(map, diff);
}

void WorldBotMgr::PrepareGeneratedBots()
{
    if (_generatedPrepareAttempted || !_config.GeneratedEnabled)
        return;

    _generatedPrepareAttempted = true;

    if (!_config.GeneratedAccountId)
    {
        TC_LOG_WARN("server.worldbots", "Skipping generated WorldBot preparation: WorldBots.Generated.AccountId is 0.");
        return;
    }

    if (_config.GeneratedProfiles.empty())
    {
        TC_LOG_WARN("server.worldbots", "Skipping generated WorldBot preparation: no generated profiles configured.");
        return;
    }

    uint32 const targetCount = _config.GeneratedCount ? _config.GeneratedCount : uint32(_config.GeneratedProfiles.size());
    if (!targetCount)
        return;

    uint32 createdCount = 0;
    uint32 existingCount = 0;
    for (uint32 index = 0; index < targetCount; ++index)
    {
        std::string const name = BuildGeneratedBotName(index);
        if (sCharacterCache->GetCharacterCacheByName(name))
        {
            ++existingCount;
            TC_LOG_DEBUG("server.worldbots", "Generated WorldBot {} already exists; keeping existing character.", name);
            continue;
        }

        WorldBotGeneratedProfile const& profile = _config.GeneratedProfiles[index % _config.GeneratedProfiles.size()];
        if (MaterializeGeneratedBotProfile(profile, index))
            ++createdCount;
    }

    TC_LOG_INFO("server.worldbots", "Generated WorldBot preparation complete: target={}, created={}, existing={}.", targetCount, createdCount, existingCount);
}

std::string WorldBotMgr::BuildGeneratedBotName(uint32 index) const
{
    std::string suffix;
    uint32 value = index;
    do
    {
        suffix.insert(suffix.begin(), char('a' + (value % 26)));
        value = value / 26;
    } while (value);

    std::string prefix = _config.GeneratedNamePrefix.empty() ? "Worldbot" : _config.GeneratedNamePrefix;
    if (prefix.size() + suffix.size() > 12)
        prefix.resize(12 - std::min<size_t>(suffix.size(), 12));

    std::string name = prefix + suffix;
    if (!normalizePlayerName(name))
        name = "Worldbot" + suffix;

    if (name.size() > 12)
        name.resize(12);

    normalizePlayerName(name);
    return name;
}

bool WorldBotMgr::MaterializeGeneratedBotProfile(WorldBotGeneratedProfile const& profile, uint32 index)
{
    if (!profile.Race || !profile.Class)
    {
        TC_LOG_WARN("server.worldbots", "Skipping generated WorldBot profile '{}': race/class must be configured.", profile.Id);
        return false;
    }

    if (!sObjectMgr->GetPlayerInfo(profile.Race, profile.Class))
    {
        TC_LOG_WARN("server.worldbots", "Skipping generated WorldBot profile '{}': invalid race/class pair {}/{}.", profile.Id,
            uint32(profile.Race), uint32(profile.Class));
        return false;
    }

    std::string const name = BuildGeneratedBotName(index);
    if (ObjectMgr::CheckPlayerName(name, LOCALE_enUS, true) != CHAR_NAME_SUCCESS)
    {
        TC_LOG_WARN("server.worldbots", "Skipping generated WorldBot profile '{}': generated name '{}' is invalid.", profile.Id, name);
        return false;
    }

    if (sCharacterCache->GetCharacterCacheByName(name))
        return false;

    CharacterCreateInfo createInfo;
    createInfo.Name = name;
    createInfo.Race = profile.Race;
    createInfo.Class = profile.Class;
    createInfo.Gender = profile.Gender;
    createInfo.OutfitId = 0;

    std::string accountName = "WorldBotGenerator" + std::to_string(_config.GeneratedAccountId);
    std::unique_ptr<WorldSession> session = WorldBotSession::Create(_config.GeneratedAccountId, std::move(accountName),
        uint8(sWorld->getIntConfig(CONFIG_EXPANSION)));
    std::unique_ptr<Player, void(*)(Player*)> newChar(new Player(session.get()), [](Player* player)
    {
        if (player)
        {
            player->CleanupsBeforeDelete();
            delete player;
        }
    });

    newChar->GetMotionMaster()->Initialize();
    if (!newChar->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), &createInfo))
    {
        TC_LOG_WARN("server.worldbots", "Failed to create generated WorldBot character '{}' from profile '{}'.", name, profile.Id);
        return false;
    }

    // @tswow-begin
    FIRE(Player,OnCreateEarly,TSPlayer(newChar.get()));
    // @tswow-end

    uint32 const maxLevel = uint32(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
    uint8 const requestedLevel = uint8(std::max<uint32>(1, std::min<uint32>(profile.Level, maxLevel)));
    if (requestedLevel != newChar->GetLevel())
        newChar->GiveLevel(requestedLevel);

    for (uint32 spellId : profile.SpellIds)
    {
        if (sSpellMgr->GetSpellInfo(spellId))
            newChar->LearnSpell(spellId, false, 0, false);
        else
            TC_LOG_WARN("server.worldbots", "Generated WorldBot {} profile '{}' references unknown spell {}.", name, profile.Id, spellId);
    }

    for (uint32 itemId : profile.GearItemIds)
        if (!newChar->StoreNewItemInBestSlots(itemId, 1))
            TC_LOG_WARN("server.worldbots", "Generated WorldBot {} profile '{}' could not equip/store item {}.", name, profile.Id, itemId);

    newChar->SetFullHealth();
    newChar->SetFullPower(POWER_MANA);
    newChar->SetAtLoginFlag(AT_LOGIN_FIRST);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    newChar->SaveToDB(trans, true);
    CharacterDatabase.CommitTransaction(trans);

    sCharacterCache->AddCharacterCacheEntry(newChar->GetGUID(), _config.GeneratedAccountId, newChar->GetName(), newChar->GetNativeGender(),
        newChar->GetRace(), newChar->GetClass(), newChar->GetLevel());
    sWorld->UpdateRealmCharCount(_config.GeneratedAccountId);
    sScriptMgr->OnPlayerCreate(newChar.get());

    TC_LOG_INFO("server.worldbots", "Created generated WorldBot {} guid={} profile='{}' account={} level={} spells={} gear={} questPlan={} despawnOnPlanComplete={}.",
        newChar->GetName(), newChar->GetGUID().ToString(), profile.Id, _config.GeneratedAccountId, newChar->GetLevel(), profile.SpellIds.size(),
        profile.GearItemIds.size(), profile.QuestPlanIds.size(), profile.DespawnOnQuestPlanComplete);
    return true;
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

bool WorldBotMgr::UpdateDebugBotDeath(Map* map, uint32 diff)
{
    if (!_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!_config.DebugDeathHandling)
    {
        _debugDeathReleaseTimer = 0;
        _debugDeathRespawnTimer = 0;
        _debugDeathReleased = false;
        return false;
    }

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    if (bot->IsAlive())
    {
        _debugDeathReleaseTimer = 0;
        _debugDeathRespawnTimer = 0;
        _debugDeathReleased = false;
        return false;
    }

    bot->AttackStop();
    bot->CombatStop(true);
    _debugRecovering = false;
    _debugLootTargetGuid.Clear();
    _debugLootMoveTimer = 0;
    _debugLootTargetTimer = 0;
    _debugQuestTargetGuid.Clear();
    _debugQuestMoveTimer = 0;
    _debugQuestTargetTimer = 0;
    _debugQuestTravelTimer = 0;
    _debugSpellCastTimer = 0;

    if (bot->getDeathState() == JUST_DIED)
    {
        bot->KillPlayer();
        return true;
    }

    if (bot->getDeathState() == CORPSE)
    {
        if (!_debugDeathReleaseTimer)
        {
            _debugDeathReleaseTimer = _config.DebugDeathReleaseDelayMs;
            if (_config.Debug)
                TC_LOG_INFO("server.worldbots", "WorldBot {} died. Releasing in {} ms.", bot->GetName(), _debugDeathReleaseTimer);
        }

        if (_debugDeathReleaseTimer > diff)
        {
            _debugDeathReleaseTimer -= diff;
            return true;
        }

        bot->BuildPlayerRepop();
        bot->RepopAtGraveyard();
        _debugDeathReleaseTimer = 0;
        _debugDeathRespawnTimer = _config.DebugDeathRespawnDelayMs;
        _debugDeathReleased = true;

        if (_config.Debug)
            TC_LOG_INFO("server.worldbots", "WorldBot {} released to graveyard. Corpse teleport in {} ms.", bot->GetName(), _debugDeathRespawnTimer);

        return true;
    }

    if (bot->getDeathState() == DEAD)
    {
        if (!_debugDeathReleased)
        {
            _debugDeathReleased = true;
            _debugDeathRespawnTimer = _config.DebugDeathRespawnDelayMs;
        }

        if (_debugDeathRespawnTimer > diff)
        {
            _debugDeathRespawnTimer -= diff;
            return true;
        }

        if (bot->IsBeingTeleportedFar())
            return true;

        if (FinishWorldBotNearTeleport(bot) && _config.Debug)
            TC_LOG_INFO("server.worldbots", "WorldBot {} completed corpse near-teleport server-side.", bot->GetName());

        if (bot->HasCorpse())
        {
            WorldLocation const& corpseLocation = bot->GetCorpseLocation();
            if (bot->GetMapId() != corpseLocation.GetMapId() || !bot->IsWithinDist3d(&corpseLocation, CORPSE_RECLAIM_RADIUS))
            {
                bot->TeleportTo(corpseLocation.GetMapId(), corpseLocation.GetPositionX(), corpseLocation.GetPositionY(),
                    corpseLocation.GetPositionZ(), corpseLocation.GetOrientation());

                if (_config.Debug)
                    TC_LOG_INFO("server.worldbots", "WorldBot {} teleporting ghost to corpse at map={} x={} y={} z={}.", bot->GetName(),
                        corpseLocation.GetMapId(), corpseLocation.GetPositionX(), corpseLocation.GetPositionY(), corpseLocation.GetPositionZ());

                return true;
            }
        }
        else if (_config.Debug)
            TC_LOG_INFO("server.worldbots", "WorldBot {} has no corpse location; resurrecting at current ghost position.", bot->GetName());

        bot->ResurrectPlayer(0.5f, false);
        bot->SpawnCorpseBones();
        if (!bot->IsStandState())
            bot->SetStandState(UNIT_STAND_STATE_STAND);

        _debugDeathReleaseTimer = 0;
        _debugDeathRespawnTimer = 0;
        _debugDeathReleased = false;

        if (_config.Debug)
            TC_LOG_INFO("server.worldbots", "WorldBot {} respawned at corpse without death penalty healthPct={} manaPct={}.", bot->GetName(),
                bot->GetHealthPct(), bot->GetMaxPower(POWER_MANA) ? bot->GetPowerPct(POWER_MANA) : 100.0f);

        return true;
    }

    return true;
}

bool WorldBotMgr::UpdateDebugBotRecovery(Map* map, uint32 diff)
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
            _debugConsumableScanTimer = 0;
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
        _debugConsumableScanTimer = 0;
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
        _debugConsumableScanTimer = 0;
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
        _debugConsumableScanTimer = 0;
        if (_config.Debug)
            TC_LOG_DEBUG("server.worldbots", "WorldBot {} entering recovery healthPct={} manaPct={}.", bot->GetName(),
                bot->GetHealthPct(), hasActiveMana ? bot->GetPowerPct(POWER_MANA) : 100.0f);
    }

    if (bot->IsMounted())
        bot->Dismount();

    if (!bot->IsStopped())
        bot->StopMoving();

    UpdateDebugBotConsumables(bot, needsHealth, needsMana, diff);

    if (bot->GetStandState() != UNIT_STAND_STATE_SIT)
        bot->SetStandState(UNIT_STAND_STATE_SIT);

    return true;
}

bool WorldBotMgr::UpdateDebugBotConsumables(Player* bot, bool needsHealth, bool needsMana, uint32 diff)
{
    if (!_config.DebugConsumables || !bot || bot->IsInCombat() || bot->GetVictim())
        return false;

    bool const needsFood = needsHealth && !bot->HasAuraType(SPELL_AURA_MOD_REGEN) && !bot->HasAuraType(SPELL_AURA_MOD_REGEN_DURING_COMBAT);
    bool const needsDrink = needsMana && !bot->HasAuraType(SPELL_AURA_MOD_POWER_REGEN) && !bot->HasAuraType(SPELL_AURA_PERIODIC_ENERGIZE);
    if (!needsFood && !needsDrink)
        return false;

    if (_debugConsumableScanTimer > diff)
    {
        _debugConsumableScanTimer -= diff;
        return false;
    }

    _debugConsumableScanTimer = _config.DebugConsumableScanIntervalMs;

    auto tryUseConsumable = [&](Item* item) -> bool
    {
        WorldBotConsumableCandidate candidate = GetWorldBotConsumableCandidate(item, needsFood, needsDrink);
        if (!candidate)
            return false;

        InventoryResult result = bot->CanUseItem(candidate.Consumable);
        if (result != EQUIP_ERR_OK)
        {
            if (_config.Debug)
                TC_LOG_INFO("server.worldbots", "WorldBot {} cannot use recovery consumable {}: result={}.", bot->GetName(),
                    candidate.Consumable->GetEntry(), uint32(result));

            return false;
        }

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(candidate.SpellId);
        SpellCastTargets targets;
        if (spellInfo && spellInfo->NeedsExplicitUnitTarget())
            targets.SetUnitTarget(bot);

        uint32 const itemEntry = candidate.Consumable->GetEntry();
        bot->CastItemUseSpell(candidate.Consumable, targets, 0, 0);

        if (bot->GetStandState() != UNIT_STAND_STATE_SIT)
            bot->SetStandState(UNIT_STAND_STATE_SIT);

        if (_config.Debug)
            TC_LOG_INFO("server.worldbots", "WorldBot {} using recovery consumable {} spell={} restoresHealth={} restoresMana={} healthPct={} manaPct={}.",
                bot->GetName(), itemEntry, candidate.SpellId, candidate.RestoresHealth, candidate.RestoresMana, bot->GetHealthPct(),
                bot->GetMaxPower(POWER_MANA) ? bot->GetPowerPct(POWER_MANA) : 100.0f);

        return true;
    };

    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        if (tryUseConsumable(bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot)))
            return true;

    for (uint8 bagSlot = INVENTORY_SLOT_BAG_START; bagSlot < INVENTORY_SLOT_BAG_END; ++bagSlot)
    {
        Bag* bag = bot->GetBagByPos(bagSlot);
        if (!bag)
            continue;

        for (uint32 slot = 0; slot < bag->GetBagSize(); ++slot)
            if (tryUseConsumable(bag->GetItemByPos(uint8(slot))))
                return true;
    }

    if (_config.Debug)
        TC_LOG_INFO("server.worldbots", "WorldBot {} found no recovery consumable needsFood={} needsDrink={}.", bot->GetName(), needsFood, needsDrink);

    return false;
}

bool WorldBotMgr::UpdateDebugBotQuestPlanCompletion(Map* map)
{
    if (!_config.DebugDespawnOnQuestPlanComplete || !_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!IsDebugBotQuestPlanComplete(bot))
        return false;

    TC_LOG_INFO("server.worldbots", "WorldBot {} completed quest plan. Despawning debug bot.", bot->GetName());

    _debugSession->LogoutPlayer(true);
    _debugSession.reset();
    _activeBotCount = 0;
    _debugLootTargetGuid.Clear();
    _debugQuestTargetGuid.Clear();
    _debugLootBlacklist.clear();

    return true;
}

bool WorldBotMgr::UpdateDebugBotQuesting(Map* map, uint32 diff)
{
    if (!_debugSession)
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!_config.DebugQuesting)
    {
        _debugQuestTargetGuid.Clear();
        _debugQuestMoveTimer = 0;
        _debugQuestTargetTimer = 0;
        _debugQuestTravelTimer = 0;
        return false;
    }

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsInCombat() || bot->GetVictim() || bot->IsBeingTeleported())
        return false;

    if (_debugQuestTargetGuid)
    {
        if (_debugQuestTargetTimer <= diff)
        {
            if (_config.Debug)
                TC_LOG_DEBUG("server.worldbots", "WorldBot {} abandoned questgiver target {} after timeout.", bot->GetName(),
                    _debugQuestTargetGuid.ToString());

            _debugQuestTargetGuid.Clear();
            _debugQuestMoveTimer = 0;
            _debugQuestTargetTimer = 0;
            return false;
        }

        _debugQuestTargetTimer -= diff;

        Creature* creature = map->GetCreature(_debugQuestTargetGuid);
        if (!IsDebugBotQuestGiver(bot, creature))
        {
            _debugQuestTargetGuid.Clear();
            _debugQuestMoveTimer = 0;
            _debugQuestTargetTimer = 0;
            return false;
        }

        if (!bot->CanInteractWithQuestGiver(creature))
        {
            if (bot->IsMounted())
                bot->Dismount();

            if (_debugQuestMoveTimer > diff && !bot->IsStopped())
            {
                _debugQuestMoveTimer -= diff;
                return true;
            }

            _debugQuestMoveTimer = 1000;
            bot->GetMotionMaster()->MovePoint(_debugMovePointId++, creature->GetPosition(), true);
            return true;
        }

        if (!bot->IsStopped())
            bot->StopMoving();

        _debugQuestTargetGuid.Clear();
        _debugQuestMoveTimer = 0;
        _debugQuestTargetTimer = 0;
        return UseDebugBotQuestGiver(bot, creature);
    }

    if (_debugQuestScanTimer > diff)
    {
        _debugQuestScanTimer -= diff;
        return false;
    }

    _debugQuestScanTimer = _config.DebugQuestScanIntervalMs;

    Creature* questGiver = SelectDebugBotQuestGiver(bot);
    if (!questGiver)
        return false;

    if (bot->CanInteractWithQuestGiver(questGiver))
        return UseDebugBotQuestGiver(bot, questGiver);

    _debugQuestTargetGuid = questGiver->GetGUID();
    _debugQuestMoveTimer = 0;
    _debugQuestTargetTimer = _config.DebugQuestInteractTimeoutMs;

    if (_config.Debug)
        TC_LOG_DEBUG("server.worldbots", "WorldBot {} moving to questgiver {} entry={} distance={}.", bot->GetName(),
            questGiver->GetGUID().ToString(), questGiver->GetEntry(), bot->GetDistance(questGiver));

    return true;
}

bool WorldBotMgr::UpdateDebugBotQuestTravel(Map* map, uint32 diff)
{
    if (!_debugSession || !_config.DebugQuesting || _config.DebugQuestPlanIds.empty())
        return false;

    Player* bot = _debugSession->GetPlayer();
    if (!bot || bot->GetMap() != map)
        return false;

    if (!map || !map->GetEntry() || !map->GetEntry()->IsContinent())
        return false;

    if (!bot->IsInWorld() || !bot->IsAlive() || bot->IsInCombat() || bot->GetVictim() || bot->IsBeingTeleported())
        return false;

    if (HasDebugBotActivePlannedKillObjective(bot))
    {
        float const liveTargetRange = std::max(_config.DebugQuestSearchRange, _config.DebugCombatSearchRange * 2.0f);
        if (Creature* killTarget = SelectDebugBotPlannedQuestKillTarget(bot, liveTargetRange))
        {
            float const distance = bot->GetDistance(killTarget);
            if (distance <= _config.DebugCombatSearchRange)
            {
                if (bot->IsMounted())
                    bot->Dismount();

                if (bot->Attack(killTarget, true))
                {
                    bot->GetMotionMaster()->MoveChase(killTarget);

                    if (_config.Debug)
                        TC_LOG_INFO("server.worldbots", "WorldBot {} engaging live planned quest target entry={} guid={} distance={} questScore={}.",
                            bot->GetName(), killTarget->GetEntry(), killTarget->GetGUID().ToString(), distance,
                            GetDebugBotPlannedQuestKillScore(bot, killTarget));

                    return true;
                }

                if (_config.Debug)
                    TC_LOG_WARN("server.worldbots", "WorldBot {} found live planned quest target entry={} guid={} distance={} but Attack() failed validAttack={} targetable={} los={}.",
                        bot->GetName(), killTarget->GetEntry(), killTarget->GetGUID().ToString(), distance, bot->IsValidAttackTarget(killTarget),
                        killTarget->isTargetableForAttack(false), bot->IsWithinLOSInMap(killTarget));
            }

            if (distance > std::max(5.0f, _config.DebugCombatSearchRange * 0.5f))
            {
                if (bot->IsMounted())
                    bot->Dismount();

                if (_debugQuestTravelTimer > diff && !bot->IsStopped())
                {
                    _debugQuestTravelTimer -= diff;
                    return true;
                }

                _debugQuestTravelTimer = 1000;
                bot->GetMotionMaster()->MovePoint(_debugMovePointId++, killTarget->GetPosition(), true);

                if (_config.Debug)
                    TC_LOG_INFO("server.worldbots", "WorldBot {} moving to live planned quest target entry={} guid={} distance={}.",
                        bot->GetName(), killTarget->GetEntry(), killTarget->GetGUID().ToString(), distance);

                return true;
            }
        }
    }

    uint32 targetMapId = MAPID_INVALID;
    Position destination;
    float arrivalDistance = 0.0f;
    uint32 questId = 0;
    uint32 targetEntry = 0;
    char const* action = nullptr;
    if (!SelectDebugBotQuestTravelDestination(bot, targetMapId, destination, arrivalDistance, questId, targetEntry, action))
        return false;

    if (targetMapId != bot->GetMapId())
    {
        if (_debugQuestTravelTimer > diff)
            _debugQuestTravelTimer -= diff;
        else
            _debugQuestTravelTimer = _config.DebugQuestScanIntervalMs;

        if (_config.Debug && _debugQuestTravelTimer == _config.DebugQuestScanIntervalMs)
            TC_LOG_INFO("server.worldbots", "WorldBot {} knows quest target but cannot path cross-map yet: quest={} action={} entry={} currentMap={} targetMap={}.",
                bot->GetName(), questId, action ? action : "unknown", targetEntry, bot->GetMapId(), targetMapId);

        return true;
    }

    float const distance = bot->GetDistance(destination.GetPositionX(), destination.GetPositionY(), destination.GetPositionZ());
    if (distance <= arrivalDistance)
    {
        if (_debugQuestTravelTimer > diff)
            _debugQuestTravelTimer -= diff;
        else if (_config.Debug)
        {
            _debugQuestTravelTimer = 3000;
            float const liveTargetRange = std::max(_config.DebugQuestSearchRange, _config.DebugCombatSearchRange * 2.0f);
            Creature* liveTarget = action && std::string(action) == "kill" ? SelectDebugBotPlannedQuestKillTarget(bot, liveTargetRange) : nullptr;
            uint32 matchingEntryCount = 0;
            uint32 matchingAliveCount = 0;
            uint32 matchingValidAttackCount = 0;
            uint32 matchingQuestScoreCount = 0;

            if (action && std::string(action) == "kill" && targetEntry)
            {
                std::list<Creature*> nearbyCreatures;
                Trinity::AllCreaturesOfEntryInRange check(bot, targetEntry, liveTargetRange);
                Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(bot, nearbyCreatures, check);
                Cell::VisitAllObjects(bot, searcher, liveTargetRange);

                for (Creature* creature : nearbyCreatures)
                {
                    ++matchingEntryCount;
                    if (creature->IsAlive())
                        ++matchingAliveCount;
                    if (creature->IsAlive() && creature->isTargetableForAttack(false) && bot->IsValidAttackTarget(creature) &&
                        bot->CanSeeOrDetect(creature))
                        ++matchingValidAttackCount;
                    if (GetDebugBotPlannedQuestKillScore(bot, creature))
                        ++matchingQuestScoreCount;
                }
            }

            TC_LOG_INFO("server.worldbots", "WorldBot {} arrived at quest point quest={} action={} entry={} map={} distance={} arrivalDistance={} activeKill={} liveTarget={} liveTargetDistance={} matchingEntry={} matchingAlive={} matchingValidAttack={} matchingQuestScore={} combatScanTimer={} attackers={}.",
                bot->GetName(), questId, action ? action : "unknown", targetEntry, targetMapId, distance, arrivalDistance,
                HasDebugBotActivePlannedKillObjective(bot), liveTarget ? liveTarget->GetGUID().ToString() : "none",
                liveTarget ? bot->GetDistance(liveTarget) : 0.0f, matchingEntryCount, matchingAliveCount, matchingValidAttackCount,
                matchingQuestScoreCount, _debugCombatScanTimer, bot->getAttackers().size());
        }

        return false;
    }

    if (bot->IsMounted())
        bot->Dismount();

    if (_debugQuestTravelTimer > diff && !bot->IsStopped())
    {
        _debugQuestTravelTimer -= diff;
        return true;
    }

    _debugQuestTravelTimer = 2000;
    bot->GetMotionMaster()->MovePoint(_debugMovePointId++, destination, true);

    if (_config.Debug)
        TC_LOG_INFO("server.worldbots", "WorldBot {} traveling for quest={} action={} entry={} map={} x={} y={} z={} distance={} arrivalDistance={}.",
            bot->GetName(), questId, action ? action : "unknown", targetEntry, targetMapId, destination.GetPositionX(), destination.GetPositionY(),
            destination.GetPositionZ(), distance, arrivalDistance);

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

Creature* WorldBotMgr::SelectDebugBotQuestGiver(Player* bot) const
{
    std::list<Creature*> creatures;
    Trinity::AllFriendlyCreaturesInGrid check(bot);
    Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(bot, creatures, check);
    Cell::VisitAllObjects(bot, searcher, _config.DebugQuestSearchRange);

    Creature* bestQuestGiver = nullptr;
    float bestDistance = _config.DebugQuestSearchRange;

    for (Creature* creature : creatures)
    {
        if (!IsDebugBotQuestGiver(bot, creature))
            continue;

        float distance = bot->GetDistance(creature);
        if (distance > bestDistance)
            continue;

        bestDistance = distance;
        bestQuestGiver = creature;
    }

    return bestQuestGiver;
}

bool WorldBotMgr::IsDebugBotQuestGiver(Player* bot, Creature* creature) const
{
    if (_config.DebugQuestPlanIds.empty())
        return false;

    if (!bot || !creature || creature->GetMap() != bot->GetMap() || !creature->IsAlive())
        return false;

    if (!creature->HasNpcFlag(UNIT_NPC_FLAG_QUESTGIVER) || creature->IsHostileTo(bot))
        return false;

    if (!creature->IsWithinDistInMap(bot, _config.DebugQuestSearchRange) || !bot->CanSeeOrDetect(creature) || !bot->IsWithinLOSInMap(creature))
        return false;

    QuestRelationResult involvedQuests = sObjectMgr->GetCreatureQuestInvolvedRelations(creature->GetEntry());
    for (uint32 questId : involvedQuests)
    {
        if (!IsDebugBotPlannedQuest(questId))
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE && bot->CanRewardQuest(quest, false))
            return true;
    }

    QuestRelationResult startQuests = sObjectMgr->GetCreatureQuestRelations(creature->GetEntry());
    for (uint32 questId : startQuests)
    {
        if (!IsDebugBotPlannedQuest(questId))
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (bot->GetQuestStatus(questId) == QUEST_STATUS_NONE && bot->CanSeeStartQuest(quest) && bot->CanTakeQuest(quest, false) &&
            bot->CanAddQuest(quest, false))
            return true;
    }

    return false;
}

bool WorldBotMgr::UseDebugBotQuestGiver(Player* bot, Creature* creature)
{
    if (!IsDebugBotQuestGiver(bot, creature) || !bot->CanInteractWithQuestGiver(creature))
        return false;

    QuestRelationResult involvedQuests = sObjectMgr->GetCreatureQuestInvolvedRelations(creature->GetEntry());
    for (uint32 questId : involvedQuests)
        if (TurnInDebugBotQuest(bot, creature, questId))
            return true;

    QuestRelationResult startQuests = sObjectMgr->GetCreatureQuestRelations(creature->GetEntry());
    for (uint32 questId : startQuests)
        if (AcceptDebugBotQuest(bot, creature, questId))
            return true;

    return false;
}

bool WorldBotMgr::TurnInDebugBotQuest(Player* bot, Creature* creature, uint32 questId)
{
    if (!IsDebugBotPlannedQuest(questId))
        return false;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest || bot->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE || !bot->CanRewardQuest(quest, false))
        return false;

    uint32 reward = 0;
    if (quest->GetRewChoiceItemsCount() > 0)
    {
        bool foundReward = false;
        for (uint32 rewardIndex = 0; rewardIndex < QUEST_REWARD_CHOICES_COUNT; ++rewardIndex)
        {
            if (!quest->RewardChoiceItemId[rewardIndex])
                continue;

            if (!bot->CanRewardQuest(quest, rewardIndex, false))
                continue;

            reward = rewardIndex;
            foundReward = true;
            break;
        }

        if (!foundReward)
            return false;
    }
    else if (!bot->CanRewardQuest(quest, reward, false))
        return false;

    bot->RewardQuest(quest, reward, creature);
    bot->PlayerTalkClass->ClearMenus();
    creature->AI()->OnQuestReward(bot, quest, reward);

    if (_config.Debug)
        TC_LOG_INFO("server.worldbots", "WorldBot {} turned in quest {} ({}) to creature {} reward={}.", bot->GetName(),
            quest->GetQuestId(), quest->GetTitle(), creature->GetEntry(), reward);

    return true;
}

bool WorldBotMgr::AcceptDebugBotQuest(Player* bot, Creature* creature, uint32 questId)
{
    if (!IsDebugBotPlannedQuest(questId))
        return false;

    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest || bot->GetQuestStatus(questId) != QUEST_STATUS_NONE)
        return false;

    if (!bot->CanSeeStartQuest(quest) || !bot->CanTakeQuest(quest, false) || !bot->CanAddQuest(quest, false))
        return false;

    bot->AddQuestAndCheckCompletion(quest, creature);

    if (_config.Debug)
        TC_LOG_INFO("server.worldbots", "WorldBot {} accepted quest {} ({}) from creature {}.", bot->GetName(),
            quest->GetQuestId(), quest->GetTitle(), creature->GetEntry());

    return true;
}

bool WorldBotMgr::IsDebugBotPlannedQuest(uint32 questId) const
{
    return std::find(_config.DebugQuestPlanIds.begin(), _config.DebugQuestPlanIds.end(), questId) != _config.DebugQuestPlanIds.end();
}

bool WorldBotMgr::IsDebugBotQuestPlanComplete(Player* bot) const
{
    if (!bot || _config.DebugQuestPlanIds.empty())
        return false;

    for (uint32 questId : _config.DebugQuestPlanIds)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return false;

        if (!bot->GetQuestRewardStatus(questId) && bot->GetQuestStatus(questId) != QUEST_STATUS_REWARDED)
            return false;
    }

    return true;
}

bool WorldBotMgr::HasDebugBotActivePlannedKillObjective(Player* bot) const
{
    if (!_config.DebugQuesting || _config.DebugQuestPlanIds.empty() || !bot)
        return false;

    for (auto const& questStatusPair : bot->getQuestStatusMap())
    {
        uint32 questId = questStatusPair.first;
        if (!IsDebugBotPlannedQuest(questId))
            continue;

        QuestStatusData const& questStatus = questStatusPair.second;
        if (questStatus.Status != QUEST_STATUS_INCOMPLETE)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        for (uint8 objectiveIndex = 0; objectiveIndex < QUEST_OBJECTIVES_COUNT; ++objectiveIndex)
        {
            if (quest->RequiredNpcOrGo[objectiveIndex] <= 0)
                continue;

            uint32 requiredCount = quest->RequiredNpcOrGoCount[objectiveIndex];
            if (requiredCount && questStatus.CreatureOrGOCount[objectiveIndex] < requiredCount)
                return true;
        }
    }

    return false;
}

uint32 WorldBotMgr::GetDebugBotPlannedQuestKillScore(Player* bot, Creature* creature) const
{
    if (!_config.DebugQuesting || _config.DebugQuestPlanIds.empty() || !bot || !creature)
        return 0;

    uint32 score = 0;
    for (auto const& questStatusPair : bot->getQuestStatusMap())
    {
        uint32 questId = questStatusPair.first;
        if (!IsDebugBotPlannedQuest(questId))
            continue;

        QuestStatusData const& questStatus = questStatusPair.second;
        if (questStatus.Status != QUEST_STATUS_INCOMPLETE)
            continue;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        for (uint8 objectiveIndex = 0; objectiveIndex < QUEST_OBJECTIVES_COUNT; ++objectiveIndex)
        {
            if (quest->RequiredNpcOrGo[objectiveIndex] <= 0)
                continue;

            if (uint32(quest->RequiredNpcOrGo[objectiveIndex]) != creature->GetEntry())
                continue;

            uint32 requiredCount = quest->RequiredNpcOrGoCount[objectiveIndex];
            if (!requiredCount || questStatus.CreatureOrGOCount[objectiveIndex] >= requiredCount)
                continue;

            score = std::max(score, 1000u + requiredCount - questStatus.CreatureOrGOCount[objectiveIndex]);
        }
    }

    return score;
}

Creature* WorldBotMgr::SelectDebugBotPlannedQuestKillTarget(Player* bot, float range) const
{
    if (!bot || !_config.DebugQuesting || _config.DebugQuestPlanIds.empty())
        return nullptr;

    std::list<Creature*> targets;
    Trinity::AllCreaturesOfEntryInRange check(bot, 0, range);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(bot, targets, check);
    Cell::VisitAllObjects(bot, searcher, range);

    Creature* bestTarget = nullptr;
    float bestDistance = range;
    uint32 bestQuestScore = 0;

    for (Creature* creature : targets)
    {
        if (!creature || !creature->IsAlive())
            continue;

        if (!creature->isTargetableForAttack(false) || !bot->IsValidAttackTarget(creature) || !bot->CanSeeOrDetect(creature))
            continue;

        uint32 questScore = GetDebugBotPlannedQuestKillScore(bot, creature);
        if (!questScore)
            continue;

        float distance = bot->GetDistance(creature);
        if (questScore < bestQuestScore)
            continue;

        if (questScore == bestQuestScore && distance > bestDistance)
            continue;

        bestTarget = creature;
        bestDistance = distance;
        bestQuestScore = questScore;
    }

    return bestTarget;
}

bool WorldBotMgr::SelectDebugBotQuestTravelDestination(Player* bot, uint32& mapId, Position& destination, float& arrivalDistance,
    uint32& questId, uint32& targetEntry, char const*& action) const
{
    if (!bot || _config.DebugQuestPlanIds.empty())
        return false;

    for (uint32 plannedQuestId : _config.DebugQuestPlanIds)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(plannedQuestId);
        if (!quest)
            continue;

        QuestStatus status = bot->GetQuestStatus(plannedQuestId);
        if (status == QUEST_STATUS_REWARDED)
            continue;

        if (status == QUEST_STATUS_COMPLETE)
        {
            if (FindNearestDebugBotQuestCreatureSpawn(bot, plannedQuestId, true, mapId, destination, targetEntry))
            {
                questId = plannedQuestId;
                action = "turn-in";
                arrivalDistance = _config.DebugQuestSearchRange * 0.75f;
                return true;
            }

            continue;
        }

        if (status == QUEST_STATUS_NONE)
        {
            if (FindNearestDebugBotQuestCreatureSpawn(bot, plannedQuestId, false, mapId, destination, targetEntry))
            {
                questId = plannedQuestId;
                action = "accept";
                arrivalDistance = _config.DebugQuestSearchRange * 0.75f;
                return true;
            }

            continue;
        }

        if (status != QUEST_STATUS_INCOMPLETE)
            continue;

        auto questStatusItr = bot->getQuestStatusMap().find(plannedQuestId);
        if (questStatusItr == bot->getQuestStatusMap().end())
            continue;

        QuestStatusData const& questStatus = questStatusItr->second;
        for (uint8 objectiveIndex = 0; objectiveIndex < QUEST_OBJECTIVES_COUNT; ++objectiveIndex)
        {
            if (quest->RequiredNpcOrGo[objectiveIndex] <= 0)
                continue;

            uint32 requiredCount = quest->RequiredNpcOrGoCount[objectiveIndex];
            if (!requiredCount || questStatus.CreatureOrGOCount[objectiveIndex] >= requiredCount)
                continue;

            uint32 objectiveEntry = uint32(quest->RequiredNpcOrGo[objectiveIndex]);
            if (FindNearestDebugBotCreatureSpawn(bot, objectiveEntry, mapId, destination))
            {
                questId = plannedQuestId;
                targetEntry = objectiveEntry;
                action = "kill";
                arrivalDistance = std::max(5.0f, _config.DebugCombatSearchRange * 0.65f);
                return true;
            }
        }
    }

    return false;
}

bool WorldBotMgr::FindNearestDebugBotCreatureSpawn(Player* bot, uint32 entry, uint32& mapId, Position& destination) const
{
    if (!bot || !entry)
        return false;

    bool found = false;
    bool foundSameMap = false;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto const& spawnPair : sObjectMgr->GetAllCreatureData())
    {
        CreatureData const& data = spawnPair.second;
        if (data.id != entry)
            continue;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapId);
        if (!mapEntry || !mapEntry->IsContinent())
            continue;

        if (data.phaseMask && bot->GetPhaseMask() && !(data.phaseMask & bot->GetPhaseMask()))
            continue;

        bool const sameMap = data.mapId == bot->GetMapId();
        if (foundSameMap && !sameMap)
            continue;

        float distance = 0.0f;
        if (sameMap)
        {
            float const dx = bot->GetPositionX() - data.spawnPoint.GetPositionX();
            float const dy = bot->GetPositionY() - data.spawnPoint.GetPositionY();
            float const dz = bot->GetPositionZ() - data.spawnPoint.GetPositionZ();
            distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        }
        else
            distance = foundSameMap ? std::numeric_limits<float>::max() : 1000000.0f;

        if (!found || sameMap != foundSameMap || distance < bestDistance)
        {
            found = true;
            foundSameMap = sameMap;
            bestDistance = distance;
            mapId = data.mapId;
            destination = data.spawnPoint;
        }
    }

    return found;
}

bool WorldBotMgr::FindNearestDebugBotQuestCreatureSpawn(Player* bot, uint32 questId, bool involvedRelation, uint32& mapId, Position& destination,
    uint32& targetEntry) const
{
    if (!bot || !questId)
        return false;

    bool found = false;
    bool foundSameMap = false;
    float bestDistance = std::numeric_limits<float>::max();

    for (auto const& spawnPair : sObjectMgr->GetAllCreatureData())
    {
        CreatureData const& data = spawnPair.second;
        QuestRelationResult relations = involvedRelation ? sObjectMgr->GetCreatureQuestInvolvedRelations(data.id) :
            sObjectMgr->GetCreatureQuestRelations(data.id);
        if (!relations.HasQuest(questId))
            continue;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapId);
        if (!mapEntry || !mapEntry->IsContinent())
            continue;

        if (data.phaseMask && bot->GetPhaseMask() && !(data.phaseMask & bot->GetPhaseMask()))
            continue;

        bool const sameMap = data.mapId == bot->GetMapId();
        if (foundSameMap && !sameMap)
            continue;

        float distance = 0.0f;
        if (sameMap)
        {
            float const dx = bot->GetPositionX() - data.spawnPoint.GetPositionX();
            float const dy = bot->GetPositionY() - data.spawnPoint.GetPositionY();
            float const dz = bot->GetPositionZ() - data.spawnPoint.GetPositionZ();
            distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        }
        else
            distance = foundSameMap ? std::numeric_limits<float>::max() : 1000000.0f;

        if (!found || sameMap != foundSameMap || distance < bestDistance)
        {
            found = true;
            foundSameMap = sameMap;
            bestDistance = distance;
            mapId = data.mapId;
            destination = data.spawnPoint;
            targetEntry = data.id;
        }
    }

    return found;
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
    uint32 bestQuestScore = 0;
    bool const strictQuestKillTarget = HasDebugBotActivePlannedKillObjective(bot);

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
        uint32 questScore = GetDebugBotPlannedQuestKillScore(bot, creature);
        if (strictQuestKillTarget && !questScore)
            continue;

        if (questScore || bestQuestScore)
        {
            if (questScore < bestQuestScore)
                continue;

            if (questScore == bestQuestScore && distance > bestDistance)
                continue;
        }
        else if (distance > bestDistance)
            continue;

        bestDistance = distance;
        bestQuestScore = questScore;
        bestTarget = creature;
    }

    return bestTarget;
}
