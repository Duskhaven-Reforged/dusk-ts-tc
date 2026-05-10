-- Cleanup for DBErrors archetypes 1-3:
-- 1. Orphan spawn addon/event/pool/spawn-group/SmartAI rows.
-- 2. Removed quest leftovers and SmartAI quest references.
-- 3. Creature/skinning loot ids and stale loot conditions.
--
-- This project uses the generated .dest schema, not acore_world.
-- Keep this file outside an explicit transaction because the module runner wraps updates.
-- Do not USE the target schema; update runners check/write `updates` in the
-- connection database. Fully qualify .dest tables instead.

-- ---------------------------------------------------------------------------
-- 1. Orphan side tables for removed creature/gameobject spawns.
-- ---------------------------------------------------------------------------

DELETE ca
FROM `default.dataset.world.dest`.`creature_addon` ca
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = ca.`guid`
WHERE c.`guid` IS NULL;

DELETE ga
FROM `default.dataset.world.dest`.`gameobject_addon` ga
LEFT JOIN `default.dataset.world.dest`.`gameobject` g ON g.`guid` = ga.`guid`
WHERE g.`guid` IS NULL;

DELETE gec
FROM `default.dataset.world.dest`.`game_event_creature` gec
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = gec.`guid`
WHERE c.`guid` IS NULL;

DELETE geg
FROM `default.dataset.world.dest`.`game_event_gameobject` geg
LEFT JOIN `default.dataset.world.dest`.`gameobject` g ON g.`guid` = geg.`guid`
WHERE g.`guid` IS NULL;

DELETE genv
FROM `default.dataset.world.dest`.`game_event_npc_vendor` genv
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = genv.`guid`
WHERE c.`guid` IS NULL;

DELETE geme
FROM `default.dataset.world.dest`.`game_event_model_equip` geme
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = geme.`guid`
WHERE c.`guid` IS NULL;

DELETE genf
FROM `default.dataset.world.dest`.`game_event_npcflag` genf
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = genf.`guid`
WHERE c.`guid` IS NULL;

DELETE sg
FROM `default.dataset.world.dest`.`spawn_group` sg
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = sg.`spawnId`
WHERE sg.`spawnType` = 0
  AND c.`guid` IS NULL;

DELETE sg
FROM `default.dataset.world.dest`.`spawn_group` sg
LEFT JOIN `default.dataset.world.dest`.`gameobject` g ON g.`guid` = sg.`spawnId`
WHERE sg.`spawnType` = 1
  AND g.`guid` IS NULL;

DELETE pm
FROM `default.dataset.world.dest`.`pool_members` pm
LEFT JOIN `default.dataset.world.dest`.`creature` c ON c.`guid` = pm.`spawnId`
WHERE pm.`type` = 0
  AND c.`guid` IS NULL;

DELETE pm
FROM `default.dataset.world.dest`.`pool_members` pm
LEFT JOIN `default.dataset.world.dest`.`gameobject` g ON g.`guid` = pm.`spawnId`
WHERE pm.`type` = 1
  AND g.`guid` IS NULL;

DELETE sgt
FROM `default.dataset.world.dest`.`spawn_group_template` sgt
LEFT JOIN `default.dataset.world.dest`.`spawn_group` sg ON sg.`groupId` = sgt.`groupId`
LEFT JOIN `default.dataset.world.dest`.`instance_spawn_groups` isg ON isg.`spawnGroupId` = sgt.`groupId`
WHERE sg.`groupId` IS NULL
  AND isg.`spawnGroupId` IS NULL;

DELETE pt
FROM `default.dataset.world.dest`.`pool_template` pt
LEFT JOIN `default.dataset.world.dest`.`pool_members` pm ON pm.`poolSpawnId` = pt.`entry`
LEFT JOIN `default.dataset.world.dest`.`game_event_pool` gep ON gep.`pool_entry` = pt.`entry`
WHERE pm.`poolSpawnId` IS NULL
  AND gep.`pool_entry` IS NULL;

-- SmartAI rows attached to removed objects, plus rows the core skips because
-- the template does not use SmartAI.
DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN `default.dataset.world.dest`.`smart_scripts` ss
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = ss.`entryorguid`
 AND cnd.`SourceId` = ss.`source_type`
 AND cnd.`SourceGroup` = ss.`id`
LEFT JOIN `default.dataset.world.dest`.`creature_template` ct
  ON ss.`source_type` = 0
 AND ss.`entryorguid` > 0
 AND ct.`entry` = ss.`entryorguid`
