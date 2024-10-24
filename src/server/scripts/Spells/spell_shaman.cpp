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

/*
 * Scripts for spells with SPELLFAMILY_SHAMAN and SPELLFAMILY_GENERIC spells used by shaman players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_sha_".
 */

#include "ScriptMgr.h"
#include "GridNotifiersImpl.h"
#include "Item.h"
#include "CellImpl.h"
#include "ObjectAccessor.h"
#include "Map.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellHistory.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"

enum ShamanSpells
{
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY      = 52759,
    SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC       = 52752,
    SPELL_SHAMAN_BIND_SIGHT                     = 6277,
    SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT         = 1230070,
    SPELL_SHAMAN_EARTH_SHIELD_HEAL              = 379,
    SPELL_SHAMAN_ELEMENTAL_MASTERY              = 16166,
    SPELL_SHAMAN_ELEMENTAL_OATH                 = 51466,
    SPELL_SHAMAN_EXHAUSTION                     = 57723,
    SPELL_SHAMAN_FIRE_NOVA_R1                   = 1535,
    SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1         = 8349,
    SPELL_SHAMAN_GLYPH_OF_EARTH_SHIELD          = 63279,
    SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM  = 55456,
    SPELL_SHAMAN_GLYPH_OF_MANA_TIDE             = 55441,
    SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM          = 62132,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD          = 23552,
    SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE   = 27635,
    SPELL_SHAMAN_ITEM_MANA_SURGE                = 23571,
    SPELL_SHAMAN_LAVA_FLOWS_R1                  = 51480,
    SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1        = 64694,
    SPELL_SHAMAN_LIGHTNING_SHIELD_R1            = 26364,
    SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE     = 52032,
    SPELL_SHAMAN_MANA_TIDE_TOTEM                = 39609,
    SPELL_SHAMAN_NATURE_GUARDIAN                = 31616,
    SPELL_SHAMAN_NATURE_GUARDIAN_THREAT         = 39301, // Serverside
    SPELL_SHAMAN_SATED                          = 57724,
    SPELL_SHAMAN_STORM_EARTH_AND_FIRE           = 51483,
    SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB      = 64695,
    SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM          = 6474,
    SPELL_SHAMAN_TOTEM_EARTHEN_POWER            = 59566,
    SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL      = 52042,
    SPELL_SHAMAN_TOTEMIC_MASTERY                = 38437,
    SPELL_SHAMAN_TIDAL_FORCE_CRIT               = 55166,
    SPELL_SHAMAN_TOTEMIC_POWER_MP5              = 28824,
    SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER      = 28825,
    SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER     = 28826,
    SPELL_SHAMAN_TOTEMIC_POWER_ARMOR            = 28827,
    SPELL_SHAMAN_WINDFURY_WEAPON_R1             = 8232,
    SPELL_SHAMAN_WINDFURY_ATTACK_MH             = 25504,
    SPELL_SHAMAN_WINDFURY_ATTACK_OH             = 33750,
    SPELL_SHAMAN_ENERGY_SURGE                   = 40465,
    SPELL_SHAMAN_POWER_SURGE                    = 40466,
    SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL     = 55533,
    SPELL_SHAMAN_SPIRIT_HUNT_HEAL               = 58879,
    SPELL_SHAMAN_ELECTRIFIED                    = 64930,
    SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE        = 71824,
    SPELL_SHAMAN_CHAINED_HEAL                   = 70809,
    SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER     = 63283,
    SPELL_SHAMAN_FREEZE                         = 63685,
    SPELL_SHAMAN_FLAMETONGUE_ATTACK             = 10444,
    SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1     = 45284,
    SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1    = 45297,
    SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1     = 26364,
    SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC          = 30824,
    SPELL_SHAMAN_STONECLAW_TOTEM                = 1230053,
    SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM       = 63298,
    SPELL_SHAMAN_MAELSTROM_POWER                = 70831,
    SPELL_SHAMAN_T10_ENHANCEMENT_4P_BONUS       = 70832,
    SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1    = 51554,

    // Duskhaven
    SPELL_SHAMAN_ACID_RAIN                      = 1270012,
    SPELL_SHAMAN_AIRS_FURY                      = 1260007,
    SPELL_SHAMAN_AIRS_GRACE                     = 1240026,
    SPELL_SHAMAN_ASTRAL_ALIGNMENT               = 1270019,
    SPELL_SHAMAN_CHAIN_LIGHTNING                = 1230019,
    SPELL_SHAMAN_CHAIN_LIGHTNING_PROC           = 1250004,
    SPELL_SHAMAN_EARTHS_STOICISM                = 1240028,
    SPELL_SHAMAN_EEP_FIRST                      = 1270021, // Man, those three are mouthful
    SPELL_SHAMAN_EEP_SECOND                     = 1270022, // first time abbreviating talent name
    SPELL_SHAMAN_EEP_TALENT                     = 1270020, // lmao
    SPELL_SHAMAN_ELEMENTAL_BASH_DUMMY           = 1240029,
    SPELL_SHAMAN_ELEMENTAL_BASH_REVERBERATION   = 1240030,
    SPELL_SHAMAN_ELEMENTAL_PROWESS              = 1250029,
    SPELL_SHAMAN_FERAL_INSTINCTS_PROC           = 1260014,
    SPELL_SHAMAN_FERAL_INSTINCTS_TALENT         = 1260013,
    SPELL_SHAMAN_FIRE_NOVA_PROC                 = 1260031,
    SPELL_SHAMAN_FLAME_SHOCK                    = 1230029,
    SPELL_SHAMAN_FLAMETONGUE_AURA               = 1230008,
    SPELL_SHAMAN_FLOW_OF_THE_TIDES              = 1270034,
    SPELL_SHAMAN_FROSTBRAND_SLOWED              = 1230016,
    SPELL_SHAMAN_GREATER_HEALING_WAVE_COUNTER   = 1230016,
    SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY     = 1270017,
    SPELL_SHAMAN_GREATER_HEALING_WAVE_HEAL      = 1270018,
    SPELL_SHAMAN_GRIT                           = 1240011,
    SPELL_SHAMAN_ICEFURY                        = 1250032,
    SPELL_SHAMAN_ICEFURY_PROC                   = 1250002,
    SPELL_SHAMAN_LAVA_BURST                     = 1230017,
    SPELL_SHAMAN_LAVA_BURST_PROC                = 1250003,
    SPELL_SHAMAN_LAVA_POOL_DEMORALIZE           = 1230023,
    SPELL_SHAMAN_LIGHTNING_BOLT                 = 1230002,
    SPELL_SHAMAN_LIGHTNING_BOLT_PROC            = 1250001,
    SPELL_SHAMAN_MASTERY_ELEMENTAL_OVERLOAD     = 1250000,
    SPELL_SHAMAN_MOLTEN_ASSAULT                 = 1260022,
    SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR = 1270002,
    SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR = 1270000,
    SPELL_SHAMAN_RUTHLESSNESS                   = 1260008,
    SPELL_SHAMAN_MAELSTROM_DEFENSE              = 1240031,
    SPELL_SHAMAN_MAELSTROM_DEFENSE_DUMMY        = 1240032,
    SPELL_SHAMAN_OVERFLOWING_SHORES             = 1270013,
    SPELL_SHAMAN_SPIRITUAL_AWAKENING            = 1260003,
    SPELL_SHAMAN_SPIRITUAL_AWAKENING_HEAL       = 1260004,
    SPELL_SHAMAN_TOTEMIC_WRATH                  = 1250013,
    SPELL_SHAMAN_WATERS_CALMING_PRESENCE        = 1230073,
    SPELL_SHAMAN_WINDFURY_PROC                  = 1260006
};

enum ShamanSpellIcons
{
    SHAMAN_ICON_ID_RESTORATIVE_TOTEMS           = 338,
    SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW             = 3087,
    SHAMAN_ICON_ID_TOTEM_OF_WRATH               = 2019
};

enum SpellHelpers
{
    SPELL_MAGE_TEMPORAL_DISPLACEMENT            = 1280021
};

