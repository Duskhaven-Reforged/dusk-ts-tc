/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include "ScriptedMissile.h"
#include "CellImpl.h"
#include "Creature.h"
#include "DBCStores.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Log.h"
#include "Map.h"
#include "MoveSplineInit.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "TemporarySummon.h"
#include "TSCustomPacket.h"
#include "TSEvents.h"
#include "TSWorldObject.h"
#include "Unit.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>
#include <list>
#include <unordered_map>
#include <vector>

namespace
{
    constexpr float TwoPi = float(M_PI) * 2.0f;
    constexpr uint32 DefaultTickMs = 50;
    constexpr uint32 DefaultDurationMs = 15000;
    constexpr opcode_t ScriptedMissileVisualSpawnOpcode = 0x7A20;
    constexpr opcode_t ScriptedMissileVisualDespawnOpcode = 0x7A21;
    constexpr opcode_t ScriptedMissileVisualStopOpcode = 0x7A22;
    constexpr opcode_t ScriptedMissileClientFacingOpcode = 0x7A23;
    std::atomic<uint32> ScriptedMissileVisualInstanceCounter = 1;
    std::unordered_map<uint64, float> ScriptedMissileClientFacingByGuid;

    enum class MotionType : uint8
    {
        Line,
        Orbit,
        Spiral,
        SineWave,
        Homing
    };

    enum class UnitCollisionMode : int32
    {
        HostileCreatures = 0,
        HostileUnits = 1,
        AnyCreature = 2,
        AnyUnit = 3,
        None = 4
    };

    enum ScriptedMissileMotionFlags : int32
    {
        SCRIPTED_MISSILE_MOTION_FLAG_ANCHOR_TO_CASTER = 0x01,
        SCRIPTED_MISSILE_MOTION_FLAG_REVERSE_ODD = 0x02,
        SCRIPTED_MISSILE_MOTION_FLAG_DESPAWN_ON_TERRAIN = 0x04,
        SCRIPTED_MISSILE_MOTION_FLAG_STOP_ON_COLLISION = 0x08
    };

    constexpr uint32 SCRIPTED_MISSILE_CLIENT_FLAG_USE_CASTER_ORIENTATION = 0x80000000u;

    struct ScriptedMissileProfile
    {
        MotionType Motion = MotionType::Line;
        UnitCollisionMode UnitCollision = UnitCollisionMode::HostileCreatures;
        uint32 Count = 1;
        uint32 DurationMs = DefaultDurationMs;
        uint32 TickMs = DefaultTickMs;
        uint32 VisualCreatureEntry = 0;
        uint32 ImpactSpellId = 0;
        uint32 VisualFlags = 0;
        int32 AttachMode = 0;
        int32 AttachPoint = 0;
        float CollisionRadius = 0.6f;
        float Radius = 0.0f;
        float RadiusVelocity = 0.0f;
        float StartDistance = 0.0f;
        float Height = 1.25f;
        float VerticalVelocity = 0.0f;
        float ForwardSpeed = 0.0f;
        float AngularSpeed = TwoPi;
        float SineAmplitude = 0.0f;
        float SineFrequency = 1.0f;
        float VisualScale = 1.0f;
        char const* ModelPath = nullptr;
        bool AnchorToCaster = false;
        bool ReverseOddMissiles = false;
        bool DespawnOnTerrain = true;
        bool StopOnCollision = false;
        bool HasClientVisuals = false;
        bool UseCasterClientOrientation = false;

        uint32 FlagsForClient() const
        {
            uint32 flags = 0;
            if (AnchorToCaster)
                flags |= SCRIPTED_MISSILE_MOTION_FLAG_ANCHOR_TO_CASTER;
            if (ReverseOddMissiles)
                flags |= SCRIPTED_MISSILE_MOTION_FLAG_REVERSE_ODD;
            if (DespawnOnTerrain)
                flags |= SCRIPTED_MISSILE_MOTION_FLAG_DESPAWN_ON_TERRAIN;
            if (StopOnCollision)
                flags |= SCRIPTED_MISSILE_MOTION_FLAG_STOP_ON_COLLISION;
            if (UseCasterClientOrientation)
                flags |= SCRIPTED_MISSILE_CLIENT_FLAG_USE_CASTER_ORIENTATION;
            return flags;
        }
    };

    struct MissileState
    {
        Position Previous;
        Position Current;
        ObjectGuid VisualGuid;
        bool Active = true;
        bool Stopped = false;
    };

