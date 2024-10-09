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
 * Scripts for spells with SPELLFAMILY_WARLOCK and SPELLFAMILY_GENERIC spells used by warlock players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warl_".
 */

#include "ScriptMgr.h"
#include "GridNotifiers.h"
#include "Creature.h"
#include "Containers.h"
#include "GameObject.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Optional.h"
#include "Player.h"
#include "Random.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"

enum WarlockSpells
{
    SPELL_WARLOCK_DRAIN_SOUL_R1                     = 1340011,
    SPELL_WARLOCK_CREATE_SOULSHARD                  = 1340012,
    SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT              = 18662,
    SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON             = 48018,
    SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT           = 48020,
    SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST         = 62388,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS      = 54435,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER    = 54443,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD      = 54508,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER     = 54509,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP           = 54444,
    SPELL_WARLOCK_DEMONIC_PACT_PROC                 = 48090,
    SPELL_WARLOCK_FEL_SYNERGY_HEAL                  = 1320002,
    SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA          = 58070,
    SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC          = 58068,
    SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME              = 63311,
    SPELL_WARLOCK_GLYPH_OF_SIPHON_LIFE              = 56216,
    SPELL_WARLOCK_HAUNT                             = 1340018,
    SPELL_WARLOCK_HAUNT_HEAL                        = 1340019,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1           = 18692,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2           = 18693,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1         = 18703,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2         = 18704,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1    = 60955,
    SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2    = 60956,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE                 = 31818,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2               = 32553,
    SPELL_WARLOCK_NETHER_PROTECTION_HOLY            = 54370,
    SPELL_WARLOCK_NETHER_PROTECTION_FIRE            = 54371,
    SPELL_WARLOCK_NETHER_PROTECTION_FROST           = 54372,
    SPELL_WARLOCK_NETHER_PROTECTION_ARCANE          = 54373,
    SPELL_WARLOCK_NETHER_PROTECTION_SHADOW          = 54374,
    SPELL_WARLOCK_NETHER_PROTECTION_NATURE          = 54375,
    SPELL_WARLOCK_SOULSHATTER_EFFECT                = 32835,
    SPELL_WARLOCK_SIPHON_LIFE_HEAL                  = 63106,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL        = 1340025,
    SPELL_WARLOCK_GLYPH_OF_LIFE_TAP_TRIGGERED       = 63321,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_R1      = 1340010,
    SPELL_WARLOCK_SEED_OF_CORRUPTION_GENERIC        = 32865,
    SPELL_WARLOCK_SHADOW_TRANCE                     = 17941,
    SPELL_WARLOCK_SOUL_LEECH_HEAL                   = 30294,
    SPELL_WARLOCK_IMP_SOUL_LEECH_R1                 = 54117,
    SPELL_WARLOCK_SOUL_LEECH_PET_MANA_1             = 54607,
    SPELL_WARLOCK_SOUL_LEECH_PET_MANA_2             = 59118,
    SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_1          = 54300,
    SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_2          = 59117,
    SPELL_REPLENISHMENT                             = 57669,
    SPELL_WARLOCK_SHADOWFLAME                       = 37378,
    SPELL_WARLOCK_FLAMESHADOW                       = 37379,
    SPELL_WARLOCK_GLYPH_OF_SUCCUBUS                 = 56250,
    SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_R1            = 18213,
    SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_PROC          = 18371,
    // Duskhaven
    SPELL_WARLOCK_BURNING_SYNERGY_PROC              = 1330027,
    SPELL_WARLOCK_BURNING_SYNERGY_TALENT            = 1330026,
    SPELL_WARLOCK_CHAOS_INCARNATE_CHB25             = 1350022,
    SPELL_WARLOCK_CHAOS_INCARNATE_DMG_BUFF          = 1350024,
    SPELL_WARLOCK_CHAOS_INCARNATE_DMG_SELF          = 1350025,
    SPELL_WARLOCK_CHAOS_INCARNATE_DOT               = 1350021,
    SPELL_WARLOCK_CHAOS_INCARNATE_HEAL              = 1350023,
    SPELL_WARLOCK_CHAOS_INCARNATE_NO_CRIT           = 1350020,
    SPELL_WARLOCK_CHAOS_INCARNATE_TALENT            = 1350019,
    SPELL_WARLOCK_CHAOS_IN_MOTION_20                = 1350032,
    SPELL_WARLOCK_CHAOS_IN_MOTION_40                = 1350033,
    SPELL_WARLOCK_CHAOS_IN_MOTION_60                = 1350034,
    SPELL_WARLOCK_CHAOS_IN_MOTION_TALENT            = 1350031,
    SPELL_WARLOCK_CORRUPTION                        = 25311,
    SPELL_WARLOCK_DEMONBOLT_DGI                     = 1330018,
    SPELL_WARLOCK_DEMONBOLT_DUMMY                   = 1330014,
    SPELL_WARLOCK_DEMONBOLT_FG                      = 1330017,
    SPELL_WARLOCK_DEMONBOLT_FHVV                    = 1330015,
    SPELL_WARLOCK_FELFIRE_BOLTS_CHANNEL             = 1350028,
    SPELL_WARLOCK_FELFIRE_BOLTS_MISSILE             = 1340029,
    SPELL_WARLOCK_IMMOLATE                          = 27215,
    SPELL_WARLOCK_RAGING_FELFIRE                    = 1340030,
    SPELL_WARLOCK_SACRIFICIAL_POWER                 = 1340005,
    SPELL_WARLOCK_SHADOWBURN_DMG                    = 1350010,
    SPELL_WARLOCK_SHADOWBURN_DUMMY                  = 1350007,
    SPELL_WARLOCK_SHADOWY_ASSISTANCE_PROC           = 1330028,
    SPELL_WARLOCK_SHADOWY_ASSISTANCE_TALENT         = 1330029,
    SPELL_WARLOCK_SOULREND                          = 1340027,
    SPELL_WARLOCK_SOW_THE_SEEDS                     = 1340017,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION               = 1340023,
    SPELL_WARLOCK_UNSTABLE_AFFLICTION_DOT           = 1340024,
};