// -51556 - Ancestral Awakening
class spell_sha_ancestral_awakening : public AuraScript
{
    PrepareAuraScript(spell_sha_ancestral_awakening);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(healInfo->GetHeal(), aurEff->GetAmount()));
        eventInfo.GetActor()->CastSpell(nullptr, SPELL_SHAMAN_ANCESTRAL_AWAKENING_DUMMY, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_ancestral_awakening::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 52759 - Ancestral Awakening (Proc)
class spell_sha_ancestral_awakening_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_ancestral_awakening_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() < 2)
            return;

        targets.sort(Trinity::HealthPctOrderPred());

        WorldObject* target = targets.front();
        targets.clear();
        targets.push_back(target);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
        args.AddSpellBP0(GetEffectValue());
        GetCaster()->CastSpell(target, SPELL_SHAMAN_ANCESTRAL_AWAKENING_PROC, args);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_ancestral_awakening_proc::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
        OnEffectHitTarget += SpellEffectFn(spell_sha_ancestral_awakening_proc::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 51474 - Astral Shift
class spell_sha_astral_shift : public AuraScript
{
    PrepareAuraScript(spell_sha_astral_shift);

public:
    spell_sha_astral_shift()
    {
        absorbPct = 0;
    }

private:
    uint32 absorbPct;

    bool Load() override
    {
        absorbPct = GetEffectInfo(EFFECT_0).CalcValue(GetCaster());
        return true;
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
    {
        // Set absorbtion amount to unlimited
        amount = -1;
    }

    void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
    {
        // reduces all damage taken while stun, fear or silence
        if (GetTarget()->HasUnitFlag(UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED) || (GetTarget()->HasUnitFlag(UNIT_FLAG_STUNNED) && GetTarget()->HasAuraWithMechanic(1<<MECHANIC_STUN)))
            absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_astral_shift::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(spell_sha_astral_shift::Absorb, EFFECT_0);
    }
};

// -51474 - Astral Shift aura
class spell_sha_astral_shift_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_astral_shift_aura);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->GetAllEffectsMechanicMask() & ((1 << MECHANIC_SILENCE) | (1 << MECHANIC_STUN) | (1 << MECHANIC_FEAR)))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_astral_shift_aura::CheckProc);
    }
};

// 52179 - Astral Shift
class spell_sha_astral_shift_visual_dummy : public AuraScript
{
    PrepareAuraScript(spell_sha_astral_shift_visual_dummy);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        // Periodic needed to remove visual on stun/fear/silence lost
        if (!GetTarget()->HasUnitFlag(UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED))
            Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_astral_shift_visual_dummy::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 2825 - Bloodlust
class spell_sha_bloodlust : public SpellScript
{
    PrepareSpellScript(spell_sha_bloodlust);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_MAGE_TEMPORAL_DISPLACEMENT,
            SPELL_SHAMAN_EXHAUSTION,
            SPELL_SHAMAN_SATED,
        });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_SATED, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_bloodlust::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);

        AfterHit += SpellHitFn(spell_sha_bloodlust::ApplyDebuff);
    }
};

// 1230000 - Chain Heal
class spell_sha_chain_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_chain_heal);

public:
    spell_sha_chain_heal()
    {
        firstHeal = true;
        riptide = false;
    }

private:
    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster()->HasAura(SPELL_SHAMAN_FLOW_OF_THE_TIDES))
        {
            if (firstHeal)
            {
                // Check if the target has Riptide
                if (AuraEffect* aurEff = GetHitUnit()->GetAuraEffect(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_SHAMAN, 0, 0x4, 0, GetCaster()->GetGUID()))
                {
                    riptide = true;
                    // Consume it
                    GetHitUnit()->RemoveAura(aurEff->GetBase());
                }
                firstHeal = false;
            }
            // Riptide increases the Chain Heal effect by 40%
            if (riptide)
            {
                float mult = 1.f + (sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FLOW_OF_THE_TIDES)->GetEffect(EFFECT_1).CalcValue() / 100);
                SetHitHeal(GetHitHeal() * mult);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_chain_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }

    bool firstHeal;
    bool riptide;
};

// 8171 - Cleansing Totem (Pulse)
class spell_sha_cleansing_totem_pulse : public SpellScript
{
    PrepareSpellScript(spell_sha_cleansing_totem_pulse);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster() && GetHitUnit() && GetOriginalCaster())
        {
            CastSpellExtraArgs args(GetOriginalCaster()->GetGUID());
            args.AddSpellMod(SPELLVALUE_BASE_POINT1, 1);
            GetCaster()->CastSpell(GetHitUnit(), SPELL_SHAMAN_CLEANSING_TOTEM_EFFECT, args);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_cleansing_totem_pulse::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 16246 - Clearcasting
class spell_sha_clearcasting : public AuraScript
{
    PrepareAuraScript(spell_sha_clearcasting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_OATH });
    }

    // Elemental Oath bonus
    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit const* owner = GetUnitOwner();
        if (Aura const* aura = owner->GetAuraOfRankedSpell(SPELL_SHAMAN_ELEMENTAL_OATH, owner->GetGUID()))
            amount = aura->GetSpellInfo()->GetEffect(EFFECT_1).CalcValue();
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sha_clearcasting::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
    }
};

// -974 - Earth Shield
class spell_sha_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_earth_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_EARTH_SHIELD_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        CastSpellExtraArgs args(aurEff);
        args.OriginalCaster = GetCasterGUID();
        args.AddSpellBP0(aurEff->GetAmount());
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_EARTH_SHIELD_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 6474 - Earthbind Totem - Fix Talent: Earthen Power
class spell_sha_earthbind_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_earthbind_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_EARTHBIND_TOTEM, SPELL_SHAMAN_TOTEM_EARTHEN_POWER });
    }

    void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (!GetCaster())
            return;
        if (Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself())
            if (AuraEffect* aur = owner->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, 2289, 0))
                if (roll_chance_i(aur->GetBaseAmount()))
                    GetTarget()->CastSpell(nullptr, SPELL_SHAMAN_TOTEM_EARTHEN_POWER, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;
        Player* owner = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return;
        // Storm, Earth and Fire
        if (AuraEffect* aurEff = owner->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_STORM_EARTH_AND_FIRE, EFFECT_1))
        {
            if (roll_chance_i(aurEff->GetAmount()))
                GetCaster()->CastSpell(GetCaster(), SPELL_SHAMAN_TOTEM_EARTHBIND_EARTHGRAB, false);
        }
    }

    void Register() override
    {
         OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_earthbind_totem::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
         OnEffectApply += AuraEffectApplyFn(spell_sha_earthbind_totem::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class EarthenPowerTargetSelector
{
    public:
        EarthenPowerTargetSelector() { }

        bool operator() (WorldObject* target)
        {
            if (!target->ToUnit())
                return true;

            if (!target->ToUnit()->HasAuraWithMechanic(1 << MECHANIC_SNARE))
                return true;

            return false;
        }
};

// 59566 - Earthen Power
class spell_sha_earthen_power : public SpellScript
{
    PrepareSpellScript(spell_sha_earthen_power);

    void FilterTargets(std::list<WorldObject*>& unitList)
    {
        unitList.remove_if(EarthenPowerTargetSelector());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_earthen_power::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// -51940 - Earthliving Weapon (Passive)
class spell_sha_earthliving_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_earthliving_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        int32 chance = 20;
        Unit* caster = eventInfo.GetActor();
        if (AuraEffect const* aurEff = caster->GetAuraEffectOfRankedSpell(SPELL_SHAMAN_BLESSING_OF_THE_ETERNALS_R1, EFFECT_1, caster->GetGUID()))
            if (eventInfo.GetProcTarget()->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT))
                chance += aurEff->GetAmount();

        return roll_chance_i(chance);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_earthliving_weapon::CheckProc);
    }
};

// -1535 - Fire Nova
class spell_sha_fire_nova : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova);

    bool Validate(SpellInfo const* spellInfo) override
    {
        SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FIRE_NOVA_R1);
        if (!firstRankSpellInfo || !spellInfo->IsRankOf(firstRankSpellInfo))
            return false;

        uint8 rank = spellInfo->GetRank();
        if (!sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1, rank, true))
            return false;
        return true;
    }

    SpellCastResult CheckFireTotem()
    {
        Unit* caster = GetCaster();
        // fire totem
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            if (!caster->IsWithinDistInMap(totem, caster->GetSpellMaxRangeForTarget(totem, GetSpellInfo())))
                return SPELL_FAILED_OUT_OF_RANGE;
            return SPELL_CAST_OK;
        }
        else
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_FIRE_TOTEM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[1]))
        {
            uint8 rank = GetSpellInfo()->GetRank();
            if (totem->IsTotem())
                caster->CastSpell(totem, sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_FIRE_NOVA_TRIGGERED_R1, rank), true);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova::CheckFireTotem);
        OnEffectHitTarget += SpellEffectFn(spell_sha_fire_nova::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -8050 - Flame Shock
class spell_sha_flame_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_flame_shock);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_FLOWS_R1, SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1 });
    }

    void HandleDispel(DispelInfo* /*dispelInfo*/)
    {
        if (Unit* caster = GetCaster())
            // Lava Flows
            if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_SHAMAN, SHAMAN_ICON_ID_SHAMAN_LAVA_FLOW, EFFECT_0))
            {
                if (SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_LAVA_FLOWS_R1))
                    if (!aurEff->GetSpellInfo()->IsRankOf(firstRankSpellInfo))
                        return;

                uint8 rank = aurEff->GetSpellInfo()->GetRank();
                caster->CastSpell(caster, sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LAVA_FLOWS_TRIGGERED_R1, rank), true);
            }
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_sha_flame_shock::HandleDispel);
    }
};

