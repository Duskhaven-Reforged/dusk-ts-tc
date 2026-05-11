-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.
UPDATE `creature_template` SET modelid1 = 0 WHERE entry = 3339;

UPDATE `creature_template` SET modelid1 = 0 WHERE entry = 7826;
UPDATE `creature_template` SET modelid1 = 0 WHERE entry = 14387;

UPDATE `creature_template` SET modelid1 = 0 WHERE entry = 13085;
UPDATE `creature_template` SET speed_walk = 1 WHERE entry = 795;

UPDATE `creature_template` SET speed_walk = 1 WHERE entry = 796;

UPDATE `creature_template` SET speed_walk = 1 WHERE entry = 797;

UPDATE `creature_template` SET type = 0 WHERE entry IN (47551, 47678, 47578, 47203, 47549, 45010, 47204);

UPDATE `creature_template` SET AIName = '' WHERE entry = 47270;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47272;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47271;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47266;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47265;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47264;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47263;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47262;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47261;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47260;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47259;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47258;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47257;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47303;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47304;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47305;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47306;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47281;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47307;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47251;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47254;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47252;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47277;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47276;

UPDATE `creature_template` SET ScriptName = '' WHERE entry = 47256;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47280;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47279;

UPDATE `creature_template` SET AIName = '' WHERE entry = 47267;

UPDATE `creature_template` diff JOIN `creature_template` base ON diff.entry IN (base.difficulty_entry_1, base.difficulty_entry_2, base.difficulty_entry_3) SET diff.faction = base.faction, diff.flags_extra = base.flags_extra, diff.unit_flags2 = base.unit_flags2 WHERE diff.entry IN (31505, 37622, 47300, 47306, 47277, 47256, 47280, 22768, 32107, 37428, 22557);

DELETE FROM `creature_template_spell` WHERE CreatureID = 15741 AND `Index` = 3;

DELETE FROM `creature_template_spell` WHERE CreatureID = 16163 AND `Index` = 2;

DELETE FROM `creature_template_spell` WHERE CreatureID = 16451 AND `Index` = 2;

DELETE FROM `creature_template_spell` WHERE CreatureID = 16994 AND `Index` = 3;

DELETE FROM `creature_template_spell` WHERE CreatureID = 19891 AND `Index` = 2;

DELETE FROM `creature_template_spell` WHERE CreatureID = 20300 AND `Index` = 3;

DELETE FROM `creature_template_spell` WHERE CreatureID = 21168 AND `Index` = 3;

DELETE FROM `creature_template_spell` WHERE CreatureID = 21874 AND `Index` = 3;

DELETE FROM `creature_template_spell` WHERE CreatureID = 22384 AND `Index` = 3;

UPDATE `creature_equip_template` SET ItemID1 = 0 WHERE CreatureID = 63201 AND ID = 3;

UPDATE `creature_equip_template` SET ItemID1 = 0 WHERE CreatureID = 9369 AND ID = 1;

UPDATE `creature_template_addon` SET auras = '' WHERE entry IN (619, 818, 881, 1344, 1729, 1732, 1768, 2018, 3204, 3218, 3892, 4299, 4303, 4460, 4488, 4606, 4805, 5915, 6747, 7158, 7438, 7440, 7505, 7506, 8409, 8914, 9024, 9038, 9039, 9397, 9499, 10426, 10508, 11347, 11672, 12902, 13142, 16029, 47257, 11577);

UPDATE `creature_addon` SET auras = '' WHERE guid IN (225013, 225012, 225011, 225010, 225009);

UPDATE `creature_addon` SET StandState = 0 WHERE guid = 225013;

UPDATE `creature_addon` SET StandState = 0 WHERE guid = 225012;

UPDATE `creature_addon` SET StandState = 0 WHERE guid = 225011;

UPDATE `creature_addon` SET StandState = 0 WHERE guid = 225010;

UPDATE `creature_addon` SET StandState = 0 WHERE guid = 225009;

UPDATE `creature` SET MovementType = 0 WHERE guid IN (6158, 74033, 67954, 27290, 27289);

UPDATE `creature_addon` SET path_id = 0 WHERE guid IN (6158, 74033, 67954, 27290, 27289);
