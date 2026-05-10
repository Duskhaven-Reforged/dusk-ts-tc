-- Cleanup for DBErrors group 3: creature template/spawn reference issues.
-- Keep .dest fully qualified so update registration stays in the runner DB.

-- Remove addon/equipment rows for creature templates that no longer exist.
DELETE cta
FROM `default.dataset.world.dest`.`creature_template_addon` cta
LEFT JOIN `default.dataset.world.dest`.`creature_template` ct ON ct.`entry` = cta.`entry`
WHERE ct.`entry` IS NULL;

DELETE cet
FROM `default.dataset.world.dest`.`creature_equip_template` cet
LEFT JOIN `default.dataset.world.dest`.`creature_template` ct ON ct.`entry` = cet.`CreatureID`
WHERE ct.`entry` IS NULL;

-- Quest starter rows imply questgiver npcflag.
UPDATE `default.dataset.world.dest`.`creature_template` ct
JOIN (
  SELECT DISTINCT `id`
  FROM `default.dataset.world.dest`.`creature_queststarter`
) qs ON qs.`id` = ct.`entry`
SET ct.`npcflag` = ct.`npcflag` | 2
WHERE (ct.`npcflag` & 2) = 0;

-- Difficulty templates inherit script/AI from their normal-mode template.
UPDATE `default.dataset.world.dest`.`creature_template` base
JOIN `default.dataset.world.dest`.`creature_template` diff
  ON diff.`entry` IN (base.`difficulty_entry_1`, base.`difficulty_entry_2`, base.`difficulty_entry_3`)
SET diff.`AIName` = ''
WHERE diff.`AIName` <> '';

UPDATE `default.dataset.world.dest`.`creature_template` base
JOIN `default.dataset.world.dest`.`creature_template` diff
  ON diff.`entry` IN (base.`difficulty_entry_1`, base.`difficulty_entry_2`, base.`difficulty_entry_3`)
SET diff.`ScriptName` = ''
WHERE diff.`ScriptName` <> '';

-- Difficulty entries should not have lower level/faction data than base mode.
UPDATE `default.dataset.world.dest`.`creature_template` base
JOIN `default.dataset.world.dest`.`creature_template` diff
  ON diff.`entry` IN (base.`difficulty_entry_1`, base.`difficulty_entry_2`, base.`difficulty_entry_3`)
SET diff.`minlevel` = base.`minlevel`
WHERE diff.`minlevel` < base.`minlevel`;

UPDATE `default.dataset.world.dest`.`creature_template` base
JOIN `default.dataset.world.dest`.`creature_template` diff
  ON diff.`entry` IN (base.`difficulty_entry_1`, base.`difficulty_entry_2`, base.`difficulty_entry_3`)
SET diff.`maxlevel` = base.`maxlevel`
WHERE diff.`maxlevel` < base.`maxlevel`;

UPDATE `default.dataset.world.dest`.`creature_template` base
JOIN `default.dataset.world.dest`.`creature_template` diff
  ON diff.`entry` IN (base.`difficulty_entry_1`, base.`difficulty_entry_2`, base.`difficulty_entry_3`)
SET diff.`faction` = base.`faction`
WHERE diff.`faction` <> base.`faction`;

-- Broken difficulty graph rows are invalid references. Clear direct self loops.
UPDATE `default.dataset.world.dest`.`creature_template`
SET `difficulty_entry_1` = 0
WHERE `difficulty_entry_1` = `entry`;

UPDATE `default.dataset.world.dest`.`creature_template`
SET `difficulty_entry_2` = 0
WHERE `difficulty_entry_2` = `entry`;

UPDATE `default.dataset.world.dest`.`creature_template`
SET `difficulty_entry_3` = 0
WHERE `difficulty_entry_3` = `entry`;