// 1230008 - Flametongue Weapon
class spell_sha_flametongue_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_flametongue_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FLAMETONGUE_ATTACK });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* mh = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
        Item* oh = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        
        if (mh->GetTemplate()->InventoryType != INVTYPE_2HWEAPON || (!mh && !oh))
            return false;

        WeaponAttackType attType;

        if (player->IsTwoHandUsed() || !oh)
            attType = player->GetAttackBySlot(mh->GetSlot()); 
        else
            attType = player->GetAttackBySlot(oh->GetSlot());

        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        /*PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();
        Unit* target = eventInfo.GetProcTarget();
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
            attType = OFF_ATTACK;

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        float basePoints = aurEff->GetSpellEffectInfo().CalcValue();

        // Flametongue max damage is normalized based on a 4.0 speed weapon
        // Tooltip says max damage = BasePoints / 25, so BasePoints / 25 / 4 to get base damage per 1.0s AS
        float attackSpeed = player->GetAttackTime(attType) / 1000.f;
        float fireDamage = basePoints / 100.0f;
        fireDamage *= attackSpeed;

        // clip value between (BasePoints / 77) and (BasePoints / 25) as the tooltip indicates
        RoundToInterval(fireDamage, basePoints / 77.0f, basePoints / 25.0f);

        // Calculate Spell Power scaling
        float spellPowerBonus = player->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
        spellPowerBonus += target->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_DAMAGE_TAKEN, SPELL_SCHOOL_MASK_FIRE);

        // calculate penalty from passive aura as is the one with level
        float const factorMod = player->CalculateSpellpowerCoefficientLevelPenalty(GetSpellInfo());

        float const spCoeff = 0.03811f;
        spellPowerBonus *= spCoeff * attackSpeed * factorMod;

        // All done, now proc damage
        CastSpellExtraArgs args(aurEff);
        args
            .SetCastItem(item)
            .AddSpellBP0(fireDamage + spellPowerBonus);
        player->CastSpell(target, SPELL_SHAMAN_FLAMETONGUE_ATTACK, args);*/
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flametongue_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_flametongue_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -63373 - Frozen Power
class spell_sha_frozen_power : public AuraScript
{
    PrepareAuraScript(spell_sha_frozen_power);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_FREEZE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!roll_chance_i(aurEff->GetAmount()))
            return;

        Unit* caster = eventInfo.GetActor();
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_FREEZE);
        float minDistance(spellInfo->GetEffect(EFFECT_0).CalcValue(caster));

        Unit* target = eventInfo.GetProcTarget();
        if (caster->GetDistance(target) < minDistance)
            return;

        caster->CastSpell(target, SPELL_SHAMAN_FREEZE, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_frozen_power::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 63279 - Glyph of Earth Shield
class spell_sha_glyph_of_earth_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_earth_shield);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* earthShield = eventInfo.GetSpellInfo();
        if (!earthShield)
            return;

        AuraEffect* earthShieldEffect = eventInfo.GetProcTarget()->GetAuraEffect(earthShield->Id, EFFECT_0, eventInfo.GetActor()->GetGUID());
        if (!earthShieldEffect)
            return;

        int32 amount = earthShieldEffect->GetAmount();
        AddPct(amount, aurEff->GetAmount());
        earthShieldEffect->SetAmount(amount);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_earth_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55440 - Glyph of Healing Wave
class spell_sha_glyph_of_healing_wave : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_healing_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        if (caster == eventInfo.GetProcTarget())
            return;

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(healInfo->GetHeal(), aurEff->GetAmount()));
        caster->CastSpell(nullptr, SPELL_SHAMAN_GLYPH_OF_HEALING_WAVE_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_healing_wave::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 63280 - Glyph of Totem of Wrath
class spell_sha_glyph_of_totem_of_wrath : public AuraScript
{
    PrepareAuraScript(spell_sha_glyph_of_totem_of_wrath);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Totem of Wrath shares family flags with other totems
        // filter by spellIcon instead
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || spellInfo->SpellIconID != SHAMAN_ICON_ID_TOTEM_OF_WRATH)
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();

        // Fire totem summon slot
        Creature* totem = ObjectAccessor::GetCreature(*caster, caster->m_SummonSlot[1]);
        if (!totem)
            return;

        SpellInfo const* totemSpell = sSpellMgr->GetSpellInfo(totem->m_spells[0]);
        if (!totemSpell)
            return;

        int32 bp0 = CalculatePct(totemSpell->GetEffect(EFFECT_0).CalcValue(caster), aurEff->GetAmount());
        int32 bp1 = CalculatePct(totemSpell->GetEffect(EFFECT_1).CalcValue(caster), aurEff->GetAmount());
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(bp0);
        args.AddSpellMod(SPELLVALUE_BASE_POINT1, bp1);
        caster->CastSpell(nullptr, SPELL_SHAMAN_TOTEM_OF_WRATH_SPELL_POWER, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_glyph_of_totem_of_wrath::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_glyph_of_totem_of_wrath::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 52041, 52046, 52047, 52048, 52049, 52050, 58759, 58760, 58761 - Healing Stream Totem
class spell_sha_healing_stream_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_stream_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM, SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        int32 damage = GetEffectValue();
        if (Unit* target = GetHitUnit())
        {
            Unit* caster = GetCaster();
            ObjectGuid originalCasterGuid = caster->GetGUID();

            if (Player* player = caster->GetAffectingPlayer())
            {
                originalCasterGuid = player->GetGUID();

                // Restorative Totems
                if (AuraEffect const* aurEff = player->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_SHAMAN, SHAMAN_ICON_ID_RESTORATIVE_TOTEMS, EFFECT_1))
                    AddPct(damage, aurEff->GetAmount());

                // Glyph of Healing Stream Totem
                if (AuraEffect const* aurEff = player->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_HEALING_STREAM_TOTEM, EFFECT_0))
                    AddPct(damage, aurEff->GetAmount());
            }

            CastSpellExtraArgs args(originalCasterGuid);
            args.AddSpellBP0(damage);
            caster->CastSpell(target, SPELL_SHAMAN_TOTEM_HEALING_STREAM_HEAL, args);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_stream_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 32182 - Heroism
class spell_sha_heroism : public SpellScript
{
    PrepareSpellScript(spell_sha_heroism);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_MAGE_TEMPORAL_DISPLACEMENT,
            SPELL_SHAMAN_EXHAUSTION,
            SPELL_SHAMAN_SATED
        });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_MAGE_TEMPORAL_DISPLACEMENT));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SHAMAN_EXHAUSTION));
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SHAMAN_SATED));
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_SHAMAN_EXHAUSTION, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_heroism::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);

        AfterHit += SpellHitFn(spell_sha_heroism::ApplyDebuff);
    }
};

// -16180 - Improved Water Shield
class spell_sha_imp_water_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_imp_water_shield);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // If we're here, we've already passed initial aura roll
        // So just chance based on 100%

        // Default chance for Healing Wave and Riptide
        int32 chance = 100;
        // Lesser Healing Wave - 0.6 of default
        if (spellInfo->SpellFamilyFlags[0] & 0x00000080)
            chance = 60;
        // Chain heal - 0.3 of default
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000100)
            chance = 30;

        if (!roll_chance_i(chance))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        // Get Water Shield
        AuraEffect const* waterShield = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x00000000, 0x00000020, 0x00000000, caster->GetGUID());
        if (!waterShield)
            return;

        uint32 spellId = waterShield->GetSpellEffectInfo().TriggerSpell;
        caster->CastSpell(nullptr, spellId, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_imp_water_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_imp_water_shield::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -30675 - Lightning Overload
class spell_sha_lightning_overload : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_overload);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1,
            SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;

        // Lightning Bolt
        if (spellInfo->SpellFamilyFlags[0] & 0x00000001)
            spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_BOLT_OVERLOAD_R1, spellInfo->GetRank());
        // Chain Lightning
        else
        {
            // Chain lightning has [LightOverload_Proc_Chance] / [Max_Number_of_Targets] chance to proc of each individual target hit.
            // A maxed LO would have a 33% / 3 = 11% chance to proc of each target.
            // LO chance was already "accounted" at the proc chance roll, now need to divide the chance by [Max_Number_of_Targets]
            float chance = 100.0f / spellInfo->GetEffect(EFFECT_0).ChainTarget;
            if (!roll_chance_f(chance))
                return;

            spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_CHAIN_LIGHTNING_OVERLOAD_R1, spellInfo->GetRank());
        }

        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_lightning_overload::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 23551 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23552 - Lightning Shield T2 Bonus