enum WarlockSpellIcons
{
    WARLOCK_ICON_ID_IMPROVED_LIFE_TAP               = 208,
    WARLOCK_ICON_ID_MANA_FEED                       = 1982,
    WARLOCK_ICON_ID_DEMONIC_PACT                    = 3220
};

// -980 - Curse of Agony
class spell_warl_curse_of_agony : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_agony);

    void ApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        _tick_amount = aurEff->GetAmount();
    }

    void HandleEffectPeriodicUpdate(AuraEffect * aurEff)
    {
        switch (aurEff->GetTickNumber())
        {
            // 1..4 ticks, 1/2 from normal tick damage
            case 1:
                aurEff->SetAmount(_tick_amount / 2);
                break;
            // 5..8 ticks have normal tick damage
            case 5:
                aurEff->SetAmount(_tick_amount);
                break;
            // 9..12 ticks, 3/2 from normal tick damage
            case 9:
                aurEff->SetAmount((_tick_amount + 1) * 3 / 2); // +1 prevent 0.5 damage possible lost at 1..4 ticks
                break;
            // 13 and 14 ticks (glyphed only), twice normal tick damage
            case 13:
                aurEff->SetAmount(_tick_amount * 2);
                break;
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_warl_curse_of_agony::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_warl_curse_of_agony::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
private:
    uint32 _tick_amount = 0;
};

// -710 - Banish
class spell_warl_banish : public SpellScript
{
    PrepareSpellScript(spell_warl_banish);

public:
    spell_warl_banish() {}

private:
    void HandleBanish(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_IMMUNE)
            return;

        if (Unit* target = GetHitUnit())
        {
            // Casting Banish on a banished target will remove applied aura
            if (Aura * banishAura = target->GetAura(GetSpellInfo()->Id, GetCaster()->GetGUID()))
                banishAura->Remove();
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_warl_banish::HandleBanish);
    }
};

// -6201 - Create Healthstone (and ranks)
class spell_warl_create_healthstone : public SpellScriptLoader
{
    public:
        spell_warl_create_healthstone() : SpellScriptLoader("spell_warl_create_healthstone") { }

        class spell_warl_create_healthstone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_create_healthstone_SpellScript);

            static uint32 const iTypes[8][3];

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return ValidateSpellInfo({ SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1, SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2 });
            }

            SpellCastResult CheckCast()
            {
                if (Player* caster = GetCaster()->ToPlayer())
                {
                    uint8 spellRank = GetSpellInfo()->GetRank();
                    ItemPosCountVec dest;
                    InventoryResult msg = caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, iTypes[spellRank - 1][0], 1, nullptr);
                    if (msg != EQUIP_ERR_OK)
                        return SPELL_FAILED_TOO_MANY_OF_ITEM;
                }
                return SPELL_CAST_OK;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 rank = 0;
                    // Improved Healthstone
                    if (AuraEffect const* aurEff = unitTarget->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, 284, 0))
                    {
                        switch (aurEff->GetId())
                        {
                            case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1:
                                rank = 1;
                                break;
                            case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2:
                                rank = 2;
                                break;
                            default:
                                TC_LOG_ERROR("spells", "Unknown rank of Improved Healthstone id: {}", aurEff->GetId());
                                break;
                        }
                    }
                    uint8 spellRank = GetSpellInfo()->GetRank();
                    if (spellRank > 0 && spellRank <= 8)
                        CreateItem(iTypes[spellRank - 1][rank]);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_create_healthstone_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnCheckCast += SpellCheckCastFn(spell_warl_create_healthstone_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_warl_create_healthstone_SpellScript();
        }
};

uint32 const spell_warl_create_healthstone::spell_warl_create_healthstone_SpellScript::iTypes[8][3] = {
    { 5512, 19004, 19005},              // Minor Healthstone
    { 5511, 19006, 19007},              // Lesser Healthstone
    { 5509, 19008, 19009},              // Healthstone
    { 5510, 19010, 19011},              // Greater Healthstone
    { 9421, 19012, 19013},              // Major Healthstone
    {22103, 22104, 22105},              // Master Healthstone
    {36889, 36890, 36891},              // Demonic Healthstone
    {36892, 36893, 36894}               // Fel Healthstone
};

// -603 - Curse of Doom
class spell_warl_curse_of_doom : public AuraScript
{
    PrepareAuraScript(spell_warl_curse_of_doom);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT });
    }

    bool Load() override
    {
        return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (!GetCaster())
            return;

        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_DEATH || !IsExpired())
            return;

        if (GetCaster()->ToPlayer()->isHonorOrXPTarget(GetTarget()))
            GetCaster()->CastSpell(GetTarget(), SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT, aurEff);
    }

    void Register() override
    {
         AfterEffectRemove += AuraEffectRemoveFn(spell_warl_curse_of_doom::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// -63156 - Decimation
class spell_warl_decimation : public AuraScript
{
    PrepareAuraScript(spell_warl_decimation);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (eventInfo.GetActionTarget()->HasAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, spellInfo, eventInfo.GetActor()))
                return true;

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_decimation::CheckProc);
    }
};

// 48018 - Demonic Circle: Summon
class spell_warl_demonic_circle_summon : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_summon);

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
    {
        // If effect is removed by expire remove the summoned demonic circle too.
        if (!(mode & AURA_EFFECT_HANDLE_REAPPLY))
            GetTarget()->RemoveGameObject(GetId(), true);

        GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
    }

    void HandleDummyTick(AuraEffect const* /*aurEff*/)
    {
        if (GameObject* circle = GetTarget()->GetGameObject(GetId()))
        {
            // Here we check if player is in demonic circle teleport range, if so add
            // WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST; allowing him to cast the WARLOCK_DEMONIC_CIRCLE_TELEPORT.
            // If not in range remove the WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST.

            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT);

            if (GetTarget()->IsWithinDist(circle, spellInfo->GetMaxRange(true)))
            {
                if (!GetTarget()->HasAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST))
                    GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST, true);
            }
            else
                GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_warl_demonic_circle_summon::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_circle_summon::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 48020 - Demonic Circle: Teleport