    Position MakePosition(float x, float y, float z, float o)
    {
        return Position(x, y, z, o);
    }

    float DistancePointToSegment2dSq(Position const& point, Position const& from, Position const& to, float* closestZ = nullptr)
    {
        float const vx = to.GetPositionX() - from.GetPositionX();
        float const vy = to.GetPositionY() - from.GetPositionY();
        float const vz = to.GetPositionZ() - from.GetPositionZ();
        float const wx = point.GetPositionX() - from.GetPositionX();
        float const wy = point.GetPositionY() - from.GetPositionY();

        float const lengthSq = vx * vx + vy * vy;
        float t = lengthSq > 0.0001f ? (wx * vx + wy * vy) / lengthSq : 0.0f;
        t = std::clamp(t, 0.0f, 1.0f);

        float const cx = from.GetPositionX() + vx * t;
        float const cy = from.GetPositionY() + vy * t;
        if (closestZ)
            *closestZ = from.GetPositionZ() + vz * t;

        float const dx = point.GetPositionX() - cx;
        float const dy = point.GetPositionY() - cy;
        return dx * dx + dy * dy;
    }

    bool GetProfile(uint32 profileId, SpellEffectInfo const& effectInfo, int32 spellDuration, ScriptedMissileProfile& profile)
    {
        ScriptedMissileMotionEntry const* motion = sScriptedMissileMotionStore.LookupEntry(profileId);
        if (!motion)
            return false;

        profile.VisualCreatureEntry = uint32(std::max<int32>(effectInfo.MiscValueB, 0));
        profile.ImpactSpellId = effectInfo.TriggerSpell;
        profile.Motion = MotionType(std::clamp<int32>(motion->Motion, int32(MotionType::Line), int32(MotionType::Homing)));
        profile.Count = uint32(std::max<int32>(motion->Count, 1));
        profile.DurationMs = motion->DurationMs > 0 ? uint32(motion->DurationMs) : (spellDuration > 0 ? uint32(spellDuration) : DefaultDurationMs);
        profile.TickMs = motion->TickMs > 0 ? uint32(motion->TickMs) : DefaultTickMs;
        profile.CollisionRadius = std::max(0.0f, motion->CollisionRadius);
        profile.Radius = motion->Radius;
        profile.RadiusVelocity = motion->RadiusVelocity;
        profile.StartDistance = std::max(0.0f, motion->StartDistance);
        profile.Height = motion->Height;
        profile.VerticalVelocity = motion->VerticalVelocity;
        profile.ForwardSpeed = motion->ForwardSpeed;
        profile.AngularSpeed = motion->AngularSpeed;
        profile.SineAmplitude = motion->SineAmplitude;
        profile.SineFrequency = motion->SineFrequency;
        profile.ModelPath = motion->ModelPath;
        profile.VisualFlags = uint32(std::max<int32>(motion->VisualFlags, 0));
        profile.AttachMode = motion->AttachMode;
        profile.AttachPoint = motion->AttachPoint;
        profile.VisualScale = motion->VisualScale > 0.0f ? motion->VisualScale : 1.0f;
        profile.HasClientVisuals = motion->ModelPath && motion->ModelPath[0];
        profile.AnchorToCaster = (motion->Flags & SCRIPTED_MISSILE_MOTION_FLAG_ANCHOR_TO_CASTER) != 0;
        profile.ReverseOddMissiles = (motion->Flags & SCRIPTED_MISSILE_MOTION_FLAG_REVERSE_ODD) != 0;
        profile.DespawnOnTerrain = (motion->Flags & SCRIPTED_MISSILE_MOTION_FLAG_DESPAWN_ON_TERRAIN) != 0;
        profile.StopOnCollision = (motion->Flags & SCRIPTED_MISSILE_MOTION_FLAG_STOP_ON_COLLISION) != 0;

        if (effectInfo.BasePoints >= int32(UnitCollisionMode::HostileCreatures) && effectInfo.BasePoints <= int32(UnitCollisionMode::None))
            profile.UnitCollision = UnitCollisionMode(effectInfo.BasePoints);

        return true;
    }

    bool UsesDestinationAsOrigin(SpellEffectInfo const& effectInfo)
    {
        return effectInfo.TargetA.GetTarget() == TARGET_DEST_DEST
            || effectInfo.TargetB.GetTarget() == TARGET_DEST_DEST;
    }

