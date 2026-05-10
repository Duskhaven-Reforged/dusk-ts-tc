-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 45391;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 3339;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 7826;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 47505;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 47473;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 45417;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 14387;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 13085;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 47184;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 0 WHERE entry = 45000;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid3 = 0 WHERE entry = 47238;

UPDATE `default.dataset.world.dest`.`creature_template` SET modelid1 = 49 WHERE entry IN (45391, 47505, 47473, 45417, 47184, 45000) AND modelid1 = 0 AND modelid2 = 0 AND modelid3 = 0 AND modelid4 = 0;

UPDATE `default.dataset.world.dest`.`creature_template` SET speed_walk = 1 WHERE entry = 795;

UPDATE `default.dataset.world.dest`.`creature_template` SET speed_walk = 1 WHERE entry = 796;

UPDATE `default.dataset.world.dest`.`creature_template` SET speed_walk = 1 WHERE entry = 797;

UPDATE `default.dataset.world.dest`.`creature_template` SET type = 0 WHERE entry IN (47551, 47678, 47578, 47203, 47549, 45010, 47204);

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47270;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47272;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47271;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47266;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47265;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47264;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47263;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47262;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47261;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47260;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47259;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47258;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47257;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47303;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47304;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47305;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47306;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47281;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47307;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47251;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47254;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47252;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47277;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47276;

UPDATE `default.dataset.world.dest`.`creature_template` SET ScriptName = '' WHERE entry = 47256;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47280;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47279;

UPDATE `default.dataset.world.dest`.`creature_template` SET AIName = '' WHERE entry = 47267;

UPDATE `default.dataset.world.dest`.`creature_template` diff JOIN `default.dataset.world.dest`.`creature_template` base ON diff.entry IN (base.difficulty_entry_1, base.difficulty_entry_2, base.difficulty_entry_3) SET diff.faction = base.faction, diff.flags_extra = base.flags_extra, diff.unit_flags2 = base.unit_flags2 WHERE diff.entry IN (31505, 37622, 47300, 47306, 47277, 47256, 47280, 22768, 32107, 37428, 22557);

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 15741 AND `Index` = 3;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 16163 AND `Index` = 2;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 16451 AND `Index` = 2;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 16994 AND `Index` = 3;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 19891 AND `Index` = 2;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 20300 AND `Index` = 3;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 21168 AND `Index` = 3;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 21874 AND `Index` = 3;

DELETE FROM `default.dataset.world.dest`.`creature_template_spell` WHERE CreatureID = 22384 AND `Index` = 3;

UPDATE `default.dataset.world.dest`.`creature_equip_template` SET ItemID1 = 0 WHERE CreatureID = 63201 AND ID = 3;

UPDATE `default.dataset.world.dest`.`creature_equip_template` SET ItemID1 = 0 WHERE CreatureID = 9369 AND ID = 1;

UPDATE `default.dataset.world.dest`.`creature_template_addon` SET auras = '' WHERE entry IN (619, 818, 881, 1344, 1729, 1732, 1768, 2018, 3204, 3218, 3892, 4299, 4303, 4460, 4488, 4606, 4805, 5915, 6747, 7158, 7438, 7440, 7505, 7506, 8409, 8914, 9024, 9038, 9039, 9397, 9499, 10426, 10508, 11347, 11672, 12902, 13142, 16029, 47257, 11577);

UPDATE `default.dataset.world.dest`.`creature_addon` SET auras = '' WHERE guid IN (225013, 225012, 225011, 225010, 225009);

UPDATE `default.dataset.world.dest`.`creature_addon` SET StandState = 0 WHERE guid = 225013;

UPDATE `default.dataset.world.dest`.`creature_addon` SET StandState = 0 WHERE guid = 225012;

UPDATE `default.dataset.world.dest`.`creature_addon` SET StandState = 0 WHERE guid = 225011;

UPDATE `default.dataset.world.dest`.`creature_addon` SET StandState = 0 WHERE guid = 225010;

UPDATE `default.dataset.world.dest`.`creature_addon` SET StandState = 0 WHERE guid = 225009;

UPDATE `default.dataset.world.dest`.`creature` SET MovementType = 0 WHERE guid IN (6158, 74033, 67954, 27290, 27289);

UPDATE `default.dataset.world.dest`.`creature_addon` SET path_id = 0 WHERE guid IN (6158, 74033, 67954, 27290, 27289);
