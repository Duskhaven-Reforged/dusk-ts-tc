-- Manual world cleanup for removed client maps.
-- Target schema: default.dataset.world.dest
-- Target maps:
-- 249, 269, 309, 409, 469, 509, 531, 532, 533, 534, 540, 542, 543,
-- 544, 545, 546, 547, 550, 552, 555, 557, 560, 564, 568, 572, 580,
-- 585, 654, 729, 739
--
-- Scope:
-- * Delete creature/gameobject spawns on those maps.
-- * Delete spawn-owned side rows: addons, waypoints, formations, linked
--   respawns, spawn groups, game events, vehicle accessories, spawn SAI,
--   and SAI timed actionlists that are only called by deleted spawn SAI.
-- * Delete direct map references present in .dest: game_tele,
--   instance_template, instance_* custom tables, access_requirement,
--   quest POI, spell target positions, playercreateinfo, map_dbc.
-- * Keep creature_template, gameobject_template, loot templates, quest
--   templates, and starter/ender rows. Audit SELECTs below show fallout.
--
-- No START TRANSACTION/COMMIT here; the TSWoW/mysql runner may wrap this file.
-- Do not USE the target schema here. The update runner checks and writes its
-- `updates` row in the connection database; switching schemas makes the file
-- look unapplied and can make the appended registration INSERT hit a duplicate.
-- Fully qualify .dest tables instead.