class spell_warl_demonic_circle_teleport : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_circle_teleport);

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
        {
            if (GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON))
            {
                player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());
                player->RemoveMovementImpairingAuras(false);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_circle_teleport::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 47193 - Demonic Empowerment
class spell_warl_demonic_empowerment : public SpellScript
{
    PrepareSpellScript(spell_warl_demonic_empowerment);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS,
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER,
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD,
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER,
            SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP
        });
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        if (Creature* targetCreature = GetHitCreature())
        {
            if (targetCreature->IsPet())
            {
                CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(targetCreature->GetEntry());
                ASSERT(ci);
                switch (ci->family)
                {
                    case CREATURE_FAMILY_SUCCUBUS:
                        targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, true);
                        break;
                    case CREATURE_FAMILY_VOIDWALKER:
                    {
                        SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER);
                        int32 hp = targetCreature->CountPctFromMaxHealth(GetCaster()->CalculateSpellDamage(spellInfo->GetEffect(EFFECT_0)));
                        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
                        args.AddSpellBP0(hp);
                        targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER, args);
                        //unitTarget->CastSpell(unitTarget, 54441, true);
                        break;
                    }
                    case CREATURE_FAMILY_FELGUARD:
                        targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, true);
                        break;
                    case CREATURE_FAMILY_FELHUNTER:
                        targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, true);
                        break;
                    case CREATURE_FAMILY_IMP:
                        targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP, true);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_demonic_empowerment::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 1340011 - Drain Soul
class spell_warl_drain_soul : public AuraScript
{
    PrepareAuraScript(spell_warl_drain_soul);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_R1,
            SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_PROC,
            SPELL_WARLOCK_CREATE_SOULSHARD,
            SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA,
            SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Drain Soul's proc tries to happen each time the warlock lands a killing blow on a unit while channeling.
        // Make sure that dying unit is afflicted by the caster's Drain Soul debuff in order to avoid a false positive.

        Unit* caster = GetCaster();
        Unit* victim = eventInfo.GetProcTarget();

        if (caster && victim)
            return victim->GetAuraApplication(SPELL_WARLOCK_DRAIN_SOUL_R1, caster->GetGUID()) != 0;

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = eventInfo.GetActor();
        // Improved Drain Soul
        Aura const* impDrainSoul = caster->GetAuraOfRankedSpell(SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_R1, caster->GetGUID());
        if (!impDrainSoul)
            return;

        int32 amount = CalculatePct(caster->GetMaxPower(POWER_MANA), impDrainSoul->GetSpellInfo()->GetEffect(EFFECT_2).CalcValue());
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        caster->CastSpell(nullptr, SPELL_WARLOCK_IMPROVED_DRAIN_SOUL_PROC, args);
    }

    void HandleTick(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();
        if (caster && caster->GetTypeId() == TYPEID_PLAYER && caster->ToPlayer()->isHonorOrXPTarget(target))
        {
            if (roll_chance_i(20))
            {
                caster->CastSpell(caster, SPELL_WARLOCK_CREATE_SOULSHARD, aurEff);
                // Glyph of Drain Soul - chance to create an additional Soul Shard
                if (AuraEffect* aur = caster->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_AURA, EFFECT_0))
                    if (roll_chance_i(aur->GetMiscValue()))
                        caster->CastSpell(caster, SPELL_WARLOCK_GLYPH_OF_DRAIN_SOUL_PROC, aur);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_drain_soul::CheckProc);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_drain_soul::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectProc += AuraEffectProcFn(spell_warl_drain_soul::HandleProc, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 47422 - Everlasting Affliction
class spell_warl_everlasting_affliction : public SpellScript
{
    PrepareSpellScript(spell_warl_everlasting_affliction);

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
        {
            // Refresh corruption on target
            if (AuraEffect* aur = target->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_WARLOCK, 0x2, 0, 0, caster->GetGUID()))
            {
                aur->ChangeAmount(aur->CalculateAmount(aur->GetCaster()), false);
                aur->CalculatePeriodic(caster, false, false);
                aur->GetBase()->RefreshDuration(true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_everlasting_affliction::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 1320001 - Fel Synergy
class spell_warl_fel_synergy : public AuraScript
{
    PrepareAuraScript(spell_warl_fel_synergy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_FEL_SYNERGY_HEAL });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return false;

        return GetTarget()->GetGuardianPet() != nullptr;
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 heal = CalculatePct(static_cast<int32>(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(heal);
        GetTarget()->CastSpell(nullptr, SPELL_WARLOCK_FEL_SYNERGY_HEAL, args); // TARGET_UNIT_PET
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_fel_synergy::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_fel_synergy::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -18094 - Nightfall
//  56218 - Glyph of Corruption
class spell_warl_glyph_of_corruption_nightfall : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_corruption_nightfall);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SHADOW_TRANCE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->CastSpell(caster, SPELL_WARLOCK_SHADOW_TRANCE, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_glyph_of_corruption_nightfall::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 63320 - Glyph of Life Tap
class spell_warl_glyph_of_life_tap : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_life_tap);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_LIFE_TAP_TRIGGERED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->CastSpell(caster, SPELL_WARLOCK_GLYPH_OF_LIFE_TAP_TRIGGERED, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_glyph_of_life_tap::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 63310 - Glyph of Shadowflame
class spell_warl_glyph_of_shadowflame : public AuraScript
{
    PrepareAuraScript(spell_warl_glyph_of_shadowflame);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_WARLOCK_GLYPH_OF_SHADOWFLAME, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_glyph_of_shadowflame::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1340018 - Haunt
class spell_warl_haunt : public SpellScript
{
    PrepareSpellScript(spell_warl_haunt);

    void HandleAfterHit()
    {
        if (Aura* aura = GetHitAura())
            if (AuraEffect* aurEff = aura->GetEffect(EFFECT_1))
                aurEff->SetAmount(CalculatePct(GetHitDamage(), aurEff->GetAmount()));
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_warl_haunt::HandleAfterHit);
    }
};

class spell_warl_haunt_aura : public AuraScript
{
    PrepareAuraScript(spell_warl_haunt_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_HAUNT_HEAL });
    }

    void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
        {
            CastSpellExtraArgs args(aurEff);
            args.OriginalCaster = GetCasterGUID();
            args.AddSpellBP0(aurEff->GetAmount());
            GetTarget()->CastSpell(caster, SPELL_WARLOCK_HAUNT_HEAL, args);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_warl_haunt_aura::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// -755 - Health Funnel
class spell_warl_health_funnel : public AuraScript
{
    PrepareAuraScript(spell_warl_health_funnel);

    void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        Unit* target = GetTarget();
        if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2, true);
        else if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1))
            target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1, true);
    }

    void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1);
        target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_health_funnel::RemoveEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_warl_health_funnel::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
    }
};

