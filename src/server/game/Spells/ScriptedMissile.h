/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#ifndef TRINITY_SCRIPTED_MISSILE_H
#define TRINITY_SCRIPTED_MISSILE_H

#include "Define.h"

class Spell;
class SpellEffectInfo;

namespace ScriptedMissile
{
    bool CreateFromSpell(Spell* spell, SpellEffectInfo const& effectInfo);
}

#endif
