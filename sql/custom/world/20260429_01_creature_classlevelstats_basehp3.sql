ALTER TABLE `creature_classlevelstats` ADD COLUMN `basehp3` int unsigned NOT NULL DEFAULT 1 AFTER `basehp2`;
UPDATE `creature_classlevelstats` SET `basehp3` = `basehp2`;
