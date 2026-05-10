-- Cleanup for DBErrors phase 2, groups 1-3:
-- 1. Creature model/display errors.
-- 2. Pool child references to removed pools.
-- 3. Creature spawn/template bad references.
--
-- This project uses generated .dest data. Do not USE the .dest schema here:
-- the SQL update runner registers this file in the connection database.

-- ---------------------------------------------------------------------------
-- 1. Creature model/display cleanup.
-- ---------------------------------------------------------------------------

-- Clear creature_template model slots that point at known missing client display IDs.
-- Do not touch generated/custom 150k display IDs; those are used by the project.
UPDATE `default.dataset.world.dest`.`creature_template`
SET `modelid1` = 0
WHERE `modelid1` IN (1257,4245,7010,13131,13150,15261,16542);

UPDATE `default.dataset.world.dest`.`creature_template`
SET `modelid2` = 0
WHERE `modelid2` IN (1257,4245,7010,13131,13150,15261,16542);

UPDATE `default.dataset.world.dest`.`creature_template`
SET `modelid3` = 0
WHERE `modelid3` IN (1257,4245,7010,13131,13150,15261,16542);

UPDATE `default.dataset.world.dest`.`creature_template`
SET `modelid4` = 0
WHERE `modelid4` IN (1257,4245,7010,13131,13150,15261,16542);

-- Some generated/custom creature templates only had the invalid model. Give
-- those a known-valid fallback display instead of leaving them with no model.
UPDATE `default.dataset.world.dest`.`creature_template`
SET `modelid1` = 49
WHERE `modelid1` = 0
  AND `modelid2` = 0
  AND `modelid3` = 0
  AND `modelid4` = 0
  AND (
       `entry` BETWEEN 45560 AND 47127
    OR `entry` IN (3339,3651,7826,11446,13085,14387,15103,16979)
  );

-- ---------------------------------------------------------------------------
-- 2. Pool cleanup.
-- ---------------------------------------------------------------------------

-- In .dest, pool_pool is stored in pool_members with type = 2:
-- spawnId = child pool, poolSpawnId = mother pool.
DELETE pm
FROM `default.dataset.world.dest`.`pool_members` pm
LEFT JOIN `default.dataset.world.dest`.`pool_template` child_pool
  ON child_pool.`entry` = pm.`spawnId`
WHERE pm.`type` = 2
  AND child_pool.`entry` IS NULL;

DELETE pm
FROM `default.dataset.world.dest`.`pool_members` pm
LEFT JOIN `default.dataset.world.dest`.`pool_template` mother_pool
  ON mother_pool.`entry` = pm.`poolSpawnId`
WHERE pm.`type` = 2
  AND mother_pool.`entry` IS NULL;

DELETE FROM `default.dataset.world.dest`.`pool_members`
WHERE `type` = 2
  AND `spawnId` = `poolSpawnId`;

UPDATE `default.dataset.world.dest`.`pool_members`
SET `chance` = 0
WHERE `type` = 2
  AND (`chance` < 0 OR `chance` > 100);

-- ---------------------------------------------------------------------------
-- 3. Creature spawn/template reference cleanup.
-- ---------------------------------------------------------------------------

DROP TEMPORARY TABLE IF EXISTS tmp_bad_creature_guids;
CREATE TEMPORARY TABLE tmp_bad_creature_guids (
  guid INT UNSIGNED NOT NULL PRIMARY KEY
) ENGINE=Memory;

INSERT IGNORE INTO tmp_bad_creature_guids (guid)
SELECT cr.`guid`
FROM `default.dataset.world.dest`.`creature` cr
LEFT JOIN `default.dataset.world.dest`.`creature_template` ct ON ct.`entry` = cr.`id`
WHERE ct.`entry` IS NULL;

-- Remove side rows owned by creature spawns whose template no longer exists.
DELETE ca
FROM `default.dataset.world.dest`.`creature_addon` ca
JOIN tmp_bad_creature_guids bad ON bad.guid = ca.`guid`;

DELETE cmo
FROM `default.dataset.world.dest`.`creature_movement_override` cmo
JOIN tmp_bad_creature_guids bad ON bad.guid = cmo.`SpawnId`;

DELETE cf
FROM `default.dataset.world.dest`.`creature_formations` cf
JOIN tmp_bad_creature_guids bad
  ON bad.guid = cf.`leaderGUID`
  OR bad.guid = cf.`memberGUID`;

DELETE lr
FROM `default.dataset.world.dest`.`linked_respawn` lr
JOIN tmp_bad_creature_guids bad
  ON (lr.`guid` = bad.guid AND lr.`linkType` IN (0, 1))
  OR (lr.`linkedGuid` = bad.guid AND lr.`linkType` IN (0, 3));

DELETE gec
FROM `default.dataset.world.dest`.`game_event_creature` gec
JOIN tmp_bad_creature_guids bad ON bad.guid = gec.`guid`;

DELETE gem
FROM `default.dataset.world.dest`.`game_event_model_equip` gem
JOIN tmp_bad_creature_guids bad ON bad.guid = gem.`guid`;

DELETE genv
FROM `default.dataset.world.dest`.`game_event_npc_vendor` genv
JOIN tmp_bad_creature_guids bad ON bad.guid = genv.`guid`;

DELETE genf
FROM `default.dataset.world.dest`.`game_event_npcflag` genf
JOIN tmp_bad_creature_guids bad ON bad.guid = genf.`guid`;

DELETE va
FROM `default.dataset.world.dest`.`vehicle_accessory` va
JOIN tmp_bad_creature_guids bad ON bad.guid = va.`guid`;

DELETE sg
FROM `default.dataset.world.dest`.`spawn_group` sg
JOIN tmp_bad_creature_guids bad ON bad.guid = sg.`spawnId`
WHERE sg.`spawnType` = 0;

DELETE pm
FROM `default.dataset.world.dest`.`pool_members` pm
JOIN tmp_bad_creature_guids bad ON bad.guid = pm.`spawnId`
WHERE pm.`type` = 0;

DELETE ibc
FROM `default.dataset.world.dest`.`instance_boss_creature` ibc
JOIN tmp_bad_creature_guids bad ON bad.guid = ibc.`guid`;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN tmp_bad_creature_guids bad
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = -bad.guid
 AND cnd.`SourceId` = 0;

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_bad_creature_guids bad
  ON ss.`source_type` = 0
 AND ss.`entryorguid` = -bad.guid;

DELETE cr
FROM `default.dataset.world.dest`.`creature` cr
JOIN tmp_bad_creature_guids bad ON bad.guid = cr.`guid`;

DROP TEMPORARY TABLE IF EXISTS tmp_bad_creature_guids;

-- Bad equipment IDs are non-fatal because the core sets them to no equipment
-- at load. Persist that correction.
UPDATE `default.dataset.world.dest`.`creature` cr
LEFT JOIN `default.dataset.world.dest`.`creature_equip_template` cet
  ON cet.`CreatureID` = cr.`id`
 AND cet.`ID` = cr.`equipment_id`
SET cr.`equipment_id` = 0
WHERE cr.`equipment_id` <> 0
  AND cet.`CreatureID` IS NULL;