class spell_sha_item_lightning_shield_trigger : public AuraScript
{
    PrepareAuraScript(spell_sha_item_lightning_shield_trigger);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_ITEM_LIGHTNING_SHIELD_DAMAGE, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_lightning_shield_trigger::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 23572 - Mana Surge
class spell_sha_item_mana_surge : public AuraScript
{
    PrepareAuraScript(spell_sha_item_mana_surge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ITEM_MANA_SURGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        int32 mana = spellInfo->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask());

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(mana, 35));
        GetTarget()->CastSpell(GetTarget(), SPELL_SHAMAN_ITEM_MANA_SURGE, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_mana_surge::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 40463 - Shaman Tier 6 Trinket
class spell_sha_item_t6_trinket : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t6_trinket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_ENERGY_SURGE,
            SPELL_SHAMAN_POWER_SURGE
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return;

        uint32 spellId;
        int32 chance;

        // Lesser Healing Wave
        if (spellInfo->SpellFamilyFlags[0] & 0x00000080)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 10;
        }
        // Lightning Bolt
        else if (spellInfo->SpellFamilyFlags[0] & 0x00000001)
        {
            spellId = SPELL_SHAMAN_ENERGY_SURGE;
            chance = 15;
        }
        // Stormstrike
        else if (spellInfo->SpellFamilyFlags[1] & 0x00000010)
        {
            spellId = SPELL_SHAMAN_POWER_SURGE;
            chance = 50;
        }
        else
            return;

        if (roll_chance_i(chance))
            eventInfo.GetActor()->CastSpell(nullptr, spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t6_trinket::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70811 - Item - Shaman T10 Elemental 2P Bonus
class spell_sha_item_t10_elemental_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_item_t10_elemental_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELEMENTAL_MASTERY });
    }

    void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        if (Player* target = GetTarget()->ToPlayer())
            target->GetSpellHistory()->ModifyCooldown(SPELL_SHAMAN_ELEMENTAL_MASTERY, -aurEff->GetAmount());
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_item_t10_elemental_2p_bonus::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1260021 - Lava Lash
class spell_sha_lava_lash : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_lash);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            int32 pct = GetEffectValue();
            int32 hitDamage = GetHitDamage();
            if (Item* offhand = caster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
            {
                if (caster->HasAura(SPELL_SHAMAN_FLAMETONGUE_AURA))
                    AddPct(hitDamage, pct);
                SetHitDamage(hitDamage);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_lava_lash::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
    }

};

// -324 - Lightning Shield
class spell_sha_lightning_shield : public AuraScript
{
    PrepareAuraScript(spell_sha_lightning_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD_R1 });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetActionTarget())
            return true;
        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerSpell = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_SHIELD_R1, aurEff->GetSpellInfo()->GetRank());

        eventInfo.GetActionTarget()->CastSpell(eventInfo.GetActor(), triggerSpell, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_lightning_shield::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_lightning_shield::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 1230020 - Maelstrom Weapon
class spell_sha_maelstrom_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_maelstrom_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_MAELSTROM_DEFENSE,
            SPELL_SHAMAN_MAELSTROM_DEFENSE_DUMMY
        });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if ((eventInfo.GetTypeMask() & (PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS))
            && (!eventInfo.GetProcTarget()->HasAura(SPELL_SHAMAN_MAELSTROM_DEFENSE) || eventInfo.GetProcTarget()->HasAura(SPELL_SHAMAN_MAELSTROM_DEFENSE_DUMMY)))
                return false;

        return true;
    }

    void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (Unit* procTarget = eventInfo.GetProcTarget())
            if (procTarget->HasAura(SPELL_SHAMAN_FERAL_INSTINCTS_TALENT))
                procTarget->CastSpell(procTarget, SPELL_SHAMAN_FERAL_INSTINCTS_PROC, true);
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_sha_maelstrom_weapon::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectProc += AuraEffectProcFn(spell_sha_maelstrom_weapon::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 52031, 52033, 52034, 52035, 52036, 58778, 58779, 58780 - Mana Spring Totem
class spell_sha_mana_spring_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_mana_spring_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (Unit* caster = GetCaster())
                if (target->GetPowerType() == POWER_MANA)
                {
                    CastSpellExtraArgs args(GetOriginalCaster()->GetGUID());
                    args.AddSpellBP0(GetEffectValue());
                    caster->CastSpell(target, SPELL_SHAMAN_MANA_SPRING_TOTEM_ENERGIZE, args);
                }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_mana_spring_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

};