// -1454 - Life Tap
class spell_warl_life_tap : public SpellScript
{
    PrepareSpellScript(spell_warl_life_tap);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_LIFE_TAP_ENERGIZE, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2 });
    }

    void HandleDummy(SpellEffIndex effIndex)
    {
        Unit* caster = GetCaster();
        int32 base = GetEffectInfo(effIndex).CalcValue();

        float penalty = caster->CalculateSpellpowerCoefficientLevelPenalty(GetSpellInfo());
        float fmana = (float)base + caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + AsUnderlyingType(SPELL_SCHOOL_SHADOW)) * 0.5f * penalty;

        // Improved Life Tap mod
        if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_IMPROVED_LIFE_TAP, 0))
            AddPct(fmana, aurEff->GetAmount());
        int32 mana = round(fmana);

        // Shouldn't Appear in Combat Log
        caster->ModifyHealth(-base);

        CastSpellExtraArgs args;
        args.AddSpellBP0(mana);
        caster->CastSpell(caster, SPELL_WARLOCK_LIFE_TAP_ENERGIZE, args);

        // Mana Feed
        int32 manaFeedVal = 0;
        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_MANA_FEED, 0))
            manaFeedVal = aurEff->GetAmount();

        if (manaFeedVal > 0)
        {
            ApplyPct(manaFeedVal, mana);
            CastSpellExtraArgs manaFeedArgs(TRIGGERED_FULL_MASK);
            manaFeedArgs.AddSpellBP0(manaFeedVal);
            caster->CastSpell(caster, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2, manaFeedArgs);
        }
    }

    SpellCastResult CheckCast()
    {
        if (int32(GetCaster()->GetHealth()) > int32(GetEffectInfo(EFFECT_0).CalcValue()))
            return SPELL_CAST_OK;
        return SPELL_FAILED_FIZZLE;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnCheckCast += SpellCheckCastFn(spell_warl_life_tap::CheckCast);
    }
};

// -30299 - Nether Protection
class spell_warl_nether_protection : public AuraScript
{
    PrepareAuraScript(spell_warl_nether_protection);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_NETHER_PROTECTION_HOLY,
            SPELL_WARLOCK_NETHER_PROTECTION_FIRE,
            SPELL_WARLOCK_NETHER_PROTECTION_NATURE,
            SPELL_WARLOCK_NETHER_PROTECTION_FROST,
            SPELL_WARLOCK_NETHER_PROTECTION_SHADOW,
            SPELL_WARLOCK_NETHER_PROTECTION_ARCANE
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (DamageInfo* damageInfo = eventInfo.GetDamageInfo())
        {
            switch (GetFirstSchoolInMask(damageInfo->GetSchoolMask()))
            {
                case SPELL_SCHOOL_HOLY:
                case SPELL_SCHOOL_FIRE:
                case SPELL_SCHOOL_NATURE:
                case SPELL_SCHOOL_FROST:
                case SPELL_SCHOOL_SHADOW:
                case SPELL_SCHOOL_ARCANE:
                    return true;
                default:
                    break;
            }
        }

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerspell = 0;

        switch (GetFirstSchoolInMask(eventInfo.GetDamageInfo()->GetSchoolMask()))
        {
            case SPELL_SCHOOL_HOLY:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_HOLY;
                break;
            case SPELL_SCHOOL_FIRE:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_FIRE;
                break;
            case SPELL_SCHOOL_NATURE:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_NATURE;
                break;
            case SPELL_SCHOOL_FROST:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_FROST;
                break;
            case SPELL_SCHOOL_SHADOW:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_SHADOW;
                break;
            case SPELL_SCHOOL_ARCANE:
                triggerspell = SPELL_WARLOCK_NETHER_PROTECTION_ARCANE;
                break;
            default:
                return;
        }