    struct ScriptedMissileClientFacingListener
    {
        ScriptedMissileClientFacingListener()
        {
            ts_events.CustomPacket.OnReceive([](TSNumber<uint32> opcode, TSPacketRead packet, TSPlayer player)
            {
                if (uint32(opcode) != ScriptedMissileClientFacingOpcode)
                    return;

                if (!player.player)
                    return;

                float const facing = packet->ReadFloat(player.player->GetOrientation());
                if (!std::isfinite(facing))
                    return;

                ScriptedMissileClientFacingByGuid[player.player->GetGUID().GetRawValue()] = Position::NormalizeOrientation(facing);
            });
        }
    } ScriptedMissileClientFacingListenerInstance;

    class ScriptedMissileEvent : public BasicEvent
    {
    public:
        ScriptedMissileEvent(Unit* caster, Position const& origin, Position const& dest, float direction, ScriptedMissileProfile const& profile)
            : _caster(caster), _origin(origin), _dest(dest), _profile(profile)
        {
            _direction = direction;
            _missiles.resize(_profile.Count);
            _visualInstanceId = ScriptedMissileVisualInstanceCounter.fetch_add(1, std::memory_order_relaxed);
            if (!_visualInstanceId)
                _visualInstanceId = ScriptedMissileVisualInstanceCounter.fetch_add(1, std::memory_order_relaxed);

            for (uint32 i = 0; i < _profile.Count; ++i)
            {
                _missiles[i].Current = Evaluate(i, 0.0f);
                _missiles[i].Previous = _missiles[i].Current;
            }
        }

        ~ScriptedMissileEvent() override
        {
            DespawnAllVisuals();
            SendClientVisualDespawn(std::numeric_limits<uint32>::max());
        }

        bool Execute(uint64 eTime, uint32 /*pTime*/) override
        {
            if (!_caster || !_caster->IsInWorld() || !_caster->IsAlive())
                return true;

            if (!_startTime)
            {
                _startTime = eTime;
                SendClientVisualSpawn();
            }

            uint32 elapsedMs = uint32(eTime - _startTime);
            float elapsedSeconds = float(elapsedMs) / 1000.0f;

            if (elapsedMs >= _profile.DurationMs)
                return true;

            bool anyActive = false;
            for (uint32 i = 0; i < _missiles.size(); ++i)
            {
                MissileState& missile = _missiles[i];
                if (!missile.Active)
                    continue;

                anyActive = true;
                if (missile.Stopped)
                    continue;

                missile.Previous = missile.Current;
                missile.Current = Evaluate(i, elapsedSeconds);

                UpdateVisual(missile, i);

                if (elapsedMs <= _profile.TickMs)
                    continue;

                if (!PassesTerrain(missile.Previous, missile.Current))
                {
                    StopOrDespawn(missile, i);
                    continue;
                }

                if (Unit* hit = FindHitUnit(missile.Previous, missile.Current, missile.VisualGuid))
                {
                    Impact(hit);
                    StopOrDespawn(missile, i);
                    continue;
                }
            }

            if (!anyActive)
                return true;

            _caster->m_Events.AddEvent(this, Milliseconds(eTime + _profile.TickMs), false);
            return false;
        }

    private:
        Position Evaluate(uint32 missileIndex, float elapsedSeconds) const
        {
            Position center = _origin;
            if (_profile.AnchorToCaster && _caster && _caster->IsInWorld())
                center.Relocate(_caster);

            float const indexPhase = _profile.Count > 0 ? TwoPi * float(missileIndex) / float(_profile.Count) : 0.0f;
            float directionSign = (_profile.ReverseOddMissiles && (missileIndex & 1)) ? -1.0f : 1.0f;
            float const baseAngle = _direction + indexPhase;
            float const angle = baseAngle + _profile.AngularSpeed * elapsedSeconds * directionSign;
            float const radius = std::max(0.0f, _profile.Radius + _profile.RadiusVelocity * elapsedSeconds);
            float const forward = _profile.StartDistance + _profile.ForwardSpeed * elapsedSeconds;
            float const z = center.GetPositionZ() + _profile.Height + _profile.VerticalVelocity * elapsedSeconds;
            float x = center.GetPositionX();
            float y = center.GetPositionY();

            switch (_profile.Motion)
            {
                case MotionType::Line:
                    x += std::cos(baseAngle) * forward;
                    y += std::sin(baseAngle) * forward;
                    break;
                case MotionType::Orbit:
                    x += std::cos(angle) * radius;
                    y += std::sin(angle) * radius;
                    break;
                case MotionType::Spiral:
                    x += std::cos(baseAngle) * forward + std::cos(angle) * radius;
                    y += std::sin(baseAngle) * forward + std::sin(angle) * radius;
                    break;
                case MotionType::SineWave:
                {
                    float const right = baseAngle + float(M_PI) * 0.5f;
                    float const wave = std::sin(elapsedSeconds * _profile.SineFrequency * TwoPi + indexPhase) * _profile.SineAmplitude;
                    x += std::cos(baseAngle) * forward + std::cos(right) * wave;
                    y += std::sin(baseAngle) * forward + std::sin(right) * wave;
                    break;
                }
                case MotionType::Homing:
                {
                    float const targetAngle = center.GetAbsoluteAngle(&_dest);
                    x += std::cos(targetAngle) * forward;
                    y += std::sin(targetAngle) * forward;
                    break;
                }
            }

            return MakePosition(x, y, z, angle);
        }