// 16191 - Mana Tide
class spell_sha_mana_tide : public AuraScript
{
    PrepareAuraScript(spell_sha_mana_tide);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(aurEff->GetAmount());
        GetTarget()->CastSpell(nullptr, aurEff->GetSpellEffectInfo().TriggerSpell, args);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_mana_tide::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 39610 - Mana Tide Totem
class spell_sha_mana_tide_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_mana_tide_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_GLYPH_OF_MANA_TIDE, SPELL_SHAMAN_MANA_TIDE_TOTEM });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* unitTarget = GetHitUnit())
            {
                if (unitTarget->GetPowerType() == POWER_MANA)
                {
                    int32 effValue = GetEffectValue();
                    // Glyph of Mana Tide
                    if (Unit* owner = caster->GetOwner())
                        if (AuraEffect* dummy = owner->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_MANA_TIDE, 0))
                            effValue += dummy->GetAmount();
                    // Regenerate 6% of Total Mana Every 3 secs
                    CastSpellExtraArgs args(GetOriginalCaster()->GetGUID());
                    args.AddSpellBP0(CalculatePct(unitTarget->GetMaxPower(POWER_MANA), effValue));
                    caster->CastSpell(unitTarget, SPELL_SHAMAN_MANA_TIDE_TOTEM, args);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_mana_tide_totem::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -30881 - Nature's Guardian
class spell_sha_nature_guardian : public AuraScript
{
    PrepareAuraScript(spell_sha_nature_guardian);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_SHAMAN_NATURE_GUARDIAN,
                SPELL_SHAMAN_NATURE_GUARDIAN_THREAT
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return false;

        int32 healthpct = GetEffectInfo(EFFECT_1).CalcValue();
        if (Unit* target = eventInfo.GetActionTarget())
            if (target->HealthBelowPctDamaged(healthpct, damageInfo->GetDamage()))
                return true;

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* target = eventInfo.GetActionTarget();
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(target->GetMaxHealth(), aurEff->GetAmount()));
        target->CastSpell(target, SPELL_SHAMAN_NATURE_GUARDIAN, args);
        if (Unit* attacker = eventInfo.GetActor())
            target->CastSpell(attacker, SPELL_SHAMAN_NATURE_GUARDIAN_THREAT, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_nature_guardian::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_nature_guardian::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 1230066 - Sentry Totem
class spell_sha_sentry_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_sentry_totem);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_BIND_SIGHT });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (Creature* totem = caster->GetMap()->GetCreature(caster->m_SummonSlot[4]))
                if (totem->IsTotem())
                    caster->CastSpell(totem, SPELL_SHAMAN_BIND_SIGHT, true);
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->GetTypeId() == TYPEID_PLAYER)
                caster->ToPlayer()->StopCastingBindSight();
    }

    void Register() override
    {
         AfterEffectApply += AuraEffectApplyFn(spell_sha_sentry_totem::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
         AfterEffectRemove += AuraEffectRemoveFn(spell_sha_sentry_totem::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 30823 - Shamanistic Rage
class spell_sha_shamanistic_rage : public AuraScript
{
    PrepareAuraScript(spell_sha_shamanistic_rage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        int32 amount = CalculatePct(static_cast<int32>(target->GetTotalAttackPowerValue(BASE_ATTACK)), aurEff->GetAmount());
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        target->CastSpell(target, SPELL_SHAMAN_SHAMANISTIC_RAGE_PROC, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_shamanistic_rage::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 55278, 55328, 55329, 55330, 55332, 55333, 55335, 58589, 58590, 58591, 1230052 - Stoneclaw Totem
class spell_sha_stoneclaw_totem : public SpellScript
{
    PrepareSpellScript(spell_sha_stoneclaw_totem);

    void HandleScriptEffect(SpellEffIndex /* effIndex */)
    {
        Unit* target = GetHitUnit();

        // Cast Absorb on totems
        for (uint8 slot = SUMMON_SLOT_TOTEM_FIRE; slot < MAX_TOTEM_SLOT; ++slot)
        {
            if (!target->m_SummonSlot[slot])
                continue;

            Creature* totem = target->GetMap()->GetCreature(target->m_SummonSlot[slot]);
            if (totem && totem->IsTotem())
            {
                CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
                int32 value = int32(CalculatePct(target->GetMaxHealth(), GetEffectValue()));
                args.AddSpellMod(SPELLVALUE_BASE_POINT0, value);
                GetCaster()->CastSpell(totem, SPELL_SHAMAN_STONECLAW_TOTEM, args);
            }
        }

        // Glyph of Stoneclaw Totem
        if (AuraEffect* aur = target->GetAuraEffect(SPELL_SHAMAN_GLYPH_OF_STONECLAW_TOTEM, 0))
        {
            CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
            args.AddSpellMod(SPELLVALUE_BASE_POINT0, GetEffectValue() * aur->GetAmount());
            GetCaster()->CastSpell(target, SPELL_SHAMAN_STONECLAW_TOTEM, args);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_stoneclaw_totem::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 58877 - Spirit Hunt
class spell_sha_spirit_hunt : public AuraScript
{
    PrepareAuraScript(spell_sha_spirit_hunt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_SPIRIT_HUNT_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* caster = eventInfo.GetActor();
        Unit* target = caster->GetOwner();
        if (!target)
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(damageInfo->GetDamage(), aurEff->GetAmount()));
        caster->CastSpell(caster, SPELL_SHAMAN_SPIRIT_HUNT_HEAL, args);
        caster->CastSpell(target, SPELL_SHAMAN_SPIRIT_HUNT_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spirit_hunt::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -51525 - Static Shock
class spell_sha_static_shock : public AuraScript
{
    PrepareAuraScript(spell_sha_static_shock);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1 });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();

        // Get Lightning Shield
        AuraEffect const* lightningShield = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x00000400, 0x00000000, 0x00000000, caster->GetGUID());
        if (!lightningShield)
            return;

        uint32 spellId = sSpellMgr->GetSpellWithRank(SPELL_SHAMAN_LIGHTNING_SHIELD_DAMAGE_R1, lightningShield->GetSpellInfo()->GetRank());
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), spellId, aurEff);
        lightningShield->GetBase()->DropCharge();
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_static_shock::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 55198 - Tidal Force
class spell_sha_tidal_force_dummy : public AuraScript
{
    PrepareAuraScript(spell_sha_tidal_force_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TIDAL_FORCE_CRIT });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->RemoveAuraFromStack(SPELL_SHAMAN_TIDAL_FORCE_CRIT);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_tidal_force_dummy::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -51490 - Thunderstorm
class spell_sha_thunderstorm : public SpellScript
{
    PrepareSpellScript(spell_sha_thunderstorm);

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        // Glyph of Thunderstorm
        if (GetCaster()->HasAura(SPELL_SHAMAN_GLYPH_OF_THUNDERSTORM))
            PreventHitDefaultEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_thunderstorm::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 38443 - Totemic Mastery (Tier 6 - 2P)
class spell_sha_totemic_mastery : public AuraScript
{
    PrepareAuraScript(spell_sha_totemic_mastery);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_TOTEMIC_MASTERY });
    }

    void HandleDummy(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();
        for (uint8 i = SUMMON_SLOT_TOTEM_FIRE; i < MAX_TOTEM_SLOT; ++i)
            if (!target->m_SummonSlot[i])
                return;

        target->CastSpell(target, SPELL_SHAMAN_TOTEMIC_MASTERY, aurEff);
        PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_totemic_mastery::HandleDummy, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 28823 - Totemic Power
class spell_sha_t3_6p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_6p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_TOTEMIC_POWER_ARMOR,
            SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER,
            SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER,
            SPELL_SHAMAN_TOTEMIC_POWER_MP5
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        uint32 spellId;
        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        switch (target->GetClass())
        {
            case CLASS_PALADIN:
            case CLASS_PRIEST:
            case CLASS_SHAMAN:
            case CLASS_DRUID:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_MP5;
                break;
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_SPELL_POWER;
                break;
            case CLASS_HUNTER:
            case CLASS_ROGUE:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_ATTACK_POWER;
                break;
            case CLASS_WARRIOR:
                spellId = SPELL_SHAMAN_TOTEMIC_POWER_ARMOR;
                break;
            default:
                return;
        }

        caster->CastSpell(target, spellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t3_6p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 28820 - Lightning Shield
class spell_sha_t3_8p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t3_8p_bonus);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        // Need remove self if Lightning Shield not active
        if (!GetTarget()->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_SHAMAN, 0x400, 0, 0))
            Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_t3_8p_bonus::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 64928 - Item - Shaman T8 Elemental 4P Bonus
class spell_sha_t8_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t8_elemental_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_ELECTRIFIED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_ELECTRIFIED);
        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks(eventInfo.GetActor()) > 0);
        amount /= spellInfo->GetMaxTicks(eventInfo.GetActor());

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(target, SPELL_SHAMAN_ELECTRIFIED, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t8_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 67228 - Item - Shaman T9 Elemental 4P Bonus (Lava Burst)
class spell_sha_t9_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t9_elemental_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE);
        int32 amount = CalculatePct(static_cast<int32>(damageInfo->GetDamage()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks(eventInfo.GetActor()) > 0);
        amount /= spellInfo->GetMaxTicks(eventInfo.GetActor());

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(target, SPELL_SHAMAN_LAVA_BURST_BONUS_DAMAGE, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t9_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70817 - Item - Shaman T10 Elemental 4P Bonus
class spell_sha_t10_elemental_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_elemental_4p_bonus);

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        // try to find spell Flame Shock on the target
        AuraEffect* flameShock = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, 0x10000000, 0x00000000, 0x00000000, caster->GetGUID());
        if (!flameShock)
            return;

        Aura* flameShockAura = flameShock->GetBase();

        int32 maxDuration = flameShockAura->GetMaxDuration();
        int32 newDuration = flameShockAura->GetDuration() + aurEff->GetAmount() * IN_MILLISECONDS;

        flameShockAura->SetDuration(newDuration);
        // is it blizzlike to change max duration for FS?
        if (newDuration > maxDuration)
            flameShockAura->SetMaxDuration(newDuration);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_elemental_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 70808 - Item - Shaman T10 Restoration 4P Bonus
class spell_sha_t10_restoration_4p_bonus : public AuraScript
{
    PrepareAuraScript(spell_sha_t10_restoration_4p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHAMAN_CHAINED_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_CHAINED_HEAL);
        int32 amount = CalculatePct(static_cast<int32>(healInfo->GetHeal()), aurEff->GetAmount());

        ASSERT(spellInfo->GetMaxTicks(eventInfo.GetActor()) > 0);
        amount /= spellInfo->GetMaxTicks(eventInfo.GetActor());

        Unit* caster = eventInfo.GetActor();
        Unit* target = eventInfo.GetProcTarget();

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(target, SPELL_SHAMAN_CHAINED_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_t10_restoration_4p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 33757 - Windfury Weapon (Passive)
class spell_sha_windfury_weapon : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_WINDFURY_WEAPON_R1,
            SPELL_SHAMAN_WINDFURY_ATTACK_MH,
            SPELL_SHAMAN_WINDFURY_ATTACK_OH
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Player* player = eventInfo.GetActor()->ToPlayer();
        if (!player)
            return false;

        Item* item = player->GetItemByGuid(GetAura()->GetCastItemGUID());
        if (!item || !item->IsEquipped())
            return false;

        WeaponAttackType attType = static_cast<WeaponAttackType>(player->GetAttackBySlot(item->GetSlot()));
        if (attType != BASE_ATTACK && attType != OFF_ATTACK)
            return false;

        if (((attType == BASE_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)) ||
            ((attType == OFF_ATTACK) && !(eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)))
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Player* player = eventInfo.GetActor()->ToPlayer();

        uint32 spellId = 0;
        WeaponAttackType attType = BASE_ATTACK;
        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_MAINHAND_ATTACK)
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_MH;

        if (eventInfo.GetTypeMask() & PROC_FLAG_DONE_OFFHAND_ATTACK)
        {
            spellId = SPELL_SHAMAN_WINDFURY_ATTACK_OH;
            attType = OFF_ATTACK;
        }

        Item* item = ASSERT_NOTNULL(player->GetWeaponForAttack(attType));

        int32 enchantId = static_cast<int32>(item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
        int32 extraAttackPower = 0;
        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_SHAMAN_WINDFURY_WEAPON_R1);
        while (spellInfo)
        {
            if (spellInfo->GetEffect(EFFECT_0).MiscValue == enchantId)
            {
                extraAttackPower = spellInfo->GetEffect(EFFECT_1).CalcValue(player);
                break;
            }
            spellInfo = spellInfo->GetNextRankSpell();
        }

        if (!extraAttackPower)
            return;

        // Value gained from additional AP
        int32 amount = static_cast<int32>(extraAttackPower / 14.f * player->GetAttackTime(attType) / 1000.f);

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        // Attack twice
        for (uint8 i = 0; i < 2; ++i)
            player->CastSpell(eventInfo.GetProcTarget(), spellId, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_windfury_weapon::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_windfury_weapon::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Duskhaven
// 1260007 - Air's Fury
class spell_sha_airs_fury : public AuraScript
{
    PrepareAuraScript(spell_sha_airs_fury);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_RUTHLESSNESS
        });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->CastSpell(target, SPELL_SHAMAN_RUTHLESSNESS, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (target->HasAura(SPELL_SHAMAN_RUTHLESSNESS))
                target->RemoveAura(SPELL_SHAMAN_RUTHLESSNESS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_airs_fury::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_airs_fury::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1230012 - Brimming with Life
class spell_sha_brimming_with_life : public AuraScript
{
    PrepareAuraScript(spell_sha_brimming_with_life);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetTarget())
            if (caster->GetHealth() < caster->GetMaxHealth())
                PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_brimming_with_life::HandlePeriodic, EFFECT_1, SPELL_AURA_MOD_RECOVERY_RATE);
    }
};

// 1270021 - Earth's Empowering Petrichor Proc 1
class spell_sha_earths_empowering_petrichor_aura_one : public AuraScript
{
    PrepareAuraScript(spell_sha_earths_empowering_petrichor_aura_one);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_EEP_SECOND
        });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetTarget())
            if (caster->HasAura(SPELL_SHAMAN_EEP_SECOND))
                caster->RemoveAura(SPELL_SHAMAN_EEP_SECOND);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_earths_empowering_petrichor_aura_one::OnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1270022 - Earth's Empowering Petrichor Proc 2
class spell_sha_earths_empowering_petrichor_aura_two : public AuraScript
{
    PrepareAuraScript(spell_sha_earths_empowering_petrichor_aura_two);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_EEP_FIRST
        });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetTarget())
            if (caster->HasAura(SPELL_SHAMAN_EEP_FIRST))
                caster->RemoveAura(SPELL_SHAMAN_EEP_FIRST);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_earths_empowering_petrichor_aura_two::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1240017 - Elemental Bash
