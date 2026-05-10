-- Auto-generated DBErrors cleanup.
-- Scope: current `default.dataset.world.dest` world schema only.

-- StartItem set but StartItemCount missing. This core stores StartItemCount as quest_template_addon.ProvidedItemCount.
INSERT INTO `default.dataset.world.dest`.`quest_template_addon` (ID, ProvidedItemCount) VALUES
    (26462, 1),
    (26450, 1),
    (26446, 1),
    (26441, 1),
    (26401, 1),
    (26397, 1),
    (26385, 1),
    (26367, 1),
    (26344, 1),
    (26333, 1),
    (26325, 1),
    (26321, 1),
    (26314, 1),
    (26302, 1),
    (26299, 1),
    (26298, 1),
    (26132, 1),
    (26107, 1),
    (965, 1)
ON DUPLICATE KEY UPDATE ProvidedItemCount = VALUES(ProvidedItemCount);

-- StartItemCount set while no StartItem exists.
UPDATE `default.dataset.world.dest`.`quest_template_addon`
SET ProvidedItemCount = 0
WHERE ID IN (26159, 26154, 26148);

-- Daily/weekly quests must be repeatable in quest_template_addon.SpecialFlags.
INSERT INTO `default.dataset.world.dest`.`quest_template_addon` (ID, SpecialFlags) VALUES
    (26408, 1),
    (26409, 1),
    (26407, 1),
    (26406, 1),
    (26405, 1),
    (26404, 1),
    (26392, 1),
    (26393, 1)
ON DUPLICATE KEY UPDATE SpecialFlags = SpecialFlags | VALUES(SpecialFlags);

-- RewardNextQuest points at retired quest ids.
UPDATE `default.dataset.world.dest`.`quest_template`
SET RewardNextQuest = 0
WHERE (ID, RewardNextQuest) IN (
    (1899, 14418),
    (1886, 14419),
    (1898, 14421)
);

-- StartItem ids no longer exist.
UPDATE `default.dataset.world.dest`.`quest_template`
SET StartItem = 0
WHERE (ID, StartItem) IN (
    (9695, 24228),
    (9308, 22989),
    (9507, 23740)
);

UPDATE `default.dataset.world.dest`.`quest_template_addon`
SET ProvidedItemCount = 0
WHERE ID IN (9695, 9308, 9507);

-- Quest starters missing QUEST_GIVER npcflag.
UPDATE `default.dataset.world.dest`.`creature_template`
SET npcflag = npcflag | 2
WHERE entry IN (3685);

-- Empty quest pool member rows.
DELETE FROM `default.dataset.world.dest`.`quest_pool_members`
WHERE (poolIndex, poolId) IN (
    (1, 5708),
    (0, 5708)
);

-- Script commands referencing removed quests.
DELETE FROM `default.dataset.world.dest`.`event_scripts`
WHERE (id, datalong) IN (
    (10923, 14484),
    (9718, 11225),
    (12028, 17609)
);