        bool PassesTerrain(Position const& from, Position const& to) const
        {
            if (!_profile.DespawnOnTerrain && !_profile.StopOnCollision)
                return true;

            return _caster->GetMap()->isInLineOfSight(
                from.GetPositionX(), from.GetPositionY(), from.GetPositionZ(),
                to.GetPositionX(), to.GetPositionY(), to.GetPositionZ(),
                _caster->GetPhaseMask(), LINEOFSIGHT_ALL_CHECKS, VMAP::ModelIgnoreFlags::Nothing);
        }

        Unit* FindHitUnit(Position const& from, Position const& to, ObjectGuid visualGuid) const
        {
            if (_profile.UnitCollision == UnitCollisionMode::None)
                return nullptr;

            float const searchRadius = std::max(_origin.GetExactDist2d(&to), _profile.Radius) + _profile.CollisionRadius + 5.0f;
            std::list<Unit*> nearby;
            Trinity::AnyUnitInObjectRangeCheck check(_caster, searchRadius);
            Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(_caster, nearby, check);
            Cell::VisitAllObjects(_caster, searcher, searchRadius);

            Unit* best = nullptr;
            float bestDistSq = std::numeric_limits<float>::max();

            for (Unit* unit : nearby)
            {
                if (!unit || unit == _caster || unit->GetGUID() == visualGuid)
                    continue;

                if (Creature const* creature = unit->ToCreature())
                    if (_profile.VisualCreatureEntry && creature->GetEntry() == _profile.VisualCreatureEntry)
                        continue;

                switch (_profile.UnitCollision)
                {
                    case UnitCollisionMode::HostileCreatures:
                        if (unit->GetTypeId() != TYPEID_UNIT || !_caster->IsValidAttackTarget(unit))
                            continue;
                        break;
                    case UnitCollisionMode::HostileUnits:
                        if (!_caster->IsValidAttackTarget(unit))
                            continue;
                        break;
                    case UnitCollisionMode::AnyCreature:
                        if (unit->GetTypeId() != TYPEID_UNIT)
                            continue;
                        break;
                    case UnitCollisionMode::AnyUnit:
                        break;
                    case UnitCollisionMode::None:
                        continue;
                }

                if (!unit->IsAlive())
                    continue;

                float const radius = _profile.CollisionRadius + unit->GetCombatReach();
                float const hitDistSq = radius * radius;
                float closestZ = to.GetPositionZ();
                float const distSq = DistancePointToSegment2dSq(unit->GetPosition(), from, to, &closestZ);
                float const verticalAllowance = std::max(3.0f, radius + 1.0f);
                if (std::fabs(unit->GetPositionZ() - closestZ) > verticalAllowance)
                    continue;

                if (distSq <= hitDistSq && distSq < bestDistSq)
                {
                    best = unit;
                    bestDistSq = distSq;
                }
            }

            return best;
        }

        void Impact(Unit* target) const
        {
            if (_profile.ImpactSpellId && _caster->IsValidAttackTarget(target))
                _caster->CastSpell(target, _profile.ImpactSpellId, CastSpellExtraArgs(TRIGGERED_FULL_MASK));
        }

