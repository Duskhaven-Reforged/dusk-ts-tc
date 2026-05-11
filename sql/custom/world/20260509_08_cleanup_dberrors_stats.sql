-- Cleanup for DBErrors group 6: player/creature stat gaps.
-- Mirrors runtime fallback: missing player levels copy nearest lower level.

DROP TEMPORARY TABLE IF EXISTS tmp_levels_1_80;
CREATE TEMPORARY TABLE tmp_levels_1_80 (
  `level` TINYINT UNSIGNED NOT NULL PRIMARY KEY
) ENGINE=Memory;

INSERT INTO tmp_levels_1_80 (`level`) VALUES
(1),(2),(3),(4),(5),(6),(7),(8),(9),(10),
(11),(12),(13),(14),(15),(16),(17),(18),(19),(20),
(21),(22),(23),(24),(25),(26),(27),(28),(29),(30),
(31),(32),(33),(34),(35),(36),(37),(38),(39),(40),
(41),(42),(43),(44),(45),(46),(47),(48),(49),(50),
(51),(52),(53),(54),(55),(56),(57),(58),(59),(60),
(61),(62),(63),(64),(65),(66),(67),(68),(69),(70),
(71),(72),(73),(74),(75),(76),(77),(78),(79),(80);

INSERT IGNORE INTO `player_levelstats`
  (`race`,`class`,`level`,`str`,`agi`,`sta`,`inte`,`spi`)
SELECT rc.`race`, rc.`class`, lvl.`level`,
       src.`str`, src.`agi`, src.`sta`, src.`inte`, src.`spi`
FROM (
  SELECT DISTINCT `race`, `class`
  FROM `player_levelstats`
  WHERE `level` = 1
) rc
JOIN tmp_levels_1_80 lvl ON lvl.`level` > 1
LEFT JOIN `player_levelstats` existing
  ON existing.`race` = rc.`race`
 AND existing.`class` = rc.`class`
 AND existing.`level` = lvl.`level`
JOIN `player_levelstats` src
  ON src.`race` = rc.`race`
 AND src.`class` = rc.`class`
 AND src.`level` = (
   SELECT MAX(prev.`level`)
   FROM `player_levelstats` prev
   WHERE prev.`race` = rc.`race`
     AND prev.`class` = rc.`class`
     AND prev.`level` < lvl.`level`
 )
WHERE existing.`race` IS NULL;

UPDATE `creature_template`
SET `minlevel` = 1
WHERE `minlevel` = 0;

UPDATE `creature_template`
SET `maxlevel` = `minlevel`
WHERE `maxlevel` < `minlevel`;

DROP TEMPORARY TABLE IF EXISTS tmp_needed_creature_classlevelstats;
CREATE TEMPORARY TABLE tmp_needed_creature_classlevelstats (
  `level` TINYINT UNSIGNED NOT NULL,
  `class` TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (`level`, `class`)
) ENGINE=Memory;

INSERT IGNORE INTO tmp_needed_creature_classlevelstats (`level`, `class`)
SELECT DISTINCT lvl.`level`, ct.`unit_class`
FROM `creature_template` ct
JOIN tmp_levels_1_80 lvl ON lvl.`level` BETWEEN ct.`minlevel` AND ct.`maxlevel`
WHERE ct.`unit_class` > 0;

INSERT IGNORE INTO `creature_classlevelstats`
  (`level`,`class`,`basehp0`,`basehp1`,`basehp2`,`basehp3`,`basemana`,`basearmor`,
   `attackpower`,`rangedattackpower`,`damage_base`,`damage_exp1`,`damage_exp2`,`comment`)
SELECT need.`level`, need.`class`,
       src.`basehp0`, src.`basehp1`, src.`basehp2`, src.`basehp3`, src.`basemana`, src.`basearmor`,
       src.`attackpower`, src.`rangedattackpower`, src.`damage_base`, src.`damage_exp1`, src.`damage_exp2`,
       'Copied from nearest lower level by DBErrors cleanup'
FROM tmp_needed_creature_classlevelstats need
LEFT JOIN `creature_classlevelstats` existing
  ON existing.`level` = need.`level`
 AND existing.`class` = need.`class`
JOIN `creature_classlevelstats` src
  ON src.`class` = need.`class`
 AND src.`level` = (
   SELECT MAX(prev.`level`)
   FROM `creature_classlevelstats` prev
   WHERE prev.`class` = need.`class`
     AND prev.`level` < need.`level`
 )
WHERE existing.`level` IS NULL;

DROP TEMPORARY TABLE IF EXISTS tmp_needed_creature_classlevelstats;
DROP TEMPORARY TABLE IF EXISTS tmp_levels_1_80;