LEFT JOIN `default.dataset.world.dest`.`creature` c
  ON ss.`source_type` = 0
 AND ss.`entryorguid` < 0
 AND c.`guid` = -ss.`entryorguid`
WHERE ss.`source_type` = 0
  AND (
       (ss.`entryorguid` > 0 AND (ct.`entry` IS NULL OR ct.`AIName` <> 'SmartAI'))
    OR (ss.`entryorguid` < 0 AND c.`guid` IS NULL)
  );

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
LEFT JOIN `default.dataset.world.dest`.`creature_template` ct
  ON ss.`source_type` = 0
 AND ss.`entryorguid` > 0
 AND ct.`entry` = ss.`entryorguid`
LEFT JOIN `default.dataset.world.dest`.`creature` c
  ON ss.`source_type` = 0
 AND ss.`entryorguid` < 0
 AND c.`guid` = -ss.`entryorguid`
WHERE ss.`source_type` = 0
  AND (
       (ss.`entryorguid` > 0 AND (ct.`entry` IS NULL OR ct.`AIName` <> 'SmartAI'))
    OR (ss.`entryorguid` < 0 AND c.`guid` IS NULL)
  );

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN `default.dataset.world.dest`.`smart_scripts` ss
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = ss.`entryorguid`
 AND cnd.`SourceId` = ss.`source_type`
 AND cnd.`SourceGroup` = ss.`id`
LEFT JOIN `default.dataset.world.dest`.`gameobject_template` gt
  ON ss.`source_type` = 1
 AND ss.`entryorguid` > 0
 AND gt.`entry` = ss.`entryorguid`
LEFT JOIN `default.dataset.world.dest`.`gameobject` g
  ON ss.`source_type` = 1
 AND ss.`entryorguid` < 0
 AND g.`guid` = -ss.`entryorguid`
WHERE ss.`source_type` = 1
  AND (
       (ss.`entryorguid` > 0 AND gt.`entry` IS NULL)
    OR (ss.`entryorguid` < 0 AND g.`guid` IS NULL)
  );

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
LEFT JOIN `default.dataset.world.dest`.`gameobject_template` gt
  ON ss.`source_type` = 1
 AND ss.`entryorguid` > 0
 AND gt.`entry` = ss.`entryorguid`
LEFT JOIN `default.dataset.world.dest`.`gameobject` g
  ON ss.`source_type` = 1
 AND ss.`entryorguid` < 0
 AND g.`guid` = -ss.`entryorguid`
WHERE ss.`source_type` = 1
  AND (
       (ss.`entryorguid` > 0 AND gt.`entry` IS NULL)
    OR (ss.`entryorguid` < 0 AND g.`guid` IS NULL)
  );

-- Broken SmartAI event links: clear bad outgoing links, then remove LINK rows
-- that no remaining event points at.
UPDATE `default.dataset.world.dest`.`smart_scripts` src
LEFT JOIN `default.dataset.world.dest`.`smart_scripts` dst
  ON dst.`entryorguid` = src.`entryorguid`
 AND dst.`source_type` = src.`source_type`
 AND dst.`id` = src.`link`
SET src.`link` = 0
WHERE src.`link` <> 0
  AND dst.`entryorguid` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN `default.dataset.world.dest`.`smart_scripts` linkrow
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = linkrow.`entryorguid`
 AND cnd.`SourceId` = linkrow.`source_type`
 AND cnd.`SourceGroup` = linkrow.`id`
LEFT JOIN `default.dataset.world.dest`.`smart_scripts` src
  ON src.`entryorguid` = linkrow.`entryorguid`
 AND src.`source_type` = linkrow.`source_type`
 AND src.`link` = linkrow.`id`
WHERE linkrow.`event_type` = 61
  AND src.`entryorguid` IS NULL;

DELETE linkrow
FROM `default.dataset.world.dest`.`smart_scripts` linkrow
LEFT JOIN `default.dataset.world.dest`.`smart_scripts` src
  ON src.`entryorguid` = linkrow.`entryorguid`
 AND src.`source_type` = linkrow.`source_type`
 AND src.`link` = linkrow.`id`
WHERE linkrow.`event_type` = 61
  AND src.`entryorguid` IS NULL;

-- ---------------------------------------------------------------------------
-- 2. Removed quest leftovers.
-- ---------------------------------------------------------------------------

DELETE qpm
FROM `default.dataset.world.dest`.`quest_pool_members` qpm
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = qpm.`questId`
WHERE qt.`ID` IS NULL;