        void UpdateVisual(MissileState& missile, uint32 /*missileIndex*/)
        {
            if (_profile.HasClientVisuals)
                return;

            if (!_profile.VisualCreatureEntry)
                return;

            Creature* visual = nullptr;
            if (missile.VisualGuid)
                visual = _caster->GetMap()->GetCreature(missile.VisualGuid);

            if (!visual)
            {
                visual = _caster->SummonCreature(_profile.VisualCreatureEntry, missile.Current, TEMPSUMMON_MANUAL_DESPAWN, Milliseconds(_profile.DurationMs));
                if (!visual)
                    return;

                visual->SetReactState(REACT_PASSIVE);
                visual->SetImmuneToAll(true);
                visual->SetCanFly(true);
                visual->SetDisableGravity(true);
                missile.VisualGuid = visual->GetGUID();
            }

            missile.Current.SetOrientation(_caster->GetPosition().GetAbsoluteAngle(&missile.Current) + float(M_PI));

            float const dx = missile.Current.GetPositionX() - visual->GetPositionX();
            float const dy = missile.Current.GetPositionY() - visual->GetPositionY();
            float const dz = missile.Current.GetPositionZ() - visual->GetPositionZ();
            float const distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < 0.05f)
            {
                visual->UpdateOrientation(missile.Current.GetOrientation());
                return;
            }

            float const tickSeconds = std::max(0.001f, float(_profile.TickMs) / 1000.0f);
            Movement::MoveSplineInit init(visual);
            init.MoveTo(missile.Current.GetPositionX(), missile.Current.GetPositionY(), missile.Current.GetPositionZ(), false, true);
            init.SetFly();
            init.SetVelocity(std::max(0.1f, distance / tickSeconds));
            init.SetFacing(missile.Current.GetOrientation());
            init.Launch();
        }

        void DespawnVisual(MissileState& missile) const
        {
            if (!missile.VisualGuid || !_caster || !_caster->IsInWorld())
                return;

            if (Creature* visual = _caster->GetMap()->GetCreature(missile.VisualGuid))
                visual->DespawnOrUnsummon();

            missile.VisualGuid.Clear();
        }

        void StopOrDespawn(MissileState& missile, uint32 missileIndex) const
        {
            if (_profile.StopOnCollision)
            {
                missile.Stopped = true;
                SendClientVisualStop(missileIndex, missile.Current);
                return;
            }

            DespawnVisual(missile);
            SendClientVisualDespawn(missileIndex);
            missile.Active = false;
        }

        void DespawnAllVisuals() const
        {
            for (MissileState const& missile : _missiles)
                if (missile.VisualGuid && _caster && _caster->IsInWorld())
                    if (Creature* visual = _caster->GetMap()->GetCreature(missile.VisualGuid))
                        visual->DespawnOrUnsummon();
        }

        void SendClientVisualSpawn() const
        {
            if (!_profile.HasClientVisuals || !_caster || !_caster->IsInWorld())
                return;

            TSPacketWrite packet = CreateCustomPacket(ScriptedMissileVisualSpawnOpcode, 76);
            packet->WriteUInt32(_visualInstanceId);
            packet->WriteUInt32(uint32(_profile.Motion));
            packet->WriteUInt32(_profile.Count);
            packet->WriteUInt32(_profile.DurationMs);
            packet->WriteUInt32(_profile.FlagsForClient());
            packet->WriteUInt32(_profile.VisualFlags);
            packet->WriteInt32(_profile.AttachMode);
            packet->WriteInt32(_profile.AttachPoint);
            packet->WriteUInt64(_caster->GetGUID().GetRawValue());
            packet->WriteUInt64(0);
            packet->WriteFloat(_origin.GetPositionX());
            packet->WriteFloat(_origin.GetPositionY());
            packet->WriteFloat(_origin.GetPositionZ());
            packet->WriteFloat(_origin.GetOrientation());
            packet->WriteFloat(_dest.GetPositionX());
            packet->WriteFloat(_dest.GetPositionY());
            packet->WriteFloat(_dest.GetPositionZ());
            packet->WriteFloat(_direction);
            packet->WriteFloat(_profile.Radius);
            packet->WriteFloat(_profile.RadiusVelocity);
            packet->WriteFloat(_profile.StartDistance);
            packet->WriteFloat(_profile.Height);
            packet->WriteFloat(_profile.VerticalVelocity);
            packet->WriteFloat(_profile.ForwardSpeed);
            packet->WriteFloat(_profile.AngularSpeed);
            packet->WriteFloat(_profile.SineAmplitude);
            packet->WriteFloat(_profile.SineFrequency);
            packet->WriteFloat(_profile.VisualScale);
            packet->WriteString(_profile.ModelPath ? _profile.ModelPath : "");
            packet->BroadcastAround(TSWorldObject(_caster), 160.0f, true);
        }