DROP TEMPORARY TABLE IF EXISTS tmp_removed_maps;
CREATE TEMPORARY TABLE tmp_removed_maps (
  map SMALLINT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_removed_maps (map) VALUES
(249),(269),(309),(409),(469),(509),(531),(532),(533),(534),
(540),(542),(543),(544),(545),(546),(547),(550),(552),(555),
(557),(560),(564),(568),(572),(580),(585),(654),(729),(739);

DROP TEMPORARY TABLE IF EXISTS tmp_removed_creature_guids;
CREATE TEMPORARY TABLE tmp_removed_creature_guids (
  guid INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_removed_creature_guids (guid)
SELECT c.guid
FROM `default.dataset.world.dest`.`creature` c
JOIN tmp_removed_maps m ON m.map = c.map;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_creature_guids_linked;
CREATE TEMPORARY TABLE tmp_removed_creature_guids_linked (
  guid INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_removed_creature_guids_linked (guid)
SELECT guid FROM tmp_removed_creature_guids;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_creature_entries;
CREATE TEMPORARY TABLE tmp_removed_creature_entries (
  entry INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_removed_creature_entries (entry)
SELECT c.id
FROM `default.dataset.world.dest`.`creature` c
JOIN tmp_removed_maps m ON m.map = c.map
WHERE c.id <> 0;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_gameobject_guids;
CREATE TEMPORARY TABLE tmp_removed_gameobject_guids (
  guid INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_removed_gameobject_guids (guid)
SELECT g.guid
FROM `default.dataset.world.dest`.`gameobject` g
JOIN tmp_removed_maps m ON m.map = g.map;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_gameobject_guids_linked;
CREATE TEMPORARY TABLE tmp_removed_gameobject_guids_linked (
  guid INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_removed_gameobject_guids_linked (guid)
SELECT guid FROM tmp_removed_gameobject_guids;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_gameobject_entries;
CREATE TEMPORARY TABLE tmp_removed_gameobject_entries (
  entry INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_removed_gameobject_entries (entry)
SELECT g.id
FROM `default.dataset.world.dest`.`gameobject` g
JOIN tmp_removed_maps m ON m.map = g.map
WHERE g.id <> 0;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_creature_path_ids;
CREATE TEMPORARY TABLE tmp_removed_creature_path_ids (
  path_id INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_removed_creature_path_ids (path_id)
SELECT ca.path_id
FROM `default.dataset.world.dest`.`creature_addon` ca
JOIN tmp_removed_creature_guids rc ON rc.guid = ca.guid
WHERE ca.path_id <> 0;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_waypoint_script_ids;
CREATE TEMPORARY TABLE tmp_removed_waypoint_script_ids (
  id INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_removed_waypoint_script_ids (id)
SELECT wd.action
FROM `default.dataset.world.dest`.`waypoint_data` wd
JOIN tmp_removed_creature_path_ids rp ON rp.path_id = wd.id
WHERE wd.action <> 0;

DROP TEMPORARY TABLE IF EXISTS tmp_removed_spawn_group_ids;
CREATE TEMPORARY TABLE tmp_removed_spawn_group_ids (
  groupId INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_removed_spawn_group_ids (groupId)
SELECT sg.groupId
FROM `default.dataset.world.dest`.`spawn_group` sg
JOIN tmp_removed_creature_guids rc
  ON sg.spawnType = 0
 AND sg.spawnId = rc.guid;

INSERT IGNORE INTO tmp_removed_spawn_group_ids (groupId)
SELECT sg.groupId
FROM `default.dataset.world.dest`.`spawn_group` sg
JOIN tmp_removed_gameobject_guids rg
  ON sg.spawnType = 1
 AND sg.spawnId = rg.guid;

INSERT IGNORE INTO tmp_removed_spawn_group_ids (groupId)
SELECT isg.spawnGroupId
FROM `default.dataset.world.dest`.`instance_spawn_groups` isg
JOIN tmp_removed_maps m ON m.map = isg.instanceMapId;

DROP TEMPORARY TABLE IF EXISTS tmp_deleted_smart_scripts;
CREATE TEMPORARY TABLE tmp_deleted_smart_scripts (
  entryorguid INT NOT NULL,
  source_type TINYINT UNSIGNED NOT NULL,
  id SMALLINT UNSIGNED NOT NULL,
  link SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (entryorguid, source_type, id, link)
);

INSERT INTO tmp_deleted_smart_scripts (entryorguid, source_type, id, link)
SELECT ss.entryorguid, ss.source_type, ss.id, ss.link
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_removed_creature_guids rc
  ON ss.source_type = 0
 AND ss.entryorguid = -CAST(rc.guid AS SIGNED);

INSERT INTO tmp_deleted_smart_scripts (entryorguid, source_type, id, link)
SELECT ss.entryorguid, ss.source_type, ss.id, ss.link
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_removed_gameobject_guids rg
  ON ss.source_type = 1
 AND ss.entryorguid = -CAST(rg.guid AS SIGNED);

DROP TEMPORARY TABLE IF EXISTS tmp_deleted_smart_scripts_live;
CREATE TEMPORARY TABLE tmp_deleted_smart_scripts_live (
  entryorguid INT NOT NULL,
  source_type TINYINT UNSIGNED NOT NULL,
  id SMALLINT UNSIGNED NOT NULL,
  link SMALLINT UNSIGNED NOT NULL,
  PRIMARY KEY (entryorguid, source_type, id, link)
);

INSERT INTO tmp_deleted_smart_scripts_live (entryorguid, source_type, id, link)
SELECT entryorguid, source_type, id, link
FROM tmp_deleted_smart_scripts;

DROP TEMPORARY TABLE IF EXISTS tmp_seq;
CREATE TEMPORARY TABLE tmp_seq (
  n INT UNSIGNED NOT NULL PRIMARY KEY
);

INSERT INTO tmp_seq (n)
WITH RECURSIVE seq(n) AS (
  SELECT 0
  UNION ALL
  SELECT n + 1 FROM seq WHERE n < 255
)
SELECT n FROM seq;

DROP TEMPORARY TABLE IF EXISTS tmp_timed_actionlist_candidates;
CREATE TEMPORARY TABLE tmp_timed_actionlist_candidates (
  entry INT NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param1
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 80
  AND ss.action_param1 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param1
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param1 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param2
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param2 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param3
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param3 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param4
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param4 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param5
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param5 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param6
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
WHERE ss.action_type = 87 AND ss.action_param6 <> 0;

INSERT IGNORE INTO tmp_timed_actionlist_candidates (entry)
SELECT ss.action_param1 + seq.n
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link
JOIN tmp_seq seq ON ss.action_param1 + seq.n <= ss.action_param2
WHERE ss.action_type = 88
  AND ss.action_param1 <> 0
  AND ss.action_param2 >= ss.action_param1;

DROP TEMPORARY TABLE IF EXISTS tmp_timed_actionlist_candidates_live;
CREATE TEMPORARY TABLE tmp_timed_actionlist_candidates_live (
  entry INT NOT NULL PRIMARY KEY
);

INSERT INTO tmp_timed_actionlist_candidates_live (entry)
SELECT entry FROM tmp_timed_actionlist_candidates;

DROP TEMPORARY TABLE IF EXISTS tmp_deleted_timed_actionlists;
CREATE TEMPORARY TABLE tmp_deleted_timed_actionlists (
  entry INT NOT NULL PRIMARY KEY
);

INSERT IGNORE INTO tmp_deleted_timed_actionlists (entry)
SELECT c.entry
FROM tmp_timed_actionlist_candidates c
WHERE EXISTS (
  SELECT 1
  FROM `default.dataset.world.dest`.`smart_scripts` ss
  WHERE ss.source_type = 9
    AND ss.entryorguid = c.entry
)
AND NOT EXISTS (
  SELECT 1
  FROM `default.dataset.world.dest`.`smart_scripts` live
  LEFT JOIN tmp_deleted_smart_scripts_live dss
    ON dss.entryorguid = live.entryorguid
   AND dss.source_type = live.source_type
   AND dss.id = live.id
   AND dss.link = live.link
  LEFT JOIN tmp_timed_actionlist_candidates_live live_candidate
    ON live.source_type = 9
   AND live.entryorguid = live_candidate.entry
  WHERE dss.entryorguid IS NULL
    AND live_candidate.entry IS NULL
    AND (
      (live.action_type = 80 AND live.action_param1 = c.entry)
      OR (live.action_type = 87 AND c.entry IN (
        live.action_param1, live.action_param2, live.action_param3,
        live.action_param4, live.action_param5, live.action_param6
      ))
      OR (
        live.action_type = 88
        AND live.action_param1 <> 0
        AND live.action_param2 >= live.action_param1
        AND c.entry BETWEEN live.action_param1 AND live.action_param2
      )
    )
);

DROP TEMPORARY TABLE IF EXISTS tmp_removed_quest_poi;
CREATE TEMPORARY TABLE tmp_removed_quest_poi (
  QuestID INT UNSIGNED NOT NULL,
  id INT UNSIGNED NOT NULL,
  PRIMARY KEY (QuestID, id)
);

INSERT INTO tmp_removed_quest_poi (QuestID, id)
SELECT qp.QuestID, qp.id
FROM `default.dataset.world.dest`.`quest_poi` qp
JOIN tmp_removed_maps m ON m.map = qp.MapID;

-- Audit before deletes.
SELECT 'creature' AS table_name, COUNT(*) AS rows_to_delete FROM tmp_removed_creature_guids;
SELECT 'gameobject' AS table_name, COUNT(*) AS rows_to_delete FROM tmp_removed_gameobject_guids;
SELECT 'smart_scripts' AS table_name, COUNT(*) AS rows_to_delete FROM tmp_deleted_smart_scripts;
SELECT 'smart_scripts timed_actionlists' AS table_name, COUNT(*) AS rows_to_delete
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_timed_actionlists dt
  ON dt.entry = ss.entryorguid
 AND ss.source_type = 9;
SELECT 'quest_poi' AS table_name, COUNT(*) AS rows_to_delete FROM tmp_removed_quest_poi;

-- Quest/template fallout audit. These are not deleted by this file.
SELECT DISTINCT 'creature_queststarter' AS ref_type, cqs.quest, cqs.id AS entry
FROM `default.dataset.world.dest`.`creature_queststarter` cqs
JOIN tmp_removed_creature_entries rce ON rce.entry = cqs.id
ORDER BY cqs.quest, cqs.id;

SELECT DISTINCT 'creature_questender' AS ref_type, cqe.quest, cqe.id AS entry
FROM `default.dataset.world.dest`.`creature_questender` cqe
JOIN tmp_removed_creature_entries rce ON rce.entry = cqe.id
ORDER BY cqe.quest, cqe.id;

SELECT DISTINCT 'gameobject_queststarter' AS ref_type, gqs.quest, gqs.id AS entry
FROM `default.dataset.world.dest`.`gameobject_queststarter` gqs
JOIN tmp_removed_gameobject_entries rge ON rge.entry = gqs.id
ORDER BY gqs.quest, gqs.id;

SELECT DISTINCT 'gameobject_questender' AS ref_type, gqe.quest, gqe.id AS entry
FROM `default.dataset.world.dest`.`gameobject_questender` gqe
JOIN tmp_removed_gameobject_entries rge ON rge.entry = gqe.id
ORDER BY gqe.quest, gqe.id;

SELECT DISTINCT 'quest_objective_creature' AS ref_type, qt.ID AS quest, req.entry
FROM `default.dataset.world.dest`.`quest_template` qt
JOIN (
  SELECT ID, RequiredNpcOrGo1 AS entry FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo1 > 0
  UNION ALL SELECT ID, RequiredNpcOrGo2 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo2 > 0
  UNION ALL SELECT ID, RequiredNpcOrGo3 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo3 > 0
  UNION ALL SELECT ID, RequiredNpcOrGo4 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo4 > 0
) req ON req.ID = qt.ID
JOIN tmp_removed_creature_entries rce ON rce.entry = req.entry
ORDER BY qt.ID, req.entry;

SELECT DISTINCT 'quest_objective_gameobject' AS ref_type, qt.ID AS quest, req.entry
FROM `default.dataset.world.dest`.`quest_template` qt
JOIN (
  SELECT ID, -RequiredNpcOrGo1 AS entry FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo1 < 0
  UNION ALL SELECT ID, -RequiredNpcOrGo2 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo2 < 0
  UNION ALL SELECT ID, -RequiredNpcOrGo3 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo3 < 0
  UNION ALL SELECT ID, -RequiredNpcOrGo4 FROM `default.dataset.world.dest`.`quest_template` WHERE RequiredNpcOrGo4 < 0
) req ON req.ID = qt.ID
JOIN tmp_removed_gameobject_entries rge ON rge.entry = req.entry
ORDER BY qt.ID, req.entry;

SELECT DISTINCT 'item_template_map_restriction' AS ref_type, it.entry, it.name, it.`Map`
FROM `default.dataset.world.dest`.`item_template` it
JOIN tmp_removed_maps m ON m.map = it.`Map`
ORDER BY it.entry;

DELETE c
FROM `default.dataset.world.dest`.`conditions` c
JOIN tmp_deleted_smart_scripts dss
  ON c.SourceTypeOrReferenceId = 22
 AND c.SourceEntry = dss.entryorguid
 AND c.SourceId = dss.source_type;

DELETE c
FROM `default.dataset.world.dest`.`conditions` c
JOIN tmp_deleted_timed_actionlists dt
  ON c.SourceTypeOrReferenceId = 22
 AND c.SourceEntry = dt.entry
 AND c.SourceId = 9;

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_smart_scripts dss
  ON dss.entryorguid = ss.entryorguid
 AND dss.source_type = ss.source_type
 AND dss.id = ss.id
 AND dss.link = ss.link;

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
JOIN tmp_deleted_timed_actionlists dt
  ON dt.entry = ss.entryorguid
WHERE ss.source_type = 9;

DELETE ws
FROM `default.dataset.world.dest`.`waypoint_scripts` ws
JOIN tmp_removed_waypoint_script_ids rws ON rws.id = ws.id;

DELETE wd
FROM `default.dataset.world.dest`.`waypoint_data` wd
JOIN tmp_removed_creature_path_ids rp ON rp.path_id = wd.id;

DELETE ca
FROM `default.dataset.world.dest`.`creature_addon` ca
JOIN tmp_removed_creature_guids rc ON rc.guid = ca.guid;

DELETE cmo
FROM `default.dataset.world.dest`.`creature_movement_override` cmo
JOIN tmp_removed_creature_guids rc ON rc.guid = cmo.SpawnId;

DELETE cf
FROM `default.dataset.world.dest`.`creature_formations` cf
JOIN tmp_removed_creature_guids rc
  ON rc.guid = cf.leaderGUID
  OR rc.guid = cf.memberGUID;

DELETE lr
FROM `default.dataset.world.dest`.`linked_respawn` lr
LEFT JOIN tmp_removed_creature_guids rc
  ON rc.guid = lr.guid
LEFT JOIN tmp_removed_gameobject_guids rg
  ON rg.guid = lr.guid
LEFT JOIN tmp_removed_creature_guids_linked rcl
  ON rcl.guid = lr.linkedGuid
LEFT JOIN tmp_removed_gameobject_guids_linked rgl
  ON rgl.guid = lr.linkedGuid
WHERE (lr.linkType IN (0,1) AND rc.guid IS NOT NULL)
   OR (lr.linkType IN (2,3) AND rg.guid IS NOT NULL)
   OR (lr.linkType IN (0,3) AND rcl.guid IS NOT NULL)
   OR (lr.linkType IN (1,2) AND rgl.guid IS NOT NULL);

DELETE gec
FROM `default.dataset.world.dest`.`game_event_creature` gec
JOIN tmp_removed_creature_guids rc ON rc.guid = gec.guid;

DELETE gem
FROM `default.dataset.world.dest`.`game_event_model_equip` gem
JOIN tmp_removed_creature_guids rc ON rc.guid = gem.guid;

DELETE genv
FROM `default.dataset.world.dest`.`game_event_npc_vendor` genv
JOIN tmp_removed_creature_guids rc ON rc.guid = genv.guid;

DELETE gef
FROM `default.dataset.world.dest`.`game_event_npcflag` gef
JOIN tmp_removed_creature_guids rc ON rc.guid = gef.guid;

DELETE va
FROM `default.dataset.world.dest`.`vehicle_accessory` va
JOIN tmp_removed_creature_guids rc ON rc.guid = va.guid;

DELETE ga
FROM `default.dataset.world.dest`.`gameobject_addon` ga
JOIN tmp_removed_gameobject_guids rg ON rg.guid = ga.guid;

DELETE geg
FROM `default.dataset.world.dest`.`game_event_gameobject` geg
JOIN tmp_removed_gameobject_guids rg ON rg.guid = geg.guid;

DELETE sg
FROM `default.dataset.world.dest`.`spawn_group` sg
WHERE (sg.spawnType = 0 AND sg.spawnId IN (SELECT guid FROM tmp_removed_creature_guids))
   OR (sg.spawnType = 1 AND sg.spawnId IN (SELECT guid FROM tmp_removed_gameobject_guids));

DELETE isg
FROM `default.dataset.world.dest`.`instance_spawn_groups` isg
JOIN tmp_removed_maps m ON m.map = isg.instanceMapId;

DELETE sgt
FROM `default.dataset.world.dest`.`spawn_group_template` sgt
JOIN tmp_removed_spawn_group_ids rsg ON rsg.groupId = sgt.groupId
WHERE NOT EXISTS (SELECT 1 FROM `default.dataset.world.dest`.`spawn_group` sg WHERE sg.groupId = sgt.groupId)
  AND NOT EXISTS (SELECT 1 FROM `default.dataset.world.dest`.`instance_spawn_groups` isg WHERE isg.spawnGroupId = sgt.groupId);

DELETE ibc
FROM `default.dataset.world.dest`.`instance_boss_creature` ibc
JOIN tmp_removed_creature_guids rc ON rc.guid = ibc.guid;

DELETE c
FROM `default.dataset.world.dest`.`creature` c
JOIN tmp_removed_creature_guids rc ON rc.guid = c.guid;

DELETE g
FROM `default.dataset.world.dest`.`gameobject` g
JOIN tmp_removed_gameobject_guids rg ON rg.guid = g.guid;

DELETE gt
FROM `default.dataset.world.dest`.`game_tele` gt
JOIN tmp_removed_maps m ON m.map = gt.map;

DELETE ar
FROM `default.dataset.world.dest`.`access_requirement` ar
JOIN tmp_removed_maps m ON m.map = ar.mapId;

DELETE qpp
FROM `default.dataset.world.dest`.`quest_poi_points` qpp
JOIN tmp_removed_quest_poi rqp
  ON rqp.QuestID = qpp.QuestID
 AND rqp.id = qpp.Idx1;

DELETE qp
FROM `default.dataset.world.dest`.`quest_poi` qp
JOIN tmp_removed_quest_poi rqp
  ON rqp.QuestID = qp.QuestID
 AND rqp.id = qp.id;

DELETE stp
FROM `default.dataset.world.dest`.`spell_target_position` stp
JOIN tmp_removed_maps m ON m.map = stp.MapID;

DELETE pci
FROM `default.dataset.world.dest`.`playercreateinfo` pci
JOIN tmp_removed_maps m ON m.map = pci.map;

DELETE bdo
FROM `default.dataset.world.dest`.`battleground_door_object` bdo
JOIN tmp_removed_maps m ON m.map = bdo.map;

DELETE cht
FROM `default.dataset.world.dest`.`creature_health_tuning` cht
JOIN tmp_removed_maps m ON m.map = cht.MapID;

DELETE ia
FROM `default.dataset.world.dest`.`instance_addon` ia
JOIN tmp_removed_maps m ON m.map = ia.map;

DELETE ibb
FROM `default.dataset.world.dest`.`instance_boss_boundary` ibb
JOIN tmp_removed_maps m ON m.map = ibb.map;

DELETE ido
FROM `default.dataset.world.dest`.`instance_door_object` ido
JOIN tmp_removed_maps m ON m.map = ido.map;

DELETE iea
FROM `default.dataset.world.dest`.`instance_encounter_achievement` iea
JOIN tmp_removed_maps m ON m.map = iea.map;

DELETE ihp
FROM `default.dataset.world.dest`.`instance_health_profile` ihp
JOIN tmp_removed_maps m ON m.map = ihp.MapID;

DELETE it
FROM `default.dataset.world.dest`.`instance_template` it
JOIN tmp_removed_maps m ON m.map = it.map;

DELETE adbc
FROM `default.dataset.world.dest`.`achievement_dbc` adbc
JOIN tmp_removed_maps m ON m.map = adbc.mapID;

DELETE md
FROM `default.dataset.world.dest`.`map_dbc` md
JOIN tmp_removed_maps m
  ON m.map = md.ID
  OR m.map = md.CorpseMapID;

-- Post-check. All should be zero.
SELECT 'creature' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`creature` c JOIN tmp_removed_maps m ON m.map = c.map;
SELECT 'gameobject' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`gameobject` g JOIN tmp_removed_maps m ON m.map = g.map;
SELECT 'game_tele' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`game_tele` gt JOIN tmp_removed_maps m ON m.map = gt.map;
SELECT 'access_requirement' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`access_requirement` ar JOIN tmp_removed_maps m ON m.map = ar.mapId;
SELECT 'quest_poi' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`quest_poi` qp JOIN tmp_removed_maps m ON m.map = qp.MapID;
SELECT 'spell_target_position' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`spell_target_position` stp JOIN tmp_removed_maps m ON m.map = stp.MapID;
SELECT 'instance_template' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`instance_template` it JOIN tmp_removed_maps m ON m.map = it.map;
SELECT 'instance_spawn_groups' AS table_name, COUNT(*) AS remaining_rows FROM `default.dataset.world.dest`.`instance_spawn_groups` isg JOIN tmp_removed_maps m ON m.map = isg.instanceMapId;
SELECT 'map_dbc' AS table_name, COUNT(DISTINCT md.ID) AS remaining_rows
FROM `default.dataset.world.dest`.`map_dbc` md
JOIN tmp_removed_maps m
  ON m.map = md.ID
  OR m.map = md.CorpseMapID;