class spell_sha_elemental_bash : public SpellScript
{
    PrepareSpellScript(spell_sha_elemental_bash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_EARTHS_STOICISM,
            SPELL_SHAMAN_ELEMENTAL_BASH_DUMMY,
            SPELL_SHAMAN_ELEMENTAL_BASH_REVERBERATION
        });
    }

    void HandleBeforeHit(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_NONE)
            return;

        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (caster->HasAura(SPELL_SHAMAN_EARTHS_STOICISM))
                {
                    int32 pct = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_EARTHS_STOICISM)->GetEffect(EFFECT_1).CalcValue();
                    damage = int32(CalculatePct(GetHitDamage(), pct));
                    caster->CastSpell(target, SPELL_SHAMAN_ELEMENTAL_BASH_DUMMY, true);
                }
    }

    void HandleAfterHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (caster->HasAura(SPELL_SHAMAN_EARTHS_STOICISM))
                {
                    CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
                    args.AddSpellBP0(damage);

                    caster->CastSpell(target, SPELL_SHAMAN_ELEMENTAL_BASH_REVERBERATION, args);
                }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_sha_elemental_bash::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_sha_elemental_bash::HandleAfterHit);
    }

private:
    int32 damage = 0;
};

// 1240030 - Elemental Bash (Earth's Stoicism Proc)
class spell_sha_elemental_bash_aoe_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_elemental_bash_aoe_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_ELEMENTAL_BASH_DUMMY
        });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_SHAMAN_ELEMENTAL_BASH_DUMMY));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_elemental_bash_aoe_proc::RemoveInvalidTargets, EFFECT_ALL, TARGET_DEST_TARGET_ENEMY);
    }
};

// 1250030 - Elemental Prowess (Buff)
class spell_sha_elemental_prowess_buff : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_prowess_buff);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_ELEMENTAL_PROWESS
        });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Unit* owner = target->GetOwner())
                if (!owner->HasAura(SPELL_SHAMAN_ELEMENTAL_PROWESS))
                    PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_elemental_prowess_buff::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_SPELL_POWER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1260030 - Fire Nova
class spell_sha_fire_nova_dummy : public SpellScript
{
    PrepareSpellScript(spell_sha_fire_nova_dummy);

    SpellCastResult CheckRequirement()
    {
        Unit* caster = GetCaster();
        float max_range = GetSpellInfo()->GetEffect(EFFECT_0).CalcRadius();

        Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(caster, caster, max_range);
        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targets, u_check);
        Cell::VisitAllObjects(caster, searcher, max_range);

        for (std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
        {
            if ((*tIter)->HasAura(SPELL_SHAMAN_FLAME_SHOCK))
                if (((*tIter)->GetAura(SPELL_SHAMAN_FLAME_SHOCK)->GetCasterGUID() != caster->GetGUID()) || (*tIter)->IsTotem() || (*tIter)->IsSpiritService() || (*tIter)->IsCritter())
                    targets.erase(tIter++);
                else
                    ++tIter;
            else
                targets.erase(tIter++);
        }

        if (targets.empty())
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_FLAME_SHOCK_NOT_ACTIVE);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_CAST_OK;
    }

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            CastSpellExtraArgs args(TRIGGERED_FULL_MASK);

            if (targets.size() == 1)
            {
                int32 spellDamage = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FIRE_NOVA_PROC)->GetEffect(EFFECT_0).CalcValue();
                int32 spellPct = GetSpellInfo()->GetEffect(EFFECT_0).CalcValue();
                int32 bonusDamage = int32(CalculatePct(spellDamage, spellPct));
                args.AddSpellBP0(bonusDamage);
            }

            for (std::list<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                caster->CastSpell(*itr, SPELL_SHAMAN_FIRE_NOVA_PROC, args);
        }
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sha_fire_nova_dummy::CheckRequirement);
        AfterCast += SpellCastFn(spell_sha_fire_nova_dummy::HandleAfterCast);
    }

private:
    std::list<Unit*> targets;
};

// 1230029 - Flameshock
class spell_sha_flameshock_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_flameshock_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_FLAME_SHOCK,
            SPELL_SHAMAN_MOLTEN_ASSAULT
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* target = eventInfo.GetProcTarget();
        Unit* caster = GetCaster();

        if (!caster || !target)
            return false;

        if (caster->GetGUID() != eventInfo.GetDamageInfo()->GetAttacker()->GetGUID())
            return false;

        if (!caster->HasAura(SPELL_SHAMAN_MOLTEN_ASSAULT))
            return false;
        else
        {
            uint32 amount = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_FLAME_SHOCK)->GetEffect(EFFECT_2).CalcValue();
            _procTargetList = eventInfo.GetActor()->SelectNearbyTargets(eventInfo.GetProcTarget(), 8.f, amount);
        }

        return !_procTargetList.empty();
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        for (std::list<Unit*>::iterator itr = _procTargetList.begin(); itr != _procTargetList.end(); ++itr)
            GetCaster()->CastSpell(*itr, SPELL_SHAMAN_FLAME_SHOCK, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_flameshock_aura::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_sha_flameshock_aura::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    std::list<Unit*> _procTargetList;
};

// 1240015 - Frostbrand Weapon
class spell_sha_frostbrand_weapon_proc : public SpellScript
{
    PrepareSpellScript(spell_sha_frostbrand_weapon_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_FROSTBRAND_SLOWED
        });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_SHAMAN_FROSTBRAND_SLOWED, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_frostbrand_weapon_proc::HandleOnHit);
    }
};