        void SendClientVisualDespawn(uint32 missileIndex) const
        {
            if (!_profile.HasClientVisuals || !_caster || !_caster->IsInWorld() || !_visualInstanceId)
                return;

            TSPacketWrite packet = CreateCustomPacket(ScriptedMissileVisualDespawnOpcode, 8);
            packet->WriteUInt32(_visualInstanceId);
            packet->WriteUInt32(missileIndex);
            packet->BroadcastAround(TSWorldObject(_caster), 160.0f, true);
        }

        void SendClientVisualStop(uint32 missileIndex, Position const& position) const
        {
            if (!_profile.HasClientVisuals || !_caster || !_caster->IsInWorld() || !_visualInstanceId)
                return;

            TSPacketWrite packet = CreateCustomPacket(ScriptedMissileVisualStopOpcode, 20);
            packet->WriteUInt32(_visualInstanceId);
            packet->WriteUInt32(missileIndex);
            packet->WriteFloat(position.GetPositionX());
            packet->WriteFloat(position.GetPositionY());
            packet->WriteFloat(position.GetPositionZ());
            packet->BroadcastAround(TSWorldObject(_caster), 160.0f, true);
        }

        Unit* _caster;
        Position _origin;
        Position _dest;
        ScriptedMissileProfile _profile;
        std::vector<MissileState> _missiles;
        uint64 _startTime = 0;
        float _direction = 0.0f;
        uint32 _visualInstanceId = 0;
    };
}

bool ScriptedMissile::CreateFromSpell(Spell* spell, SpellEffectInfo const& effectInfo)
{
    if (!spell)
        return false;

    Unit* caster = spell->GetCaster()->ToUnit();
    if (!caster || !caster->IsInWorld())
        return false;

    uint32 const profileId = uint32(std::max<int32>(effectInfo.MiscValue, 0));
    if (!profileId)
        return false;

    Position origin = caster->GetPosition();
    Position dest = origin;
    float direction = caster->GetOrientation();
    origin.SetOrientation(direction);
    bool useCasterClientOrientation = false;
    bool hasExplicitDestination = false;
    bool const useDestinationAsOrigin = UsesDestinationAsOrigin(effectInfo);
    if (spell->m_targets.HasDst())
    {
        SpellDestination const* dst = spell->m_targets.GetDst();
        hasExplicitDestination = true;
        dest.Relocate(dst->_position);
        if (origin.GetExactDist2d(&dest) > 0.01f)
            direction = origin.GetAbsoluteAngle(&dest);
        else if (!useDestinationAsOrigin)
        {
            useCasterClientOrientation = true;
            dest.RelocateOffset(Position(8.0f, 0.0f, 0.0f, 0.0f));
        }
    }
    else
    {
        useCasterClientOrientation = true;
        dest.RelocateOffset(Position(8.0f, 0.0f, 0.0f, 0.0f));
    }

    if (useCasterClientOrientation)
    {
        auto const itr = ScriptedMissileClientFacingByGuid.find(caster->GetGUID().GetRawValue());
        if (itr != ScriptedMissileClientFacingByGuid.end())
        {
            direction = itr->second;
            origin.SetOrientation(direction);
            dest = origin;
            dest.RelocateOffset(Position(8.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    if (hasExplicitDestination && useDestinationAsOrigin)
    {
        origin.Relocate(dest);
        origin.SetOrientation(direction);
        dest = origin;
        dest.RelocateOffset(Position(8.0f, 0.0f, 0.0f, 0.0f));
        useCasterClientOrientation = false;
    }

    ScriptedMissileProfile profile;
    if (!GetProfile(profileId, effectInfo, spell->GetSpellInfo()->GetDuration(), profile))
    {
        TC_LOG_ERROR("spells", "ScriptedMissile: spell {} references missing ScriptedMissileMotion.cdbc row {}", spell->GetSpellInfo()->Id, profileId);
        return false;
    }

    profile.UseCasterClientOrientation = useCasterClientOrientation;
    caster->m_Events.AddEvent(new ScriptedMissileEvent(caster, origin, dest, direction, profile), caster->m_Events.CalculateTime(Milliseconds(1)));
    return true;
}