DELETE qpt
FROM `default.dataset.world.dest`.`quest_pool_template` qpt
LEFT JOIN `default.dataset.world.dest`.`quest_pool_members` qpm ON qpm.`poolId` = qpt.`poolId`
WHERE qpm.`poolId` IS NULL;

DELETE cqs
FROM `default.dataset.world.dest`.`creature_queststarter` cqs
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = cqs.`quest`
WHERE qt.`ID` IS NULL;

DELETE cqe
FROM `default.dataset.world.dest`.`creature_questender` cqe
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = cqe.`quest`
WHERE qt.`ID` IS NULL;

DELETE gqs
FROM `default.dataset.world.dest`.`gameobject_queststarter` gqs
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = gqs.`quest`
WHERE qt.`ID` IS NULL;

DELETE gqe
FROM `default.dataset.world.dest`.`gameobject_questender` gqe
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = gqe.`quest`
WHERE qt.`ID` IS NULL;

DELETE qpp
FROM `default.dataset.world.dest`.`quest_poi_points` qpp
LEFT JOIN `default.dataset.world.dest`.`quest_poi` qp
  ON qp.`QuestID` = qpp.`QuestID`
 AND qp.`id` = qpp.`Idx1`
WHERE qp.`QuestID` IS NULL;

DELETE qp
FROM `default.dataset.world.dest`.`quest_poi` qp
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = qp.`QuestID`
WHERE qt.`ID` IS NULL;

DELETE qta
FROM `default.dataset.world.dest`.`quest_template_addon` qta
LEFT JOIN `default.dataset.world.dest`.`quest_template` qt ON qt.`ID` = qta.`ID`
WHERE qt.`ID` IS NULL;

UPDATE `default.dataset.world.dest`.`quest_template` qt
LEFT JOIN `default.dataset.world.dest`.`quest_template` nextq ON nextq.`ID` = qt.`RewardNextQuest`
SET qt.`RewardNextQuest` = 0
WHERE qt.`RewardNextQuest` <> 0
  AND nextq.`ID` IS NULL;

UPDATE `default.dataset.world.dest`.`quest_template_addon` qta
LEFT JOIN `default.dataset.world.dest`.`quest_template` prevq ON prevq.`ID` = ABS(qta.`PrevQuestID`)
SET qta.`PrevQuestID` = 0
WHERE qta.`PrevQuestID` <> 0
  AND prevq.`ID` IS NULL;

UPDATE `default.dataset.world.dest`.`quest_template_addon` qta
LEFT JOIN `default.dataset.world.dest`.`quest_template` nextq ON nextq.`ID` = ABS(qta.`NextQuestID`)
SET qta.`NextQuestID` = 0
WHERE qta.`NextQuestID` <> 0
  AND nextq.`ID` IS NULL;

UPDATE `default.dataset.world.dest`.`quest_template_addon` qta
LEFT JOIN `default.dataset.world.dest`.`quest_template` breadq ON breadq.`ID` = qta.`BreadcrumbForQuestId`
SET qta.`BreadcrumbForQuestId` = 0
WHERE qta.`BreadcrumbForQuestId` <> 0
  AND breadq.`ID` IS NULL;

-- SmartAI quest references:
-- events 19/20 = accepted/reward quest; actions 6/7/15/26 = fail/offer/credit quest.
DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN `default.dataset.world.dest`.`smart_scripts` ss
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = ss.`entryorguid`
 AND cnd.`SourceId` = ss.`source_type`
 AND cnd.`SourceGroup` = ss.`id`
LEFT JOIN `default.dataset.world.dest`.`quest_template` eq ON eq.`ID` = ss.`event_param1`
WHERE ss.`event_type` IN (19, 20)
  AND ss.`event_param1` <> 0
  AND eq.`ID` IS NULL;

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
LEFT JOIN `default.dataset.world.dest`.`quest_template` eq ON eq.`ID` = ss.`event_param1`
WHERE ss.`event_type` IN (19, 20)
  AND ss.`event_param1` <> 0
  AND eq.`ID` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
JOIN `default.dataset.world.dest`.`smart_scripts` ss
  ON cnd.`SourceTypeOrReferenceId` = 22
 AND cnd.`SourceEntry` = ss.`entryorguid`
 AND cnd.`SourceId` = ss.`source_type`
 AND cnd.`SourceGroup` = ss.`id`