// 1270014 - Greater Healing Wave
class spell_sha_greater_healing_wave : public AuraScript
{
    PrepareAuraScript(spell_sha_greater_healing_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY,
            SPELL_SHAMAN_GREATER_HEALING_WAVE_HEAL
        });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetTarget();

        if (!caster || !caster->IsAlive())
            return;

        std::list<Unit*> TargetList;
        Trinity::AnyFriendlyUnitInObjectRangeCheck checker(caster, caster, 40.0f);
        Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(caster, TargetList, checker);
        Cell::VisitAllObjects(caster, searcher, 40.0f);

        for (std::list<Unit*>::iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
            if (!(*itr)->HasAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY))
                TargetList.erase(itr++);
            else
                if ((*itr)->GetAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY)->GetCasterGUID() != caster->GetGUID())
                    TargetList.erase(itr++);

        if (!TargetList.empty())
            caster->CastSpell(TargetList.front(), SPELL_SHAMAN_GREATER_HEALING_WAVE_HEAL, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_greater_healing_wave::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1270018 - Greater Healing Wave (Heal)
class spell_sha_greater_healing_wave_heal : public SpellScript
{
    PrepareSpellScript(spell_sha_greater_healing_wave_heal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_GREATER_HEALING_WAVE_COUNTER,
            SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY
        });
    }

    void CalculateHeal(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
            {
                if (!caster->HasAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_COUNTER) || !target)
                    return;
                else
                {
                    if (!target->HasAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY))
                        return;

                    target->RemoveAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_DUMMY);

                    int32 damage = GetHitDamage();
                    uint32 auraCount = caster->GetAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_COUNTER)->GetStackAmount();
                    damage = damage * auraCount / 40;

                    SetHitDamage(damage);
                    caster->RemoveAura(SPELL_SHAMAN_GREATER_HEALING_WAVE_COUNTER);
                }
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_greater_healing_wave_heal::CalculateHeal, EFFECT_0, SPELL_EFFECT_HEAL);
    }
};

// 1240011 - Grit
class spell_sha_grit : public AuraScript
{
    PrepareAuraScript(spell_sha_grit);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_GRIT
        });
    }

    bool CheckProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        Unit* procTarget = eventInfo.GetProcTarget();
        int32 dmgPct = (float)eventInfo.GetDamageInfo()->GetDamage() / procTarget->GetMaxHealth() * 100;
        int32 pct = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_GRIT)->GetEffect(EFFECT_0).CalcValue();

        if (pct < dmgPct)
            return false;

        return true;
    }

    void Register() override
    {
        DoCheckEffectProc += AuraCheckEffectProcFn(spell_sha_grit::CheckProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 1270011 - Healing Rain
class spell_sha_healing_rain : public AuraScript
{
    PrepareAuraScript(spell_sha_healing_rain);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_ACID_RAIN
        });
    }

    void HandlePeriodicDamage(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();

        if (!caster || !caster->HasAura(SPELL_SHAMAN_ACID_RAIN))
            PreventDefaultAction();
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();

        if (!caster || !caster->HasAura(SPELL_SHAMAN_EEP_TALENT) || !IsExpired())
            return;

        caster->CastSpell(caster, SPELL_SHAMAN_EEP_FIRST, true);
        caster->CastSpell(caster, SPELL_SHAMAN_EEP_SECOND, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_healing_rain::HandlePeriodicDamage, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        AfterEffectRemove += AuraEffectRemoveFn(spell_sha_healing_rain::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_sha_healing_rain_spell : public SpellScript
{
    PrepareSpellScript(spell_sha_healing_rain_spell);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_OVERFLOWING_SHORES
        });
    }

    void HandleHitEffect(SpellEffIndex effIndex)
    {
        if (!GetCaster()->HasAura(SPELL_SHAMAN_OVERFLOWING_SHORES))
            PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_healing_rain_spell::HandleHitEffect, EFFECT_2, SPELL_EFFECT_HEAL);
    }
};

// 1240022 - Lava Pool Totem (dmg)
class spell_sha_lava_pool_totem_dmg : public SpellScript
{
    PrepareSpellScript(spell_sha_lava_pool_totem_dmg);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_LAVA_POOL_DEMORALIZE
        });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_SHAMAN_LAVA_POOL_DEMORALIZE, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_lava_pool_totem_dmg::HandleOnHit);
    }
};

// 1240027 - Lesser Elemental Bash
class spell_sha_lesser_elemental_bash : public SpellScript
{
    PrepareSpellScript(spell_sha_lesser_elemental_bash);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_AIRS_GRACE
        });
    }

    void HandleOnHit()
    {
        int32 pct = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_AIRS_GRACE)->GetEffect(EFFECT_0).CalcValue();
        int32 damage = CalculatePct(GetHitDamage(), pct);

        SetHitDamage(damage);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_sha_lesser_elemental_bash::HandleOnHit);
    }
};

// 1250000 - Mastery: Elemental Overload
class spell_sha_elemental_overload : public AuraScript
{
    PrepareAuraScript(spell_sha_elemental_overload);

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        DamageInfo* dmgInfo = eventInfo.GetDamageInfo();
        Unit* caster = GetTarget();
        Unit* target = dmgInfo->GetVictim();
        uint32 spellId = 0;
        int32 mult = caster->GetAuraEffect(SPELL_SHAMAN_MASTERY_ELEMENTAL_OVERLOAD, EFFECT_0)->GetAmount();
        int32 damage = CalculatePct(dmgInfo->GetDamage(), mult);

        switch (dmgInfo->GetSpellInfo()->Id)
        {
            case SPELL_SHAMAN_CHAIN_LIGHTNING:
                spellId = SPELL_SHAMAN_CHAIN_LIGHTNING_PROC;
                break;
            case SPELL_SHAMAN_ICEFURY:
                spellId = SPELL_SHAMAN_ICEFURY_PROC;
                break;
            case SPELL_SHAMAN_LAVA_BURST:
                spellId = SPELL_SHAMAN_LAVA_BURST_PROC;
                break;
            case SPELL_SHAMAN_LIGHTNING_BOLT:
                spellId = SPELL_SHAMAN_LIGHTNING_BOLT_PROC;
                break;
            default:
                break;
        }

        if (!dmgInfo || !caster || !target || !spellId || !damage)
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(damage);
        caster->CastSpell(target, spellId, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_elemental_overload::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1270000 - Mastery: Spiritual Reservoir
class spell_sha_spiritual_reservoir_store : public AuraScript
{
    PrepareAuraScript(spell_sha_spiritual_reservoir_store);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR,
            SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR
        });
    }

    void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        Unit* caster = GetTarget();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        HealInfo* healInfo = eventInfo.GetHealInfo();

        if (!caster->HasAura(SPELL_SHAMAN_ASTRAL_ALIGNMENT))
            if (!healInfo)
                return;

        uint32 overheal = healInfo->GetHeal() - healInfo->GetEffectiveHeal();

        if (!caster->HasAura(SPELL_SHAMAN_ASTRAL_ALIGNMENT))
            if (!overheal)
                return;

        if (caster && caster->HasAura(SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR))
        {
            if (overheal < 0)
                overheal = 0;
        
            AuraEffect* overhealReservoir = caster->GetAuraEffect(SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR, EFFECT_0);
            int32 currentAmount = overhealReservoir->GetAmount();
            int32 overhealMult = caster->GetAuraEffect(SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR, EFFECT_0)->GetAmount();
            uint32 overhealAmt = uint32(CalculatePct(overheal, overhealMult));
            int32 healthMult = caster->GetAuraEffect(SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR, EFFECT_1)->GetAmount();
            uint32 maxReservoir = uint32(CalculatePct(caster->GetMaxHealth(), healthMult));
            int32 damageDone = 0;
        
            if (damageInfo && damageInfo->GetAttacker() == caster)
            {
                int32 damagePct = sSpellMgr->GetSpellInfo(SPELL_SHAMAN_ASTRAL_ALIGNMENT)->GetEffect(EFFECT_0).CalcValue();
                damageDone = CalculatePct(damageInfo->GetDamage(), damagePct);
            }
            
            if ((currentAmount + overhealAmt + damageDone) > maxReservoir)
                overhealReservoir->SetAmount(maxReservoir);
            else
                overhealReservoir->SetAmount(currentAmount + overhealAmt + damageDone);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spiritual_reservoir_store::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1270003 - Spiritual Awakening
class spell_sha_spiritual_awakening : public AuraScript
{
    PrepareAuraScript(spell_sha_spiritual_awakening);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR,
            SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR,
            SPELL_SHAMAN_SPIRITUAL_AWAKENING,
            SPELL_SHAMAN_SPIRITUAL_AWAKENING_HEAL
        });
    }

    void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        HealInfo* healInfo = eventInfo.GetHealInfo();

        if (!healInfo)
            return;

        Unit* healTarget = healInfo->GetTarget();

        if (!healTarget)
            return;

        if (Unit* caster = GetTarget())
            if (caster->HasAura(SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR))
            {
                Unit* triggerTarget = nullptr;
                Trinity::MostHPMissingGroupInRange u_check(healTarget, 100.f, 0);
                Trinity::UnitLastSearcher<Trinity::MostHPMissingGroupInRange> searcher(healTarget, triggerTarget, u_check);
                Cell::VisitAllObjects(healTarget, searcher, 100.f);

                if (!triggerTarget)
                    return;

                int32 healthMult = caster->GetAuraEffect(SPELL_SHAMAN_RESTORATION_MASTERY_SPIRITUAL_RESERVOIR, EFFECT_1)->GetAmount();
                uint32 maxReservoir = uint32(CalculatePct(caster->GetMaxHealth(), healthMult));
                AuraEffect* overhealReservoir = caster->GetAuraEffect(SPELL_SHAMAN_RESTORATION_MASTERY_OVERHEAL_RESERVOIR, EFFECT_0);
                int32 currentAmount = overhealReservoir->GetAmount();

                if (!currentAmount)
                    return;

                int32 useAmount = CalculatePct(currentAmount, 33);
                int32 reservoirPct = caster->GetAuraEffect(SPELL_SHAMAN_SPIRITUAL_AWAKENING, EFFECT_1)->GetAmount();
                int32 restorePct = caster->GetAuraEffect(SPELL_SHAMAN_SPIRITUAL_AWAKENING, EFFECT_2)->GetAmount();

                if (currentAmount < CalculatePct(maxReservoir, reservoirPct))
                    overhealReservoir->SetAmount(CalculatePct(maxReservoir, restorePct));
                else
                    overhealReservoir->SetAmount(currentAmount - useAmount);

                CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
                args.AddSpellBP0(useAmount);
                caster->CastSpell(triggerTarget, SPELL_SHAMAN_SPIRITUAL_AWAKENING_HEAL, args);
            }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_sha_spiritual_awakening::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1230074 - Water's Calming Presence