        if (Unit* target = eventInfo.GetActionTarget())
            target->CastSpell(target, triggerspell, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_nether_protection::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_nether_protection::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 54909, 53646 - Demonic Pact
class spell_warl_demonic_pact : public AuraScript
{
    PrepareAuraScript(spell_warl_demonic_pact);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_DEMONIC_PACT_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && eventInfo.GetActor()->IsPet();
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (Unit* owner = eventInfo.GetActor()->GetOwner())
        {
            if (AuraEffect* aurEff = owner->GetDummyAuraEffect(SPELLFAMILY_WARLOCK, WARLOCK_ICON_ID_DEMONIC_PACT, EFFECT_0))
            {
                int32 bp = static_cast<int32>((aurEff->GetAmount() * owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC) + 100.0f) / 100.0f);
                CastSpellExtraArgs args(aurEff);
                args.AddSpellBP0(bp);
                args.AddSpellMod(SPELLVALUE_BASE_POINT1, bp);
                owner->CastSpell(nullptr, SPELL_WARLOCK_DEMONIC_PACT_PROC, args);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_demonic_pact::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_demonic_pact::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

// 18541 - Ritual of Doom Effect
class spell_warl_ritual_of_doom_effect : public SpellScript
{
    PrepareSpellScript(spell_warl_ritual_of_doom_effect);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, GetEffectValue(), true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_warl_ritual_of_doom_effect::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 6358 - Seduction
class spell_warl_seduction : public SpellScript
{
    PrepareSpellScript(spell_warl_seduction);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_GLYPH_OF_SUCCUBUS });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* owner = GetCaster()->GetOwner();
        if (!owner || !owner->HasAura(SPELL_WARLOCK_GLYPH_OF_SUCCUBUS))
            return;

        Unit* target = GetHitUnit();
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, ObjectGuid::Empty, target->GetAura(32409)); // SW:D shall not be removed.
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_warl_seduction::HandleDummy, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 1340010 - Seed of Corruption
class spell_warl_seed_of_corruption : public SpellScript
{
    PrepareSpellScript(spell_warl_seed_of_corruption);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([&](WorldObject const* target)
        {
            if (Unit const* unitTarget = target->ToUnit())
                if (WorldLocation const* dest = GetExplTargetDest())
                    if (!unitTarget->IsWithinLOS(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ()))
                        return true;

            return false;
        });
    }

    void HandleEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(1))
                caster->CastSpell(GetHitUnit(), SPELL_WARLOCK_CORRUPTION, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_seed_of_corruption::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_warl_seed_of_corruption::HandleEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 1340009 - Seed of Corruption
class spell_warl_seed_of_corruption_dummy : public AuraScript
{
    PrepareAuraScript(spell_warl_seed_of_corruption_dummy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_R1 });
    }

    void CalculateBuffer(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // effect 1 scales with 14% of caster's SP (DBC data)
        amount = caster->SpellDamageBonusDone(GetUnitOwner(), GetSpellInfo(), amount, SPELL_DIRECT_DAMAGE, aurEff->GetSpellEffectInfo(), GetAura()->GetDonePct());
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 amount = aurEff->GetAmount() - damageInfo->GetDamage();
        if (amount > 0)
        {
            const_cast<AuraEffect*>(aurEff)->SetAmount(amount);
            if (!GetTarget()->HealthBelowPctDamaged(1, damageInfo->GetDamage()))
                return;
        }

        Remove();

        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint32 spellId = sSpellMgr->GetSpellWithRank(SPELL_WARLOCK_SEED_OF_CORRUPTION_DAMAGE_R1, GetSpellInfo()->GetRank());
        caster->CastSpell(eventInfo.GetActionTarget(), spellId, aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_seed_of_corruption_dummy::CalculateBuffer, EFFECT_1, SPELL_AURA_DUMMY);
        OnEffectProc += AuraEffectProcFn(spell_warl_seed_of_corruption_dummy::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// 32863 - Seed of Corruption
// 36123 - Seed of Corruption
// 38252 - Seed of Corruption
// 39367 - Seed of Corruption
// 44141 - Seed of Corruption
// 70388 - Seed of Corruption
// Monster spells, triggered only on amount drop (not on death)
class spell_warl_seed_of_corruption_generic : public AuraScript
{
    PrepareAuraScript(spell_warl_seed_of_corruption_generic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SEED_OF_CORRUPTION_GENERIC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        int32 amount = aurEff->GetAmount() - damageInfo->GetDamage();
        if (amount > 0)
        {
            const_cast<AuraEffect*>(aurEff)->SetAmount(amount);
            return;
        }

        Remove();

        Unit* caster = GetCaster();
        if (!caster)
            return;

        caster->CastSpell(eventInfo.GetActionTarget(), SPELL_WARLOCK_SEED_OF_CORRUPTION_GENERIC, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_seed_of_corruption_generic::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

// -6229 - Shadow Ward
class spell_warl_shadow_ward : public AuraScript
{
    PrepareAuraScript(spell_warl_shadow_ward);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.68% from sp bonus
            float bonus = 0.8068f;

            bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateSpellpowerCoefficientLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_shadow_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 63108 - Siphon Life
class spell_warl_siphon_life : public AuraScript
{
    PrepareAuraScript(spell_warl_siphon_life);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SIPHON_LIFE_HEAL,
            SPELL_WARLOCK_GLYPH_OF_SIPHON_LIFE
        });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return false;

        return GetTarget()->IsAlive();
    }

    void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 amount = CalculatePct(static_cast<int32>(eventInfo.GetDamageInfo()->GetDamage()), aurEff->GetAmount());
        // Glyph of Siphon Life
        if (AuraEffect const* glyph = GetTarget()->GetAuraEffect(SPELL_WARLOCK_GLYPH_OF_SIPHON_LIFE, EFFECT_0))
            AddPct(amount, glyph->GetAmount());

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(amount);
        GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_SIPHON_LIFE_HEAL, args);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_warl_siphon_life::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_warl_siphon_life::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -30293 - Soul Leech
class spell_warl_soul_leech : public AuraScript
{
    PrepareAuraScript(spell_warl_soul_leech);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SOUL_LEECH_HEAL,
            SPELL_WARLOCK_IMP_SOUL_LEECH_R1,
            SPELL_WARLOCK_SOUL_LEECH_PET_MANA_1,
            SPELL_WARLOCK_SOUL_LEECH_PET_MANA_2,
            SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_1,
            SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_2,
            SPELL_REPLENISHMENT
        });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        static uint32 const casterMana[2] = { SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_1, SPELL_WARLOCK_SOUL_LEECH_CASTER_MANA_2 };
        static uint32 const petMana[2]    = { SPELL_WARLOCK_SOUL_LEECH_PET_MANA_1,    SPELL_WARLOCK_SOUL_LEECH_PET_MANA_2    };

        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* caster = eventInfo.GetActor();
        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
        args.AddSpellBP0(CalculatePct(damageInfo->GetDamage(), aurEff->GetAmount()));
        caster->CastSpell(caster, SPELL_WARLOCK_SOUL_LEECH_HEAL, args);

        // Improved Soul Leech code below
        AuraEffect const* impSoulLeech = GetTarget()->GetAuraEffectOfRankedSpell(SPELL_WARLOCK_IMP_SOUL_LEECH_R1, EFFECT_1, aurEff->GetCasterGUID());
        if (!impSoulLeech)
            return;

        uint8 impSoulLeechRank = impSoulLeech->GetSpellInfo()->GetRank();
        uint32 selfSpellId = casterMana[impSoulLeechRank - 1];
        uint32 petSpellId = petMana[impSoulLeechRank - 1];

        caster->CastSpell(nullptr, selfSpellId, aurEff);
        caster->CastSpell(nullptr, petSpellId, aurEff);

        if (roll_chance_i(impSoulLeech->GetAmount()))
            caster->CastSpell(nullptr, SPELL_REPLENISHMENT, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_soul_leech::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 29858 - Soulshatter
class spell_warl_soulshatter : public SpellScript
{
    PrepareSpellScript(spell_warl_soulshatter);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_SOULSHATTER_EFFECT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        if (Unit* target = GetHitUnit())
            if (target->GetThreatManager().IsThreatenedBy(caster, true))
                caster->CastSpell(target, SPELL_WARLOCK_SOULSHATTER_EFFECT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 37377 - Shadowflame
// 39437 - Shadowflame Hellfire and RoF
template <uint32 TriggerSpellId>
class spell_warl_t4_2p_bonus : public AuraScript
{
    PrepareAuraScript(spell_warl_t4_2p_bonus);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ TriggerSpellId });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->CastSpell(caster, TriggerSpellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_warl_t4_2p_bonus::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 1340024 - Unstable Affliction
class spell_warl_unstable_affliction : public AuraScript
{
    PrepareAuraScript(spell_warl_unstable_affliction);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL });
    }

    void HandleDispel(DispelInfo* dispelInfo)
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect const* aurEff = GetEffect(EFFECT_0))
                if (Unit* target = dispelInfo->GetDispeller()->ToUnit())
                    caster->CastSpell(target, SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL, true);
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_warl_unstable_affliction::HandleDispel);
    }
};

// Duskhaven
// 1350011 - Chaos Bolt
class spell_warl_chaos_bolt : public SpellScript
{
    PrepareSpellScript(spell_warl_chaos_bolt);