LEFT JOIN `default.dataset.world.dest`.`quest_template` aq ON aq.`ID` = ss.`action_param1`
WHERE ss.`action_type` IN (6, 7, 15, 26)
  AND ss.`action_param1` <> 0
  AND aq.`ID` IS NULL;

DELETE ss
FROM `default.dataset.world.dest`.`smart_scripts` ss
LEFT JOIN `default.dataset.world.dest`.`quest_template` aq ON aq.`ID` = ss.`action_param1`
WHERE ss.`action_type` IN (6, 7, 15, 26)
  AND ss.`action_param1` <> 0
  AND aq.`ID` IS NULL;

-- ---------------------------------------------------------------------------
-- 3. Loot cleanup.
-- ---------------------------------------------------------------------------

UPDATE `default.dataset.world.dest`.`creature_template` ct
LEFT JOIN `default.dataset.world.dest`.`creature_loot_template` clt ON clt.`Entry` = ct.`lootid`
SET ct.`lootid` = 0
WHERE ct.`lootid` <> 0
  AND clt.`Entry` IS NULL;

UPDATE `default.dataset.world.dest`.`creature_template` ct
LEFT JOIN `default.dataset.world.dest`.`skinning_loot_template` slt ON slt.`Entry` = ct.`skinloot`
SET ct.`skinloot` = 0
WHERE ct.`skinloot` <> 0
  AND slt.`Entry` IS NULL;

UPDATE `default.dataset.world.dest`.`creature_template` ct
LEFT JOIN `default.dataset.world.dest`.`pickpocketing_loot_template` plt ON plt.`Entry` = ct.`pickpocketloot`
SET ct.`pickpocketloot` = 0
WHERE ct.`pickpocketloot` <> 0
  AND plt.`Entry` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
LEFT JOIN `default.dataset.world.dest`.`creature_loot_template` clt
  ON clt.`Entry` = cnd.`SourceGroup`
 AND clt.`Item` = cnd.`SourceEntry`
WHERE cnd.`SourceTypeOrReferenceId` = 1
  AND clt.`Entry` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
LEFT JOIN `default.dataset.world.dest`.`gameobject_loot_template` glt
  ON glt.`Entry` = cnd.`SourceGroup`
 AND glt.`Item` = cnd.`SourceEntry`
WHERE cnd.`SourceTypeOrReferenceId` = 4
  AND glt.`Entry` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
LEFT JOIN `default.dataset.world.dest`.`reference_loot_template` rlt
  ON rlt.`Entry` = cnd.`SourceGroup`
 AND rlt.`Item` = cnd.`SourceEntry`
WHERE cnd.`SourceTypeOrReferenceId` = 10
  AND rlt.`Entry` IS NULL;

DELETE cnd
FROM `default.dataset.world.dest`.`conditions` cnd
LEFT JOIN `default.dataset.world.dest`.`skinning_loot_template` slt
  ON slt.`Entry` = cnd.`SourceGroup`
 AND slt.`Item` = cnd.`SourceEntry`
WHERE cnd.`SourceTypeOrReferenceId` = 11
  AND slt.`Entry` IS NULL;

-- Exact unused loot template rows from DBErrors audit/current .dest state.
-- Keep these as fixed ID lists. The broad anti-join version is too slow during build.
DELETE FROM `default.dataset.world.dest`.`creature_loot_template`
WHERE `Entry` IN (20047,28189);

DELETE FROM `default.dataset.world.dest`.`skinning_loot_template`
WHERE `Entry` IN (
  60000,60001,60002,60004,60005,60006,60007,60008,60009,60010,
  60011,60012,60013,60014,60015,60016,60017,60018,60019,60020,
  60021,60022,60023,60024,60025,60026,60027,60028,60029,60030,
  60031,60032,60033,60034,60035,60036,60037,60038,60039,60040,
  60041,60042,60043,60044,60045,60046,60047,60048,60049,60050,
  60051,60052,60053,60054,60055,60056,60057,60058,60059,60060,
  60062,60063,60064,60065,60066,60067,60069,60070,60071,60072,
  60073,60074,60075,60076,60077,60078
);

DELETE FROM `default.dataset.world.dest`.`reference_loot_template`
WHERE `Entry` IN (
  4000,4001,4002,4100,4101,4102,4103,4104,4105,4106,
  4110,4111,4112,4200,4201,4202,4203,4204,4205,4208,
  4209,4210,34106,34107,34108,34109,34111,34113,34114,34155,
  34377,35064,35065,35066,35067,35068,35070,35072,35074,35077,
  35078,35079,35080,35081,35082,35084
);