class spell_sha_waters_calming_presence : public AuraScript
{
    PrepareAuraScript(spell_sha_waters_calming_presence);

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Unit* owner = target->GetOwner())
                if (!owner->HasAura(SPELL_SHAMAN_WATERS_CALMING_PRESENCE))
                    PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_waters_calming_presence::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1260005 - Windfury (Dummy Aura)
class spell_sha_windfury_weapon_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_windfury_weapon_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_WINDFURY_PROC
        });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            target->CastSpell(target, SPELL_SHAMAN_WINDFURY_PROC, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_windfury_weapon_aura::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1230065 - Wrath (Flametongue Totem)
class spell_sha_wrath_flametongue_totem : public AuraScript
{
    PrepareAuraScript(spell_sha_wrath_flametongue_totem);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_TOTEMIC_WRATH
        });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Unit* owner = target->GetOwner())
                if (owner->HasAura(SPELL_SHAMAN_TOTEMIC_WRATH))
                    PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_wrath_flametongue_totem::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_SPELL_POWER, AURA_EFFECT_HANDLE_REAL);
    }
};


// 1250013 - Wrath (Totemic Wrath)
class spell_sha_wrath_totemic_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_wrath_totemic_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_SHAMAN_TOTEMIC_WRATH
        });
    }

    void HandleOnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Unit* owner = target->GetOwner())
                if (!owner->HasAura(SPELL_SHAMAN_TOTEMIC_WRATH))
                    PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_wrath_totemic_aura::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_SPELL_POWER, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_shaman_spell_scripts()
{
    RegisterSpellScript(spell_sha_ancestral_awakening);
    RegisterSpellScript(spell_sha_ancestral_awakening_proc);
    RegisterSpellScript(spell_sha_astral_shift);
    RegisterSpellScript(spell_sha_astral_shift_aura);
    RegisterSpellScript(spell_sha_astral_shift_visual_dummy);
    RegisterSpellScript(spell_sha_bloodlust);
    RegisterSpellScript(spell_sha_chain_heal);
    RegisterSpellScript(spell_sha_cleansing_totem_pulse);
    RegisterSpellScript(spell_sha_clearcasting);
    RegisterSpellScript(spell_sha_earth_shield);
    RegisterSpellScript(spell_sha_earthbind_totem);
    RegisterSpellScript(spell_sha_earthen_power);
    RegisterSpellScript(spell_sha_earthliving_weapon);
    RegisterSpellScript(spell_sha_fire_nova);
    RegisterSpellScript(spell_sha_flame_shock);
    RegisterSpellScript(spell_sha_flametongue_weapon);
    RegisterSpellScript(spell_sha_frozen_power);
    RegisterSpellScript(spell_sha_glyph_of_earth_shield);
    RegisterSpellScript(spell_sha_glyph_of_healing_wave);
    RegisterSpellScript(spell_sha_glyph_of_totem_of_wrath);
    RegisterSpellScript(spell_sha_healing_stream_totem);
    RegisterSpellScript(spell_sha_heroism);
    RegisterSpellScript(spell_sha_imp_water_shield);
    RegisterSpellScript(spell_sha_lightning_overload);
    RegisterSpellScript(spell_sha_item_lightning_shield);
    RegisterSpellScript(spell_sha_item_lightning_shield_trigger);
    RegisterSpellScript(spell_sha_item_mana_surge);
    RegisterSpellScript(spell_sha_item_t6_trinket);
    RegisterSpellScript(spell_sha_item_t10_elemental_2p_bonus);
    RegisterSpellScript(spell_sha_lava_lash);
    RegisterSpellScript(spell_sha_lightning_shield);
    RegisterSpellScript(spell_sha_maelstrom_weapon);
    RegisterSpellScript(spell_sha_mana_spring_totem);
    RegisterSpellScript(spell_sha_mana_tide);
    RegisterSpellScript(spell_sha_mana_tide_totem);
    RegisterSpellScript(spell_sha_nature_guardian);
    RegisterSpellScript(spell_sha_sentry_totem);
    RegisterSpellScript(spell_sha_shamanistic_rage);
    RegisterSpellScript(spell_sha_stoneclaw_totem);
    RegisterSpellScript(spell_sha_spirit_hunt);
    RegisterSpellScript(spell_sha_static_shock);
    RegisterSpellScript(spell_sha_tidal_force_dummy);
    RegisterSpellScript(spell_sha_thunderstorm);
    RegisterSpellScript(spell_sha_totemic_mastery);
    RegisterSpellScript(spell_sha_t3_6p_bonus);
    RegisterSpellScript(spell_sha_t3_8p_bonus);
    RegisterSpellScript(spell_sha_t8_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_t9_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_t10_elemental_4p_bonus);
    RegisterSpellScript(spell_sha_t10_restoration_4p_bonus);
    RegisterSpellScript(spell_sha_windfury_weapon);
    // Duskhaven
    // RegisterSpellScript(spell_sha_airs_fury);
    // RegisterSpellScript(spell_sha_brimming_with_life);
    // RegisterSpellScript(spell_sha_earths_empowering_petrichor_aura_one);
    // RegisterSpellScript(spell_sha_earths_empowering_petrichor_aura_two);
    // RegisterSpellScript(spell_sha_elemental_bash);
    // RegisterSpellScript(spell_sha_elemental_bash_aoe_proc);
    // RegisterSpellScript(spell_sha_elemental_prowess_buff);
    // RegisterSpellScript(spell_sha_fire_nova_dummy);
    // RegisterSpellScript(spell_sha_flameshock_aura);
    // RegisterSpellScript(spell_sha_frostbrand_weapon_proc);
    // RegisterSpellScript(spell_sha_greater_healing_wave);
    // RegisterSpellScript(spell_sha_greater_healing_wave_heal);
    // RegisterSpellScript(spell_sha_grit);
    // RegisterSpellScript(spell_sha_healing_rain);
    // RegisterSpellScript(spell_sha_healing_rain_spell);
    // RegisterSpellScript(spell_sha_lava_pool_totem_dmg);
    // RegisterSpellScript(spell_sha_lesser_elemental_bash);
    // RegisterSpellScript(spell_sha_elemental_overload);
    // RegisterSpellScript(spell_sha_spiritual_reservoir_store);
    // RegisterSpellScript(spell_sha_spiritual_awakening);
    // RegisterSpellScript(spell_sha_waters_calming_presence);
    // RegisterSpellScript(spell_sha_windfury_weapon_aura);
    // RegisterSpellScript(spell_sha_wrath_flametongue_totem);
    // RegisterSpellScript(spell_sha_wrath_totemic_aura);
}