    void HandleBeforeCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARLOCK_CHAOS_INCARNATE_TALENT))
            {
                int32 roll = irand(1, 100);

                if (roll > 80)
                    caster->CastSpell(caster, SPELL_WARLOCK_CHAOS_INCARNATE_NO_CRIT, true);
                else if (roll > 60)
                    spellId = SPELL_WARLOCK_CHAOS_INCARNATE_DOT;
                else if (roll > 40)
                    spellId = SPELL_WARLOCK_CHAOS_INCARNATE_CHB25;
                else if (roll > 20)
                    spellId = SPELL_WARLOCK_CHAOS_INCARNATE_HEAL;
                else
                {
                    caster->CastSpell(caster, SPELL_WARLOCK_CHAOS_INCARNATE_DMG_BUFF, true);
                    spellId = SPELL_WARLOCK_CHAOS_INCARNATE_DMG_SELF;
                }
            }
    }

    void HandleOnHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            CastSpellExtraArgs args(effIndex);

            if (spellId = SPELL_WARLOCK_CHAOS_INCARNATE_DOT)
            {
                int32 pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CHAOS_INCARNATE_DOT)->GetEffect(EFFECT_1).CalcValue() / 3;
                args.AddSpellBP0(CalculatePct(GetHitDamage(), pct));
                caster->CastSpell(GetHitUnit(), spellId, args);
            }
            else if (spellId == SPELL_WARLOCK_CHAOS_INCARNATE_CHB25)
            {
                int32 pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CHAOS_INCARNATE_CHB25)->GetEffect(EFFECT_1).CalcValue();
                args.AddSpellBP0(CalculatePct(GetHitDamage(), pct));
                caster->CastSpell(GetHitUnit(), spellId, args);
            }
            else if (spellId == SPELL_WARLOCK_CHAOS_INCARNATE_HEAL)
            {
                int32 pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CHAOS_INCARNATE_HEAL)->GetEffect(EFFECT_1).CalcValue();
                args.AddSpellBP0(CalculatePct(GetHitDamage(), pct));
                caster->CastSpell(caster, spellId, args);
            }
            else if (spellId == SPELL_WARLOCK_CHAOS_INCARNATE_DMG_SELF)
            {
                int32 pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CHAOS_INCARNATE_DMG_BUFF)->GetEffect(EFFECT_1).CalcValue();
                args.AddSpellBP0(CalculatePct(GetHitDamage(), pct));
                caster->CastSpell(caster, spellId, args);
            }
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_warl_chaos_bolt::HandleBeforeCast);
        OnEffectHitTarget += SpellEffectFn(spell_warl_chaos_bolt::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    uint32 spellId = 0;
};

// 25311 - Corruption
class spell_warl_corruption : public SpellScript
{
    PrepareSpellScript(spell_warl_corruption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_UNSTABLE_AFFLICTION,
            SPELL_WARLOCK_UNSTABLE_AFFLICTION_DOT
        });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (caster->HasAura(SPELL_WARLOCK_UNSTABLE_AFFLICTION))
                    caster->CastSpell(target, SPELL_WARLOCK_UNSTABLE_AFFLICTION_DOT, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_corruption::HandleOnHit);
    }
};

// 1330018 - Demonbolt
class spell_warl_demonbolt : public SpellScript
{
    PrepareSpellScript(spell_warl_demonbolt);

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            if (Creature* grimSummon = caster->GetMap()->GetCreature(caster->m_SummonSlot[6]))
                switch (grimSummon->GetCreatureTemplate()->Entry)
                {
                    case 44037:
                    case 44039:
                        spellId = SPELL_WARLOCK_DEMONBOLT_FHVV;
                        break;
                    case 44041:
                        caster->CastSpell(caster, SPELL_WARLOCK_DEMONBOLT_FG, true);
                        break;
                    case 44043:
                    case 44045:
                        spellId = SPELL_WARLOCK_DEMONBOLT_DGI;
                    default:
                        break;
                }
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (spellId)
                    caster->CastSpell(target, spellId, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_demonbolt::HandleOnCast);
        OnHit += SpellHitFn(spell_warl_demonbolt::HandleOnHit);
    }

private:
    uint32 spellId = 0;
};

// 1330015 - Demonbolt (AoE)
class spell_warl_demonbolt_aoe : public SpellScript
{
    PrepareSpellScript(spell_warl_demonbolt_aoe);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_DEMONBOLT_DUMMY
        });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_WARLOCK_DEMONBOLT_DUMMY));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warl_demonbolt_aoe::RemoveInvalidTargets, EFFECT_ALL, TARGET_DEST_TARGET_ENEMY);
    }
};

// 1350029 - Felfire Bolt
class spell_warl_felfire_bolt : public SpellScript
{
    PrepareSpellScript(spell_warl_felfire_bolt);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_RAGING_FELFIRE
        });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
            {
                AuraApplication* aurApp = target->GetAuraApplication(SPELL_WARLOCK_IMMOLATE, caster->GetGUID());

                if (caster->HasAura(SPELL_WARLOCK_RAGING_FELFIRE) && aurApp)
                {
                    int32 bonusDuration = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_RAGING_FELFIRE)->GetEffect(EFFECT_1).CalcValue();
                    Aura* aura = aurApp->GetBase();
                    aura->SetDuration(aura->GetDuration() + bonusDuration);
                }
            }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_warl_felfire_bolt::HandleOnHit);
    }
};

// 1350028 - Felfire Bolts
class spell_warl_felfire_bolts_channel : public AuraScript
{
    PrepareAuraScript(spell_warl_felfire_bolts_channel);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_CHAOS_IN_MOTION_20,
            SPELL_WARLOCK_CHAOS_IN_MOTION_40,
            SPELL_WARLOCK_CHAOS_IN_MOTION_60,
            SPELL_WARLOCK_CHAOS_IN_MOTION_TALENT,
            SPELL_WARLOCK_FELFIRE_BOLTS_CHANNEL,
            SPELL_WARLOCK_FELFIRE_BOLTS_MISSILE,
            SPELL_WARLOCK_IMMOLATE
        });
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        Unit* caster = GetCaster();
        std::list<Unit*> targetList;
        float range = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELFIRE_BOLTS_CHANNEL)->GetEffect(EFFECT_0).CalcRadius();

        caster->SelectNearbyTargets(nullptr, range, 1);

        for (std::list<Unit*>::iterator tItr = targetList.begin(); tItr != targetList.end();)
        {
            if (!(*tItr)->GetAuraApplication(SPELL_WARLOCK_IMMOLATE, GetCasterGUID()))
                targetList.erase(tItr++);
            else
                ++tItr;
        }

        if (targetList.empty())
            return;

        Unit* target = Trinity::Containers::SelectRandomContainerElement(targetList);

        caster->CastSpell(target, SPELL_WARLOCK_FELFIRE_BOLTS_MISSILE, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARLOCK_CHAOS_IN_MOTION_TALENT))
            {
                int32 roll = irand(1, 60);

                if (roll > 40)
                    caster->CastSpell(caster, SPELL_WARLOCK_CHAOS_IN_MOTION_60, true);
                else if (roll > 20)
                    caster->CastSpell(caster, SPELL_WARLOCK_CHAOS_IN_MOTION_40, true);
                else
                    caster->CastSpell(caster, SPELL_WARLOCK_CHAOS_IN_MOTION_20, true);
            }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_felfire_bolts_channel::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_warl_felfire_bolts_channel::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_warl_felfire_bolts_cast : public SpellScript
{
    PrepareSpellScript(spell_warl_felfire_bolts_cast);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_FELFIRE_BOLTS_CHANNEL,
            SPELL_WARLOCK_IMMOLATE
        });
    }

    SpellCastResult CheckRequirement()
    {
        Unit* caster = GetCaster();
        std::list<Unit*> targetList;
        float range = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELFIRE_BOLTS_CHANNEL)->GetEffect(EFFECT_0).CalcRadius();

        caster->SelectNearbyTargets(nullptr, range, 1);

        for (std::list<Unit*>::iterator tItr = targetList.begin(); tItr != targetList.end();)
        {
            if (!(*tItr)->GetAuraApplication(SPELL_WARLOCK_IMMOLATE, caster->GetGUID()))
                targetList.erase(tItr++);
            else
                ++tItr;
        }

        if (targetList.empty())
        {
            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_IMMOLATE_NOT_ACTIVE);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_felfire_bolts_cast::CheckRequirement);
    }
};

// 1330031 - Grimoire: Felhunter
// 1330030 - Grimoire: Voidwalker
class spell_warl_grimoire_shadowy_assistance : public SpellScript
{
    PrepareSpellScript(spell_warl_grimoire_shadowy_assistance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SHADOWY_ASSISTANCE_PROC,
            SPELL_WARLOCK_SHADOWY_ASSISTANCE_TALENT
        });
    }

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARLOCK_SHADOWY_ASSISTANCE_TALENT))
                caster->CastSpell(caster, SPELL_WARLOCK_SHADOWY_ASSISTANCE_PROC, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_grimoire_shadowy_assistance::HandleOnCast);
    }
};

// 1330021 - Grimoire: Imp
// 1330022 - Grimoire: Succubus
class spell_warl_grimoire_burning_synergy : public SpellScript
{
    PrepareSpellScript(spell_warl_grimoire_burning_synergy);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_BURNING_SYNERGY_PROC,
            SPELL_WARLOCK_BURNING_SYNERGY_TALENT
        });
    }

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARLOCK_BURNING_SYNERGY_TALENT))
                caster->CastSpell(caster, SPELL_WARLOCK_BURNING_SYNERGY_PROC, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_grimoire_burning_synergy::HandleOnCast);
    }
};

// Summons - Remove Sacrificial Power
class spell_warl_remove_sacrificial_power : public SpellScript
{
    PrepareSpellScript(spell_warl_remove_sacrificial_power);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_WARLOCK_SACRIFICIAL_POWER
            });
    }

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_WARLOCK_SACRIFICIAL_POWER))
                caster->RemoveAura(SPELL_WARLOCK_SACRIFICIAL_POWER);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_warl_remove_sacrificial_power::HandleOnCast);
    }
};

// 1340004 - Sacrifice Demon
class spell_warl_sacrifice_demon : public SpellScript
{
    PrepareSpellScript(spell_warl_sacrifice_demon);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SACRIFICIAL_POWER
        });
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        Creature* summon = caster->GetMap()->GetCreature(caster->m_SummonSlot[0]);

        if (!summon)
            return SPELL_FAILED_NO_PET;

        return SPELL_CAST_OK;
    }

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_WARLOCK_SACRIFICIAL_POWER, true);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_warl_sacrifice_demon::CheckCast);
        OnCast += SpellCastFn(spell_warl_sacrifice_demon::HandleOnCast);
    }
};

// 1350007 - Shadowburn
class spell_warl_shadowburn : public SpellScript
{
    PrepareSpellScript(spell_warl_shadowburn);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SHADOWBURN_DUMMY
        });
    }

    void HandleOnHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
            {
                float pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SHADOWBURN_DUMMY)->GetEffect(EFFECT_0).CalcValue();

                if (target->GetHealthPct() <= pct)
                    caster->CastSpell(target, SPELL_WARLOCK_SHADOWBURN_DMG, true);
            }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_warl_shadowburn::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 1340027 - Soulrend
class spell_warl_soulrend : public AuraScript
{
    PrepareAuraScript(spell_warl_soulrend);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_WARLOCK_SOULREND
        });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        uint32 dots = GetTarget()->GetDoTsByCaster(GetCasterGUID());

        if (dots > 3)
            dots = 3;

        int32 pct = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SOULREND)->GetEffect(EFFECT_1).CalcValue() * dots;
        amount += CalculatePct(amount, pct);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warl_soulrend::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

void AddSC_warlock_spell_scripts()
{
    RegisterSpellScript(spell_warl_curse_of_agony);
    RegisterSpellScript(spell_warl_banish);
    new spell_warl_create_healthstone();
    RegisterSpellScript(spell_warl_curse_of_doom);
    RegisterSpellScript(spell_warl_decimation);
    RegisterSpellScript(spell_warl_demonic_circle_summon);
    RegisterSpellScript(spell_warl_demonic_circle_teleport);
    RegisterSpellScript(spell_warl_demonic_empowerment);
    RegisterSpellScript(spell_warl_demonic_pact);
    RegisterSpellScript(spell_warl_drain_soul);
    RegisterSpellScript(spell_warl_everlasting_affliction);
    RegisterSpellScript(spell_warl_fel_synergy);
    RegisterSpellScript(spell_warl_glyph_of_life_tap);
    RegisterSpellScript(spell_warl_glyph_of_shadowflame);
    RegisterSpellAndAuraScriptPair(spell_warl_haunt, spell_warl_haunt_aura);
    RegisterSpellScript(spell_warl_health_funnel);
    RegisterSpellScript(spell_warl_glyph_of_corruption_nightfall);
    RegisterSpellScript(spell_warl_life_tap);
    RegisterSpellScript(spell_warl_nether_protection);
    RegisterSpellScript(spell_warl_ritual_of_doom_effect);
    RegisterSpellScript(spell_warl_seduction);
    RegisterSpellScript(spell_warl_seed_of_corruption);
    RegisterSpellScript(spell_warl_seed_of_corruption_dummy);
    RegisterSpellScript(spell_warl_seed_of_corruption_generic);
    RegisterSpellScript(spell_warl_shadow_ward);
    RegisterSpellScript(spell_warl_siphon_life);
    RegisterSpellScript(spell_warl_soul_leech);
    RegisterSpellScript(spell_warl_soulshatter);
    RegisterSpellScriptWithArgs(spell_warl_t4_2p_bonus<SPELL_WARLOCK_FLAMESHADOW>, "spell_warl_t4_2p_bonus_shadow");
    RegisterSpellScriptWithArgs(spell_warl_t4_2p_bonus<SPELL_WARLOCK_SHADOWFLAME>, "spell_warl_t4_2p_bonus_fire");
    RegisterSpellScript(spell_warl_unstable_affliction);
    // Duskhaven
    // RegisterSpellScript(spell_warl_chaos_bolt);
    // RegisterSpellScript(spell_warl_corruption);
    // RegisterSpellScript(spell_warl_demonbolt);
    // RegisterSpellScript(spell_warl_demonbolt_aoe);
    // RegisterSpellScript(spell_warl_felfire_bolt);
    // RegisterSpellScript(spell_warl_felfire_bolts_channel);
    // RegisterSpellScript(spell_warl_felfire_bolts_cast);
    // RegisterSpellScript(spell_warl_grimoire_shadowy_assistance);
    // RegisterSpellScript(spell_warl_grimoire_burning_synergy);
    // RegisterSpellScript(spell_warl_remove_sacrificial_power);
    // RegisterSpellScript(spell_warl_sacrifice_demon);
    // RegisterSpellScript(spell_warl_shadowburn);
    // RegisterSpellScript(spell_warl_soulrend);
}
